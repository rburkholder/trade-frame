/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

/*
 * File:      Server.cpp
 * Author:    raymond@burkholder.net
 * Project:   TableTrader
 * Created:   2022/08/02 09:58:23
 */

#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/format.hpp>

#include "Server.hpp"
#include "Server_impl.hpp"

namespace {
  //const std::string sFormatFloat( "%0.*f" ); // boost::format does not process variable precision
  const std::string sFormatFloat( "%0.2f" );
}

Server::Server(
  int argc, char *argv[],
  const config::Choices& choices,
  const std::string &wtConfigurationFile
)
: Wt::WServer( argc, argv, wtConfigurationFile )
, m_choices( choices )
{
  m_implServer = std::make_unique<Server_impl>();
}

Server::~Server() {
}

bool Server::ValidateLogin( const std::string& sUserName, const std::string& sPassWord ) {
  return ( ( sUserName == m_choices.m_sUIUserName ) && ( sPassWord == m_choices.m_sUIPassWord ) );
}

void Server::AddCandidateFutures( fAddCandidateFutures_t&& f ) {
  //TODO: need to query Server_impl to see if something in progress, if so, skip to the point of sync
  for ( const std::string& s: m_choices.m_vCandidateFutures ) {
    f( s );
  }
}

void Server::Start(
    const std::string& sSessionId, const std::string& sUnderlyingFuture,
    fUpdateUnderlyingInfo_t&& fUpdateUnderlyingInfo,
    fUpdateUnderlyingPrice_t&& fUpdateUnderlyingPrice,
    fUpdateOptionExpiries_t&& fUpdateOptionExpiries,
    fUpdateOptionExpiriesDone_t&& fUpdateOptionExpiriesDone
) {
  assert( fUpdateUnderlyingInfo );
  m_fUpdateUnderlyingInfo = std::move( fUpdateUnderlyingInfo );

  assert( fUpdateUnderlyingPrice );
  m_fUpdateUnderlyingPrice = std::move( fUpdateUnderlyingPrice );

  assert( fUpdateOptionExpiries );
  m_fUpdateOptionExpiries = std::move( fUpdateOptionExpiries );

  assert( fUpdateOptionExpiriesDone );
  m_fUpdateOptionExpiriesDone = std::move( fUpdateOptionExpiriesDone );

  boost::format format( sFormatFloat );

  m_implServer->Start(
    sUnderlyingFuture,
    [this,sSessionId](const std::string& sName, int multiplier ) { // fUpdateUnderlyingInfo_t
      post(
        sSessionId,
        [this, sName_=std::move(sName), multiplier ]() {
          std::string sMultiplier = boost::lexical_cast<std::string>( multiplier );
          m_fUpdateUnderlyingInfo( sName_, sMultiplier );
        }
      );
    },
    [this,sSessionId,format_=std::move(format)]( double price, int precision) mutable { // fUpdateUnderlyingPrice_t
      //format_ % precision % price;
      format_ % price;
      std::string sPrice( format_.str() );
      post(
        sSessionId,
        [this,sPrice_=std::move(sPrice)](){
          m_fUpdateUnderlyingPrice( sPrice_ );
        }
      );
    },
    [this,sSessionId]( boost::gregorian::date date ){ // fAddExpiry_t
      post(
        sSessionId,
        [this,date](){
          //std::string sDate = boost::lexical_cast<std::string>( date );
          const std::string sDate = boost::gregorian::to_iso_string( date );
          m_fUpdateOptionExpiries( sDate );
        }
      );
    },
    [this,sSessionId](){ // fAddExpiryDone_t
      post(
        sSessionId,
        [this](){
          m_fUpdateOptionExpiriesDone();
        }
      );
    }
  );
}

void Server::PrepareStrikeSelection(
  const std::string& sDate,
  fPopulateStrike_t&& fPopulateStrike,
  fPopulateStrikeDone_t&& fPopulateStrikeDone
) {
  boost::gregorian::date date( boost::gregorian::date_from_iso_string( sDate ) );
  m_implServer->PopulateStrikes(
    date,
    [fPopulateStrike_=std::move(fPopulateStrike)]( double strike, int precision){
      boost::format format( "%0." + boost::lexical_cast<std::string>( precision ) + "f" );
      format % strike;
      fPopulateStrike_( format.str() );

      if ( false ) {
        try {
          format % precision % strike;
          fPopulateStrike_( format.str() );
        }
        catch( boost::io::too_many_args& e ) {
          BOOST_LOG_TRIVIAL(error) << "boost::format too many args " << strike << "," << precision ;
        }
        catch (boost::io::too_few_args& e ) {
          BOOST_LOG_TRIVIAL(error) << "boost::format too few args " << strike << "," << precision;
        }
      }
    },
    [fPopulateStrikeDone_=std::move(fPopulateStrikeDone)](){
      fPopulateStrikeDone_();
    }
  );
}

void Server::AddStrike( const std::string& ) {
}

void Server::DelStrike( const std::string& ) {
}

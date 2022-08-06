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

#include <boost/lexical_cast.hpp>

#include "Server.hpp"
#include "Server_impl.hpp"

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
    [this,sSessionId]( double price, int precision) { // fUpdateUnderlyingPrice_t
      post(
        sSessionId,
        [this, price](){
          std::string sPrice = boost::lexical_cast<std::string>( price );
          m_fUpdateUnderlyingPrice( sPrice );
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

void Server::PrepareStrikeSelection( const std::string& sDate ) {
  boost::gregorian::date date( boost::gregorian::date_from_iso_string( sDate ) );
}
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
 * File:      Server_impl.cpp
 * Author:    raymond@burkholder.net
 * Project:   TableTrader
 * Created:   2022/08/03 17:14:41
 */

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>

#include <TFTrading/DBWrapper.h>
#include <TFTrading/OrderManager.h>
#include <TFTrading/PortfolioManager.h>
#include <TFTrading/ProviderManager.h>
#include <TFTrading/InstrumentManager.h>

#include <TFTrading/BuildInstrument.h>

#include <TFIQFeed/Provider.h>
#include <TFInteractiveBrokers/IBTWS.h>

#include "Server_impl.hpp"

namespace {
  static const std::string sTWS( "tws" );
  static const std::string sIQFeed( "iqfeed" );
  static const std::string sDataBaseName( "TableTrader.db" );
}

Server_impl::Server_impl()
: m_state( EState::quiescent )
{

  ou::tf::ProviderManager& providers( ou::tf::ProviderManager::GlobalInstance() );

  //using pProviderTWS_t = ou::tf::ib::TWS::pProvider_t;
  m_pProviderTWS = ou::tf::ib::TWS::Factory();
  m_pProviderTWS->SetName( sTWS ); // needs to match name in database
  providers.Register( m_pProviderTWS );
  m_pProviderTWS->OnConnected.Add( MakeDelegate( this, &Server_impl::Connected_TWS ) );
  m_pProviderTWS->OnDisconnected.Add( MakeDelegate( this, &Server_impl::Disconnected_TWS ) );

  //using pProviderIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;
  m_pProviderIQFeed = ou::tf::iqfeed::IQFeedProvider::Factory();
  m_pProviderIQFeed->SetName( sIQFeed ); // needs to match name in database
  providers.Register( m_pProviderIQFeed );
  m_pProviderIQFeed->OnConnected.Add( MakeDelegate( this, &Server_impl::Connected_IQFeed ) );
  m_pProviderIQFeed->OnDisconnected.Add( MakeDelegate( this, &Server_impl::Disconnected_IQFeed ) );

  assert( m_pProviderTWS );
  assert( m_pProviderIQFeed );

  // for testing, start fresh each time
  if ( boost::filesystem::exists( sDataBaseName ) ) {
    boost::filesystem::remove( sDataBaseName );
  }

  m_pdb = std::make_unique<ou::tf::db>( sDataBaseName );

  m_pProviderTWS->Connect();
  m_pProviderIQFeed->Connect();

  m_state = EState::connecting;

}

Server_impl::~Server_impl() {
  if ( m_pWatchUnderlying ) {
    m_pWatchUnderlying->StopWatch();
    m_pWatchUnderlying->OnQuote.Remove( MakeDelegate( this, &Server_impl::UnderlyingQuote ) );
    m_pWatchUnderlying->OnTrade.Remove( MakeDelegate( this, &Server_impl::UnderlyingTrade ) );
  }
  m_pProviderTWS->Disconnect();
  m_pProviderIQFeed->Disconnect();
}

void Server_impl::Connected_TWS( int n ) {
  BOOST_LOG_TRIVIAL(info) << "Provider TWS connected";
  Connected( n );
}

void Server_impl::Connected_IQFeed( int n ) {
  BOOST_LOG_TRIVIAL(info) << "Provider IQFeed connected";
  Connected( n );
}

void Server_impl::Connected( int ) {
  if ( m_pProviderTWS->Connected() && m_pProviderIQFeed->Connected() ) {
    m_state = EState::connected;
    m_pBuildInstrument = std::make_unique<ou::tf::BuildInstrument>( m_pProviderIQFeed, m_pProviderTWS );
    // TODO: generate signal or status to interface
  }
}

void Server_impl::Disconnected_TWS( int n ) {
  BOOST_LOG_TRIVIAL(info) << "Provider TWS disconnected";
  Disconnected( n );
}

void Server_impl::Disconnected_IQFeed( int n ) {
  BOOST_LOG_TRIVIAL(info) << "Provider IQFeed disconnected";
  Disconnected( n );
}

void Server_impl::Disconnected( int ) {
  if ( !m_pProviderTWS->Connected() && !m_pProviderIQFeed->Connected() ) {
    m_state = EState::disconnected;
  }
}

void Server_impl::Start(
  const std::string& sUnderlyingFuture,
  fUpdateUnderlyingInfo_t&& fUpdateUnderlyingInfo,
  fUpdateUnderlyingPrice_t&& fUpdateUnderlyingPrice
) {

  assert( fUpdateUnderlyingInfo );
  assert( fUpdateUnderlyingPrice );

  m_fUpdateUnderlyingInfo = std::move( fUpdateUnderlyingInfo );
  m_fUpdateUnderlyingPrice = std::move( fUpdateUnderlyingPrice );

  m_pBuildInstrument->Queue(
    sUnderlyingFuture,
    [this]( pInstrument_t pInstrument ){
      UnderlyingInitialize( pInstrument );
    } );
}

void Server_impl::UnderlyingInitialize( pInstrument_t pInstrument ) {

  const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName() );
  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );

  if ( pm.PortfolioExists( idInstrument ) ) {
    m_pPortfolio = pm.GetPortfolio( idInstrument );
  }
  else {
    // TODO will need a portfolio per table instance
    m_pPortfolio
      = pm.ConstructPortfolio(
          idInstrument, "aoTF", "USD",
          ou::tf::Portfolio::EPortfolioType::Standard,
          ou::tf::Currency::Name[ ou::tf::Currency::USD ] );
  }

  m_pWatchUnderlying = std::make_shared<ou::tf::Watch>( pInstrument, m_pProviderIQFeed );
  m_pWatchUnderlying->OnQuote.Add( MakeDelegate( this, &Server_impl::UnderlyingQuote ) );
  m_pWatchUnderlying->OnTrade.Add( MakeDelegate( this, &Server_impl::UnderlyingTrade ) );
  m_pWatchUnderlying->OnFundamentals.Add( MakeDelegate( this, &Server_impl::UnderlyingFundamentals ) );
  m_pWatchUnderlying->StartWatch();

  // don't really need underlying position at this time, just a watch
  //pPosition_t pPosition = ConstructPosition( pInstrument );
}

void Server_impl::UnderlyingFundamentals( const ou::tf::Watch::Fundamentals& fundamentals ) {
  m_nPrecision = fundamentals.nPrecision;
  m_fUpdateUnderlyingInfo( m_pWatchUnderlying->GetInstrumentName(), fundamentals.nContractSize );
}

void Server_impl::UnderlyingQuote( const ou::tf::Quote& quote ) {
  m_quoteUnderlying = quote;
}

void Server_impl::UnderlyingTrade( const ou::tf::Trade& trade ) {
  //BOOST_LOG_TRIVIAL(info) << "Trade " << trade.Volume() << "@" << trade.Price();
  const double price = m_tradeUnderlying.Price();
  if ( price != trade.Price() ) {
    m_fUpdateUnderlyingPrice( price, m_nPrecision );
  }
  m_tradeUnderlying = trade;
}

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

#include <boost/log/trivial.hpp>

#include <TFTrading/DBWrapper.h>
#include <TFTrading/OrderManager.h>
#include <TFTrading/PortfolioManager.h>
#include <TFTrading/ProviderManager.h>
#include <TFTrading/InstrumentManager.h>

#include <TFIQFeed/Provider.h>
#include <TFInteractiveBrokers/IBTWS.h>

#include "Server_impl.hpp"

namespace {
  static const std::string sDataBaseName( "TableTrader.db" );
}

Server_impl::Server_impl()
: m_state( EState::quiescent )
{

  ou::tf::ProviderManager& providers( ou::tf::ProviderManager::GlobalInstance() );

  //using pProviderTWS_t = ou::tf::ib::TWS::pProvider_t;
  m_pProviderTWS = ou::tf::ib::TWS::Factory();
  m_pProviderTWS->SetName( "tws" ); // needs to match name in database
  providers.Register( m_pProviderTWS );
  m_pProviderTWS->OnConnected.Add( MakeDelegate( this, &Server_impl::Connected_TWS ) );
  m_pProviderTWS->OnDisconnected.Add( MakeDelegate( this, &Server_impl::Disconnected_TWS ) );

  //using pProviderIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;
  m_pProviderIQFeed = ou::tf::iqfeed::IQFeedProvider::Factory();
  m_pProviderIQFeed->SetName( "iqf" ); // needs to match name in database
  providers.Register( m_pProviderIQFeed );
  m_pProviderIQFeed->OnConnected.Add( MakeDelegate( this, &Server_impl::Connected_IQFeed ) );
  m_pProviderIQFeed->OnDisconnected.Add( MakeDelegate( this, &Server_impl::Disconnected_IQFeed ) );

  assert( m_pProviderTWS );
  assert( m_pProviderIQFeed );

  m_pdb = std::make_unique<ou::tf::db>( sDataBaseName );

  m_pProviderTWS->Connect();
  m_pProviderIQFeed->Connect();

  m_state = EState::connecting;

}

Server_impl::~Server_impl() {
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

void Server_impl::Start( const std::string& sUnderlyingFuture ) {

}
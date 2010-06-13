/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include <vector>
#include <string>

#include <LibTrading/Order.h>

#include "Process.h"

CProcess::CProcess(void)
: 
//  CIQFeed<CProcess>(),
  //m_tws( "U215226" ),
  m_tws( "DU15100" ),
  m_bIBConnected( false )
{
}

CProcess::~CProcess(void)
{
}

void CProcess::IBConnect( void ) {
  if ( !m_bIBConnected ) {
    m_tws.Connect();
    m_bIBConnected = true;
  }
}

void CProcess::IBDisconnect( void ) {
  if ( m_bIBConnected ) {
    m_tws.Disconnect();
    m_bIBConnected = false;
  }
}

void CProcess::PlaceBuyOrder( void ) {
  COrder::pInstrument_t instrument( new CInstrument( "ICE", "SMART", InstrumentType::Stock ) );
  m_tws.PlaceOrder( new COrder( instrument, OrderType::Market, OrderSide::Buy, 100 ) );
}

void CProcess::PlaceSellOrder( void ) {
  COrder::pInstrument_t instrument( new CInstrument( "ICE", "SMART", InstrumentType::Stock ) );
  m_tws.PlaceOrder( new COrder( instrument, OrderType::Market, OrderSide::Buy, -100 ) );
}

void CProcess::OnIQFeedConnected( void ) {
  std::vector<std::string> vs;
  vs.push_back( "@YM#" );
//  vs.push_back( "INDU" );
//  vs.push_back( "TICk" );
//  vs.push_back( "TRIN" );
}

void CProcess::OnIQFeedDisConnected( void ) {
}

//void CProcess::OnIQFeedUpdateMessage( linebuffer_t* pBuffer, CIQFUpdateMessage* msg) {
//}


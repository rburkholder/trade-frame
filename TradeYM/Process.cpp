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
#include <sstream>

#include <LibTrading/Order.h>

#include <LibHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <LibHDF5TimeSeries/HDF5DataManager.h>

#include "Process.h"

CProcess::CProcess(void)
: 
//  CIQFeed<CProcess>(),
  m_tws( "U215226" ),
  m_bIBConnected( false ),
  m_pSymbol( NULL )
{
  m_sSymbolName = "ICE";
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
  Order::pInstrument_t instrument( new Instrument( m_sSymbolName, "SMART", InstrumentType::Stock ) );
  m_tws.PlaceOrder( new Order( instrument, OrderType::Market, OrderSide::Buy, 100 ) );
}

void CProcess::PlaceSellOrder( void ) {
  Order::pInstrument_t instrument( new Instrument( m_sSymbolName, "SMART", InstrumentType::Stock ) );
  m_tws.PlaceOrder( new Order( instrument, OrderType::Market, OrderSide::Sell, 100 ) );
}

void CProcess::OnIQFeedConnected( void ) {
  std::vector<std::string> vs;
  vs.push_back( "@YM#" );
  vs.push_back( "INDU" );
  vs.push_back( "TICk" );
  vs.push_back( "TRIN" );
}

void CProcess::OnIQFeedDisConnected( void ) {
}

//void CProcess::OnIQFeedUpdateMessage( linebuffer_t* pBuffer, CIQFUpdateMessage* msg) {
//}

void CProcess::StartWatch( void ) {
  if ( ( NULL == m_pSymbol ) && ( m_bIBConnected ) ) {
    m_pSymbol = m_tws.GetSymbol( m_sSymbolName );
    m_tws.AddQuoteHandler( m_sSymbolName, MakeDelegate( this, &CProcess::HandleOnQuote ) );
    m_tws.AddTradeHandler( m_sSymbolName, MakeDelegate( this, &CProcess::HandleOnTrade ) );
  }
}

void CProcess::StopWatch( void ) {
  if ( NULL != m_pSymbol ) {
    m_tws.RemoveQuoteHandler( m_sSymbolName, MakeDelegate( this, &CProcess::HandleOnQuote ) );
    m_tws.RemoveTradeHandler( m_sSymbolName, MakeDelegate( this, &CProcess::HandleOnTrade ) );
    m_pSymbol = NULL;

    std::stringstream ss;
    ss << m_vQuotes.Size() << " Quotes, " << m_vTrades.Size() << " Trades" << std::endl;
    OutputDebugString( ss.str().c_str() );

    string sPathName;

    ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );

    if ( 0 != m_vQuotes.Size() ) {
      sPathName = "/quote/" + m_sSymbolName;
      HDF5WriteTimeSeries<Quotes, Quote> wtsq( dm );
      wtsq.Write( sPathName, &m_vQuotes );
    }

    if ( 0 != m_vTrades.Size() ) {
      sPathName = "/trade/" + m_sSymbolName;
      HDF5WriteTimeSeries<Trades, Trade> wtst( dm );
      wtst.Write( sPathName, &m_vTrades );
    }
  }
}

void CProcess::HandleOnQuote(IBSymbol::quote_t quote) {
//  std::stringstream ss;
//  ss << "Q: " << quote.DateTime() << "," << quote.Bid() << "," << quote.Ask() << std::endl;
//  OutputDebugString( ss.str().c_str() );
  m_vQuotes.Append( quote );
}

void CProcess::HandleOnTrade(IBSymbol::trade_t trade ) {
//  std::stringstream ss;
//  ss << "T: " << trade.DateTime() << "," << trade.Volume() << "@" << trade.Trade() << std::endl;
//  OutputDebugString( ss.str().c_str() );
  m_vTrades.Append( trade );
}
/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "IBSymbol.h"

//#include "boost/date_time/local_time/local_time.hpp"

CIBSymbol::CIBSymbol( TickerId id, pInstrument_t pInstrument )
: 
  CSymbol<CIBSymbol,TickerId>( id, pInstrument ), 
    m_TickerId( id ), m_conId( 0 ),
    m_bAskFound( false ), m_bAskSizeFound( false ), 
    m_bBidFound( false ), m_bBidSizeFound( false ),
    m_bLastTimeStampFound( false ), m_bLastFound( false ), m_bLastSizeFound( false ),
    m_nAskSize( 0 ), m_nBidSize( 0 ), m_nLastSize( 0 ),
    m_dblAsk( 0 ), m_dblBid( 0 ), m_dblLast( 0 ),
    m_nVolume( 0 ),
    m_dblHigh( 0 ), m_dblLow( 0 ), m_dblClose( 0 ),
    m_bQuoteTradeWatchInProgress( false ), m_bDepthWatchInProgress( false ) {
}

CIBSymbol::~CIBSymbol(void) {
}

void CIBSymbol::AcceptTickPrice(TickType tickType, double price) {
  switch ( tickType ) {
    case TickType::BID:
      if ( price != m_dblBid ) {
        m_dblBid = price;
        m_bBidFound = true;
        BuildQuote();
      }
      break;
    case TickType::ASK:
      if ( price != m_dblAsk ) {
        m_dblAsk = price;
        m_bAskFound = true;
        BuildQuote();
      }
      break;
    case TickType::LAST:
      m_dblLast = price;
      m_bLastFound = true;
      BuildTrade();
      break;
    case TickType::HIGH:
      m_dblHigh = price;
//      std::cout << m_sSymbolName << " High " << price << std::endl;
      break;
    case TickType::LOW:
      m_dblLow = price;
//      std::cout << m_sSymbolName << " Low " << price << std::endl;
      break;
    case TickType::CLOSE:
      m_dblClose = price;
//      std::cout << m_sSymbolName << " Close " << price << std::endl;
      break;
  }
}

void CIBSymbol::AcceptTickSize(TickType tickType, int size) {
  switch ( tickType ) {
    case TickType::BID_SIZE:
      if ( size != m_nBidSize ) {
        m_nBidSize = size;
        m_bBidSizeFound = true;
        BuildQuote();
      }
      break;
    case TickType::ASK_SIZE:
      if ( size != m_nAskSize ) {
        m_nAskSize = size;
        m_bAskSizeFound = true;
        BuildQuote();
      }
      break;
    case TickType::LAST_SIZE:
      m_nLastSize = size;
      m_bLastSizeFound = true;
      BuildTrade();
      break;
    case TickType::VOLUME:
      m_nVolume = size;
      m_bLastFound = m_bLastSizeFound = false;  // reset flags to get in sync
      //BuildTrade();
      break;
  }
}

void CIBSymbol::AcceptTickString(TickType tickType, const IBString &value) {
  switch ( tickType ) {
    case TickType::LAST_TIMESTAMP:
      m_bLastTimeStampFound = true;
      m_bLastFound = m_bLastSizeFound = false; // timestamp seems to lead the trade and size
      BuildTrade();
      break;
  }
}

void CIBSymbol::BuildQuote() {
  if ( m_bAskFound && m_bBidFound && m_bAskSizeFound && m_bBidSizeFound ) {
    //boost::local_time::local_date_time ldt = 
    //  boost::local_time::local_microsec_clock::local_time();
    CQuote quote( m_TimeSource.External(), m_dblBid, m_nBidSize, m_dblAsk, m_nAskSize );
    //std::cout << "Q:" << quote.m_dt << " " 
    //  << quote.m_nBidSize << "@" << quote.m_dblBid << " "
    //  << quote.m_nAskSize << "@" << quote.m_dblAsk 
    //  << std::endl;
    m_OnQuote( quote );  
  }
}

void CIBSymbol::BuildTrade() {
  //if ( !m_bLastTimeStampFound && m_bLastFound && m_bLastSizeFound ) {
  //  std::cout << m_sSymbolName << " Trade is weird" << std::endl;
  //}
  //if ( m_bLastTimeStampFound && m_bLastFound && m_bLastSizeFound ) {
  if ( m_bLastFound && m_bLastSizeFound ) {
    CTrade trade( m_TimeSource.External(), m_dblLast, m_nLastSize );
    //std::cout << "T:" << trade.m_dt << " " << trade.m_nTradeSize << "@" << trade.m_dblTrade << std::endl;
    m_OnTrade( trade );
    //m_bLastTimeStampFound = m_bLastFound = m_bLastSizeFound = false;
    m_bLastFound = m_bLastSizeFound = false;
  }
}
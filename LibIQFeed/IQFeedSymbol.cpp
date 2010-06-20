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

#include <iostream>

#include "IQFeedSymbol.h"

CIQFeedSymbol::CIQFeedSymbol(const symbol_id_t& sSymbol, pInstrument_t pInstrument) 
: 
  CSymbol<CIQFeedSymbol,symbol_id_t>( sSymbol, pInstrument ),
  m_cnt( 0 ), m_dblTrade( 0 ), m_dblChange( 0 ), m_nTradeSize( 0 ), m_nTotalVolume( ),
  m_dblBid( 0 ), m_dblAsk( 0 ), m_nBidSize( 0 ), m_nAskSize( 0 ), 
  m_dblOpen( 0 ), m_dblClose( 0 ), m_cntTrades( 0 ), m_dblHigh( 0 ), m_dblLow( 0 ), 
  m_nOpenInterest( 0 ), m_QStatus( qUnknown ),
  m_bQuoteTradeWatchInProgress( false ), m_bDepthWatchInProgress( false )
{
}

CIQFeedSymbol::~CIQFeedSymbol(void) {
}

void CIQFeedSymbol::HandleFundamentalMessage( CIQFFundamentalMessage *pMsg ) {
  m_sOptionRoots = pMsg->Field( CIQFFundamentalMessage::FRootOptionSymbols );
  m_AverageVolume = pMsg->Integer( CIQFFundamentalMessage::FAveVolume );
  m_sCompanyName = pMsg->Field( CIQFFundamentalMessage::FName );
  m_Precision = pMsg->Integer( CIQFFundamentalMessage::FPrecision );
  m_dblHistoricalVolatility = pMsg->Double( CIQFFundamentalMessage::FVolatility );
  m_dblStrikePrice = pMsg->Double( CIQFFundamentalMessage::FStrikePrice );

  OnFundamentalMessage( this );
}

template <typename T>
void CIQFeedSymbol::DecodePricingMessage( CIQFPricingMessage<T> *pMsg ) {
  m_bNewTrade = m_bNewQuote = m_bNewOpen = false;
  std::string sLastTradeTime = pMsg->Field( CIQFPricingMessage<T>::QPLastTradeTime );
  if ( sLastTradeTime.length() > 0 ) {  // can we do 'assume' anything if it is 0?
    ptime dtLastTrade;
    double dblOpen, dblBid, dblAsk;
    int nBidSize, nAskSize;
    char chType = sLastTradeTime[ sLastTradeTime.length() - 1 ];
    m_dtLastTrade = pMsg->LastTradeTime();
    switch ( chType ) {
    case 't':
    case 'T':
      m_dblTrade = pMsg->Double( CIQFPricingMessage<T>::QPLast );
      m_dblChange = pMsg->Double( CIQFPricingMessage<T>::QPChange );
      m_nTotalVolume = pMsg->Integer( CIQFPricingMessage<T>::QPTtlVol );
      m_nTradeSize = pMsg->Integer( CIQFPricingMessage<T>::QPLastVol );
      m_dblHigh = pMsg->Double( CIQFPricingMessage<T>::QPHigh );
      m_dblLow = pMsg->Double( CIQFPricingMessage<T>::QPLow );
      m_dblClose = pMsg->Double( CIQFPricingMessage<T>::QPClose );
      m_cntTrades = pMsg->Integer( CIQFPricingMessage<T>::QPNumTrades );
      m_bNewTrade = true;

      dblOpen = pMsg->Double( CIQFPricingMessage<T>::QPOpen );
      if ( ( m_dblOpen != dblOpen ) && ( 0 != dblOpen ) ) { 
        m_dblOpen = dblOpen; 
        m_bNewOpen = true; 
        std::cout << "IQF new open: " << GetId() << "=" << m_dblOpen << std::endl;
      };

      // fall through to processing bid / ask
    case 'b':
    case 'a':
      dblBid = pMsg->Double( CIQFPricingMessage<T>::QPBid );
      if ( m_dblBid != dblBid ) { m_dblBid = dblBid; m_bNewQuote = true; }
      nBidSize = pMsg->Integer( CIQFPricingMessage<T>::QPBidSize );
      if ( m_nBidSize != nBidSize ) { m_nBidSize = nBidSize; m_bNewQuote = true; }
      dblAsk = pMsg->Double( CIQFPricingMessage<T>::QPAsk );
      if ( m_dblAsk != dblAsk ) { m_dblAsk = dblAsk; m_bNewQuote = true; }
      nAskSize = pMsg->Integer( CIQFPricingMessage<T>::QPAskSize );
      if ( m_nAskSize != nAskSize ) { m_nAskSize = nAskSize; m_bNewQuote = true; }
      break;
    case 'o':
      break;
    }
  }
  //OpenInterest = pMsg->Integer( CIQFPricingMessage::QPOpenInterest );

  //CString s;
  //s.Format( "%s: %c %0.2f@%d b=%0.2f@%d a=%0.2f@%d #=%d", 
  //  m_sSymbol, chType, dblTrade, TradeSize, dblBid, BidSize, dblAsk, AskSize, cntTrades );
  //theApp.pConsoleMessages->WriteLine( s ); 
}

void CIQFeedSymbol::HandleSummaryMessage( CIQFSummaryMessage *pMsg ) {
  DecodePricingMessage<CIQFSummaryMessage>( pMsg );
  OnSummaryMessage( this );
}

void CIQFeedSymbol::HandleUpdateMessage( CIQFUpdateMessage *pMsg ) {

  if ( qUnknown == m_QStatus ) {
    m_QStatus = ( _T( "Not Found" ) == pMsg->Field( CIQFPricingMessage<CIQFUpdateMessage>::QPLast ) ) ? qNotFound : qFound;
    if ( qNotFound == m_QStatus ) {
      std::cout << GetId() << " not found" << endl;
    }
  }
  if ( qFound == m_QStatus ) {
    DecodePricingMessage<CIQFUpdateMessage>( pMsg );
    OnUpdateMessage( this );
    ptime dt( microsec_clock::local_time() );
    // quote needs to be sent before the trade
    if ( m_bNewQuote ) {
      CQuote quote( dt, m_dblBid, m_nBidSize, m_dblAsk, m_nAskSize );
      CSymbol::m_OnQuote( quote );
    }
    if ( m_bNewTrade ) {
      CTrade trade( dt, m_dblTrade, m_nTradeSize );
      CSymbol::m_OnTrade( trade );
      if ( m_bNewOpen ) {
        CSymbol::m_OnOpen( trade );
      }
    }
  }
}

void CIQFeedSymbol::HandleNewsMessage( CIQFNewsMessage *pMsg ) {
}

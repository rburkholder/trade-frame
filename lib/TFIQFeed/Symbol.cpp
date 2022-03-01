/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

//#include "StdAfx.h"

#include <iostream>

#include <OUCommon/TimeSource.h>

#include "Symbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

IQFeedSymbol::IQFeedSymbol(const symbol_id_t& sSymbol, pInstrument_t pInstrument)
: Symbol<IQFeedSymbol>( pInstrument, sSymbol ),
  m_cnt( 0 ),
  m_QStatus( qUnknown ),
  m_stateWatch( WatchState::None ),
  m_bDepthWatchInProgress( false ),
  m_bWaitForFirstQuote( true )
{
  m_pFundamentals = std::make_shared<Fundamentals>();
  m_pSummary = std::make_shared<Summary>();
}

IQFeedSymbol::~IQFeedSymbol(void) {
}

void IQFeedSymbol::HandleFundamentalMessage(
  IQFFundamentalMessage *pMsg,
  fLookupSecurityType_t&& fLookupSecurityType,
  fLookupListedMarket_t&& fLookupListedMarket
) {

  Fundamentals& fundamentals( *m_pFundamentals );

  fundamentals.sSymbolName = pMsg->Field( IQFFundamentalMessage::FSymbol );
  fundamentals.sCompanyName = pMsg->Field( IQFFundamentalMessage::FCompanyName );
  fundamentals.sExchangeRoot = pMsg->Field( IQFFundamentalMessage::FExchangeRoot );
  fundamentals.sOptionRoots = pMsg->Field( IQFFundamentalMessage::FRootOptionSymbols );
  fundamentals.sExchange = fLookupListedMarket( pMsg->Field( IQFFundamentalMessage::fExchangeID ) );
  fundamentals.nPrecision = pMsg->Integer( IQFFundamentalMessage::FPrecision );
  fundamentals.nContractSize = pMsg->Integer( IQFFundamentalMessage::FContractSize );
  fundamentals.nAverageVolume = pMsg->Integer( IQFFundamentalMessage::FAveVolume );
  fundamentals.eSecurityType = fLookupSecurityType( pMsg->Integer( IQFFundamentalMessage::FSecurityType ) );
  //fundamentals.nShortInterest = pMsg->Integer( IQFFundamentalMessage::FShortInterest ); // TODO: not in fundamental
  fundamentals.dblHistoricalVolatility = pMsg->Double( IQFFundamentalMessage::FVolatility );
  fundamentals.dblStrikePrice = pMsg->Double( IQFFundamentalMessage::FStrikePrice );
  fundamentals.dblPriceEarnings = pMsg->Double( IQFFundamentalMessage::FPriceEarnings );
  fundamentals.dbl52WkHi = pMsg->Double( IQFFundamentalMessage::F52WkHi );
  fundamentals.dbl52WkLo = pMsg->Double( IQFFundamentalMessage::F52WkLo );
  fundamentals.dblDividendAmount = pMsg->Double( IQFFundamentalMessage::FDivAmt );
  fundamentals.dblDividendRate = pMsg->Double( IQFFundamentalMessage::FDivRate );
  fundamentals.dblDividendYield = pMsg->Double( IQFFundamentalMessage::FDivYld );
  fundamentals.dblTickSize = pMsg->Double( IQFFundamentalMessage::FMinimumTickSize );
  fundamentals.dateExDividend = pMsg->Date( IQFFundamentalMessage::FDivExDate );
  fundamentals.dateExpiration = pMsg->Date( IQFFundamentalMessage::FExpirationDate );
  fundamentals.timeSessionOpen = pMsg->Time( IQFFundamentalMessage::FSessionOpenTime );
  fundamentals.timeSessionClose = pMsg->Time( IQFFundamentalMessage::FSessionCloseTime );

  switch ( fundamentals.eSecurityType ) {
    case ESecurityType::IEOption:
    case ESecurityType::FOption:
      {
        const std::string& symbol( fundamentals.sSymbolName );
        // assumes strike is last part of name
        for ( std::string::const_reverse_iterator iter = symbol.rbegin(); iter != symbol.rend(); iter++ ) {
          if ( ( '.' == *iter ) || ( ('0' <= *iter ) && ( '9' >= *iter ) ) ) {}
          else {
            if ( 'L' >= *iter ) fundamentals.eOptionSide = ou::tf::OptionSide::enumOptionSide::Call;
            if ( 'M' <= *iter ) fundamentals.eOptionSide = ou::tf::OptionSide::enumOptionSide::Put;
            break;
          }
        }
      }
      break;
    default: {}
  }

  OnFundamentalMessage( m_pFundamentals );
}

template <typename T>
void IQFeedSymbol::DecodeDynamicFeedMessage( IQFDynamicFeedMessage<T>* pMsg )  {

  // http://www.iqfeed.net/dev/api/docs/Level1UpdateSummaryMessage.cfm
  double dblOpen, dblBid, dblAsk;
  int nBidSize, nAskSize;

  Summary& summary( *m_pSummary );

  summary.bNewTrade = summary.bNewQuote = summary.bNewOpen = false;

  std::string content = pMsg->Field( IQFDynamicFeedMessage<T>::DFMessageContents );
  for ( const char id: content ) {
    switch ( id ) {
      case 'C':
        summary.dblTrade = pMsg->Double( IQFDynamicFeedMessage<T>::DFMostRecentTrade );
        summary.nTradeSize = pMsg->Integer( IQFDynamicFeedMessage<T>::DFMostRecentTradeSize );
        summary.cntTrades = pMsg->Integer( IQFDynamicFeedMessage<T>::DFNumTrades );
        summary.nTotalVolume = pMsg->Integer( IQFDynamicFeedMessage<T>::DFTtlVol );
        summary.bNewTrade = true;
        break;
      case 'a':
        dblAsk = pMsg->Double( IQFDynamicFeedMessage<T>::DFAsk );
        if ( summary.dblAsk != dblAsk ) { summary.dblAsk = dblAsk; summary.bNewQuote = true; }
        nAskSize = pMsg->Integer( IQFDynamicFeedMessage<T>::DFAskSize );
        if ( summary.nAskSize != nAskSize ) { summary.nAskSize = nAskSize; summary.bNewQuote = true; }
        break;
      case 'b':
        dblBid = pMsg->Double( IQFDynamicFeedMessage<T>::DFBid );
        if ( summary.dblBid != dblBid ) { summary.dblBid = dblBid; summary.bNewQuote = true; }
        nBidSize = pMsg->Integer( IQFDynamicFeedMessage<T>::DFBidSize );
        if ( summary.nBidSize != nBidSize ) { summary.nBidSize = nBidSize; summary.bNewQuote = true; }
        break;
      case 'o':
        // TODO: may not be using the correct field here.
        dblOpen = pMsg->Double( IQFDynamicFeedMessage<T>::DFMostRecentTrade );
        if ( ( summary.dblOpen != dblOpen ) && ( 0 != dblOpen ) ) {
          summary.dblOpen = dblOpen;
          summary.bNewOpen = true;
          std::cout << "IQF new open 1: " << GetId() << "=" << summary.dblOpen << std::endl;
        };
        break;
      case 'E':
        // will need to supply fields
        break;
      case 'O': // any non C,E trade
        break;
    }
  }
  if ( m_bWaitForFirstQuote ) {
    if ( summary.bNewQuote ) {
      if ( ( -1 == summary.nBidSize ) || ( -1 == summary.nAskSize ) ) {
        summary.bNewQuote = false;
      }
      else {
        m_bWaitForFirstQuote = false;
      }
    }
  }

}

template <typename T>
void IQFeedSymbol::DecodePricingMessage( IQFPricingMessage<T>* pMsg ) {

  Summary& summary( *m_pSummary );

  summary.bNewTrade = summary.bNewQuote = summary.bNewOpen = false;

  char chType;
  ptime dtLastTrade;
  double dblOpen, dblBid, dblAsk;
  int nBidSize, nAskSize;

  std::string sLastTradeTime = pMsg->Field( IQFPricingMessage<T>::QPLastTradeTime );
  if ( sLastTradeTime.length() > 0 ) {
    chType = sLastTradeTime[ sLastTradeTime.length() - 1 ];
  }
  else {
    chType = 'q';
  }
// TODO: test that data file is available
  summary.dtLastTrade = pMsg->LastTradeTime();
  switch ( chType ) {
    case 't':
    case 'T':
      summary.dblTrade = pMsg->Double( IQFPricingMessage<T>::QPLast );
      summary.dblChange = pMsg->Double( IQFPricingMessage<T>::QPChange );
      summary.nTotalVolume = pMsg->Integer( IQFPricingMessage<T>::QPTtlVol );
      summary.nTradeSize = pMsg->Integer( IQFPricingMessage<T>::QPLastVol );
      summary.dblHigh = pMsg->Double( IQFPricingMessage<T>::QPHigh );
      summary.dblLow = pMsg->Double( IQFPricingMessage<T>::QPLow );
      summary.dblClose = pMsg->Double( IQFPricingMessage<T>::QPClose );
      summary.cntTrades = pMsg->Integer( IQFPricingMessage<T>::QPNumTrades );
      summary.bNewTrade = true;

      dblOpen = pMsg->Double( IQFPricingMessage<T>::QPOpen );
      if ( ( summary.dblOpen != dblOpen ) && ( 0 != dblOpen ) ) {
        summary.dblOpen = dblOpen;
        summary.bNewOpen = true;
        std::cout << "IQF new open 2: " << GetId() << "=" << summary.dblOpen << std::endl;
      };
      summary.nOpenInterest = pMsg->Integer( IQFPricingMessage<T>::QPOpenInterest );

      // fall through to processing bid / ask
    case 'q':
    case 'b':
    case 'a':
      dblBid = pMsg->Double( IQFPricingMessage<T>::QPBid );
      if ( summary.dblBid != dblBid ) { summary.dblBid = dblBid; summary.bNewQuote = true; }
      nBidSize = pMsg->Integer( IQFPricingMessage<T>::QPBidSize );
      if ( summary.nBidSize != nBidSize ) { summary.nBidSize = nBidSize; summary.bNewQuote = true; }
      dblAsk = pMsg->Double( IQFPricingMessage<T>::QPAsk );
      if ( summary.dblAsk != dblAsk ) { summary.dblAsk = dblAsk; summary.bNewQuote = true; }
      nAskSize = pMsg->Integer( IQFPricingMessage<T>::QPAskSize );
      if ( summary.nAskSize != nAskSize ) { summary.nAskSize = nAskSize; summary.bNewQuote = true; }
      break;
    case 'o':
      break;
    default:
      std::cout << "IQFeedSymbol::DecodePricingMessage: " << this->m_pInstrument->GetInstrumentName() << " Unknown price type: " << chType << std::endl;
  }
//  }

  if ( false ) {
    std::cout
      << m_pInstrument->GetInstrumentName()
      << ","    << chType
      << ",t="  << summary.dblTrade
      << ",oi=" << summary.nOpenInterest
      << ",b="  << summary.dblBid
      << ",a="  << summary.dblAsk
      << ",#="  << summary.cntTrades
      << std::endl;
  }

}

void IQFeedSymbol::HandleSummaryMessage( IQFSummaryMessage* pMsg ) {

  DecodePricingMessage<IQFSummaryMessage>( pMsg );
  OnSummaryMessage( m_pSummary );

  Summary& summary( *m_pSummary );

  if ( summary.bNewQuote ) { // before or after OnSummaryMessage? UpdateMessage has it after
    ptime dt( ou::TimeSource::Instance().External() );
    Quote quote( dt, summary.dblBid, summary.nBidSize, summary.dblAsk, summary.nAskSize );
    Symbol::m_OnQuote( quote );
  }

}

void IQFeedSymbol::HandleUpdateMessage( IQFUpdateMessage* pMsg ) {

  if ( qUnknown == m_QStatus ) {
    m_QStatus = ( "Not Found" == pMsg->Field( IQFPricingMessage<IQFUpdateMessage>::QPLast ) ) ? qNotFound : qFound;
    if ( qNotFound == m_QStatus ) {
      std::cout << GetId() << " not found" << std::endl;
    }
  }
  if ( qFound == m_QStatus ) {
    DecodePricingMessage<IQFUpdateMessage>( pMsg );
    OnUpdateMessage( m_pSummary );

    Summary& summary( *m_pSummary );

    //ptime dt( microsec_clock::local_time() );
    ptime dt( ou::TimeSource::Instance().External() );
    // quote needs to be sent before the trade
    if ( summary.bNewQuote ) {
      const Quote quote( dt, summary.dblBid, summary.nBidSize, summary.dblAsk, summary.nAskSize );
      Symbol::m_OnQuote( quote );
    }
    if ( summary.bNewTrade ) {
      Trade trade( dt, summary.dblTrade, summary.nTradeSize );
      Symbol::m_OnTrade( trade );
      if ( summary.bNewOpen ) {
        Symbol::m_OnOpen( trade );
      }
    }
  }
}

void IQFeedSymbol::HandleDynamicFeedSummaryMessage( IQFDynamicFeedSummaryMessage* pMsg ) {

  DecodeDynamicFeedMessage<IQFDynamicFeedSummaryMessage>( pMsg );
  OnSummaryMessage( m_pSummary );

  Summary& summary( *m_pSummary );

  if ( summary.bNewQuote ) { // before or after OnSummaryMessage? UpdateMessage has it after
    ptime dt( ou::TimeSource::Instance().External() );
    Quote quote( dt, summary.dblBid, summary.nBidSize, summary.dblAsk, summary.nAskSize );
    Symbol::m_OnQuote( quote );
  }

}

void IQFeedSymbol::HandleDynamicFeedUpdateMessage( IQFDynamicFeedUpdateMessage* pMsg ) {

//  if ( qUnknown == m_QStatus ) {
//    m_QStatus = ( "Not Found" == pMsg->Field( IQFPricingMessage<IQFUpdateMessage>::QPLast ) ) ? qNotFound : qFound;
//    if ( qNotFound == m_QStatus ) {
//      std::cout << GetId() << " not found" << std::endl;
//    }
//  }
//  if ( qFound == m_QStatus ) {
    DecodeDynamicFeedMessage<IQFDynamicFeedUpdateMessage>( pMsg );
    OnUpdateMessage( m_pSummary );

    Summary& summary( *m_pSummary );

    //ptime dt( microsec_clock::local_time() );
    ptime dt( ou::TimeSource::Instance().External() );
    // quote needs to be sent before the trade
    if ( summary.bNewQuote ) {
      const Quote quote( dt, summary.dblBid, summary.nBidSize, summary.dblAsk, summary.nAskSize );
      Symbol::m_OnQuote( quote );
    }
    if ( summary.bNewTrade ) {
      Trade trade( dt, summary.dblTrade, summary.nTradeSize );
      Symbol::m_OnTrade( trade );
      if ( summary.bNewOpen ) {
        Symbol::m_OnOpen( trade );
      }
    }
//  }
}

void IQFeedSymbol::HandleNewsMessage( IQFNewsMessage* pMsg ) {
}

} // namespace iqfeed
} // namespace tf
} // namespace ou

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

#include <boost/log/trivial.hpp>

#include <boost/asio/post.hpp>

#include <OUCommon/TimeSource.h>

#include <TFTrading/MacroStrand.h>

#include "Symbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

IQFeedSymbol::IQFeedSymbol( const idSymbol_t& sSymbol, pInstrument_t pInstrument )
: Symbol<IQFeedSymbol>( pInstrument, sSymbol )
, m_cnt( 0 )
, m_QStatus( qUnknown )
, m_stateWatch( WatchState::None )
, m_bWaitForFirstQuote( true )
{
  m_pFundamentals = std::make_shared<Fundamentals>();
  m_pSummary = std::make_shared<Summary>();
}

IQFeedSymbol::~IQFeedSymbol() {
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
  fundamentals.nFormatCode = pMsg->Integer( IQFFundamentalMessage::FFormatCode );
  fundamentals.nContractSize = pMsg->Integer( IQFFundamentalMessage::FContractSize );
  fundamentals.nAverageVolume = pMsg->Integer( IQFFundamentalMessage::FAveVolume );
  fundamentals.nSIC = pMsg->Integer( IQFFundamentalMessage::FSIC );
  fundamentals.nNAICS = pMsg->Integer( IQFFundamentalMessage::FNAICS );
  fundamentals.eSecurityType = fLookupSecurityType( pMsg->Integer( IQFFundamentalMessage::FSecurityType ) );
  fundamentals.dblHistoricalVolatility = pMsg->Double( IQFFundamentalMessage::FVolatility );
  fundamentals.dblStrikePrice = pMsg->Double( IQFFundamentalMessage::FStrikePrice );
  fundamentals.dblPriceEarnings = pMsg->Double( IQFFundamentalMessage::FPriceEarnings );
  fundamentals.dblAssets = pMsg->Double( IQFFundamentalMessage::FCurAssets );
  fundamentals.dblLiabilities = pMsg->Double( IQFFundamentalMessage::FCurLiab );
  fundamentals.dblCommonSharesOutstanding = pMsg->Double( IQFFundamentalMessage::FCommonShares );
  fundamentals.dbl52WkHi = pMsg->Double( IQFFundamentalMessage::F52WkHi );
  fundamentals.dbl52WkLo = pMsg->Double( IQFFundamentalMessage::F52WkLo );
  fundamentals.dblDividendAmount = pMsg->Double( IQFFundamentalMessage::FDivAmt );
  fundamentals.dblDividendRate = pMsg->Double( IQFFundamentalMessage::FDivRate );
  fundamentals.dblDividendYield = pMsg->Double( IQFFundamentalMessage::FDivYld );
  fundamentals.dblTickSize = pMsg->Double( IQFFundamentalMessage::FMinimumTickSize );
  fundamentals.datePayed = pMsg->Date( IQFFundamentalMessage::FDivPayDate );
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
            if ( 'L' >= *iter ) fundamentals.eOptionSide = ou::tf::OptionSide::EOptionSide::Call;
            if ( 'M' <= *iter ) fundamentals.eOptionSide = ou::tf::OptionSide::EOptionSide::Put;
            break;
          }
        }
      }
      break;
    default: {}
  }
  STRAND( OnFundamentalMessage( m_pFundamentals ) )
}

template <typename T>
void IQFeedSymbol::DecodeDynamicFeedMessage( IQFDynamicFeedMessage<T>* pMsg )  {

  // http://www.iqfeed.net/dev/api/docs/Level1UpdateSummaryMessage.cfm

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
        {
          double dblAsk = pMsg->Double( IQFDynamicFeedMessage<T>::DFAsk );
          if ( summary.dblAsk != dblAsk ) { summary.dblAsk = dblAsk; summary.bNewQuote = true; }
          int nAskSize = pMsg->Integer( IQFDynamicFeedMessage<T>::DFAskSize );
          if ( summary.nAskSize != nAskSize ) { summary.nAskSize = nAskSize; summary.bNewQuote = true; }
        }
        break;
      case 'b':
        {
          double dblBid = pMsg->Double( IQFDynamicFeedMessage<T>::DFBid );
          if ( summary.dblBid != dblBid ) { summary.dblBid = dblBid; summary.bNewQuote = true; }
          int nBidSize = pMsg->Integer( IQFDynamicFeedMessage<T>::DFBidSize );
          if ( summary.nBidSize != nBidSize ) { summary.nBidSize = nBidSize; summary.bNewQuote = true; }
        }
        break;
      case 'v': // volume update
        summary.nOpenInterest = pMsg->Integer( IQFDynamicFeedMessage<T>::DFOpenInterest );
        break;
      case 'o': // open
        {
          // TODO: may not be using the correct field here.
          double dblOpen = pMsg->Double( IQFDynamicFeedMessage<T>::DFMostRecentTrade );
          if ( ( summary.dblOpen != dblOpen ) && ( 0 != dblOpen ) ) {
            summary.dblOpen = dblOpen;
            summary.bNewOpen = true;
              //BOOST_LOG_TRIVIAL(info)
              //  << "IQF new open 1: " << GetId() << "=" << summary.dblOpen;
          };
        }
        break;
      case 'h':  // high
      case 'l':  // low
      case 'c':  // close
        break;
      case 's':  // settlement
        break;
      case 'E':
        // will need to supply fields
        break;
      case 'O': // any non C,E trade
        break;
      default: {
        BOOST_LOG_TRIVIAL(warning) << "iqfeed msg has unknown field type: '" << content << "'";
        }
        break;
    }
  }

  if ( 0 == content.size() ) {
    summary.dblOpen = pMsg->Double( IQFDynamicFeedMessage<T>::DFMostRecentTrade );
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
        BOOST_LOG_TRIVIAL(info)
          << "IQF new open 2: " << GetId() << "=" << summary.dblOpen;
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
      BOOST_LOG_TRIVIAL(error)
        << "IQFeedSymbol::DecodePricingMessage: " << this->m_pInstrument->GetInstrumentName() << " Unknown price type: " << chType;
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

  STRAND( OnSummaryMessage( m_pSummary ) )

  Summary& summary( *m_pSummary );

  if ( summary.bNewQuote ) { // before or after OnSummaryMessage? UpdateMessage has it after
    ptime dt( ou::TimeSource::GlobalInstance().External() );
    Quote quote( dt, summary.dblBid, summary.nBidSize, summary.dblAsk, summary.nAskSize );
    STRAND_CAPTURE( (Symbol::m_OnQuote( quote )), quote )
  }

}

void IQFeedSymbol::HandleUpdateMessage( IQFUpdateMessage* pMsg ) {

  if ( qUnknown == m_QStatus ) {
    m_QStatus = ( "Not Found" == pMsg->Field( IQFPricingMessage<IQFUpdateMessage>::QPLast ) ) ? qNotFound : qFound;
    if ( qNotFound == m_QStatus ) {
      BOOST_LOG_TRIVIAL(error)
        << "IQFeedSymbol::HandleUpdateMessage: " << GetId() << " not found";
    }
  }
  if ( qFound == m_QStatus ) {
    DecodePricingMessage<IQFUpdateMessage>( pMsg );

    STRAND( OnUpdateMessage( m_pSummary ) )

    Summary& summary( *m_pSummary );

    //ptime dt( microsec_clock::local_time() );
    ptime dt( ou::TimeSource::GlobalInstance().External() );
    // quote needs to be sent before the trade
    if ( summary.bNewQuote ) {
      const Quote quote( dt, summary.dblBid, summary.nBidSize, summary.dblAsk, summary.nAskSize );
      STRAND_CAPTURE( (Symbol::m_OnQuote( quote )), quote )
    }

    if ( summary.bNewTrade ) {
      Trade trade( dt, summary.dblTrade, summary.nTradeSize );
      STRAND_CAPTURE( (Symbol::m_OnTrade( trade )), trade)
      if ( summary.bNewOpen ) {
        STRAND_CAPTURE( (Symbol::m_OnOpen( trade )), trade)
      }
    }
  }
}

void IQFeedSymbol::HandleDynamicFeedSummaryMessage( IQFDynamicFeedSummaryMessage* pMsg ) {

  DecodeDynamicFeedMessage<IQFDynamicFeedSummaryMessage>( pMsg );

  STRAND( OnSummaryMessage( m_pSummary ) )

  Summary& summary( *m_pSummary );

  if ( summary.bNewQuote ) { // before or after OnSummaryMessage? UpdateMessage has it after
    ptime dt( ou::TimeSource::GlobalInstance().External() );
    Quote quote( dt, summary.dblBid, summary.nBidSize, summary.dblAsk, summary.nAskSize );
    STRAND_CAPTURE( (Symbol::m_OnQuote( quote )), quote )
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

    STRAND( OnUpdateMessage( m_pSummary ) )

    Summary& summary( *m_pSummary );

    //ptime dt( microsec_clock::local_time() );
    ptime dt( ou::TimeSource::GlobalInstance().External() );
    // quote needs to be sent before the trade
    if ( summary.bNewQuote ) {
      const Quote quote( dt, summary.dblBid, summary.nBidSize, summary.dblAsk, summary.nAskSize );
      STRAND_CAPTURE( (Symbol::m_OnQuote( quote )), quote )

    }
    if ( summary.bNewTrade ) {
      Trade trade( dt, summary.dblTrade, summary.nTradeSize );
      STRAND_CAPTURE( (Symbol::m_OnTrade( trade )), trade )

      if ( summary.bNewOpen ) {
        STRAND_CAPTURE( (Symbol::m_OnOpen( trade )), trade )
      }
    }
//  }
}

void IQFeedSymbol::HandleNewsMessage( IQFNewsMessage* pMsg ) {
}

void IQFeedSymbol::SubmitMarketDepthByMM( const ou::tf::DepthByMM& md ) {
  STRAND_CAPTURE( (Symbol::m_OnDepthByMM( md )), md )
}

void IQFeedSymbol::SubmitMarketDepthByOrder( const ou::tf::DepthByOrder& md ) {
  STRAND_CAPTURE( (Symbol::m_OnDepthByOrder( md )), md )
}

} // namespace iqfeed
} // namespace tf
} // namespace ou

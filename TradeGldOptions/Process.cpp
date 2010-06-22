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

#include <algorithm>

#include "Process.h"

#include <LibIndicators/Pivots.h>

//
// ==================
//

COptionInfo::COptionInfo( double dblStrike ) 
: m_dblStrike( dblStrike ),
  m_dblCallDelta( 0 ),
  m_dblPutDelta( 0 ),
  m_dblGamma( 0 ),
  m_bWatching( false ),
  m_pCallSymbol( NULL ),
  m_pPutSymbol( NULL )
{
}

COptionInfo::COptionInfo( const COptionInfo& rhs ) 
: m_dblStrike( rhs.m_dblStrike ),
  m_dblCallDelta( rhs.m_dblCallDelta ),
  m_dblPutDelta( rhs.m_dblPutDelta ),
  m_dblGamma( rhs.m_dblGamma ),
  m_pCallSymbol( rhs.m_pCallSymbol ), m_pPutSymbol( rhs.m_pPutSymbol ),
  m_bWatching( false ), 
  m_CallBid( 0 ), m_CallAsk( 0 ), m_PutBid( 0 ), m_PutAsk( 0 )
{ 
  assert( !rhs.m_bWatching );
}

COptionInfo::~COptionInfo( void ) {
}

COptionInfo& COptionInfo::operator=( const COptionInfo& rhs ) {
  assert( !rhs.m_bWatching );
  assert( !m_bWatching );
  m_dblStrike = rhs.m_dblStrike;
  m_dblCallDelta = rhs.m_dblCallDelta;
  m_dblPutDelta = rhs.m_dblPutDelta;
  m_dblGamma = rhs.m_dblGamma;
  m_pCallSymbol = rhs.m_pCallSymbol;
  m_pPutSymbol = rhs.m_pPutSymbol;
  return *this;
}

void COptionInfo::HandleCallQuote( const CQuote& quote ) {
  m_CallBid = quote.Bid();
  m_CallAsk = quote.Ask();
}

void COptionInfo::HandlePutQuote( const CQuote& quote ) {
  m_PutBid = quote.Bid();
  m_PutAsk = quote.Ask();
}

void COptionInfo::HandleCallTrade( const CTrade& trade ) {
  m_ss.str( "" );
  m_ss << "Trade(C" << m_dblStrike << "): " << trade.Volume() << "@" << trade.Trade() << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void COptionInfo::HandlePutTrade( const CTrade& trade ) {
  m_ss.str( "" );
  m_ss << "Trade(P" << m_dblStrike << "): " << trade.Volume() << "@" << trade.Trade() << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}


//
// ==================
//

CProcess::CProcess(void)
:
  m_tws( "U215226" ),
  m_bIBConnected( false ), m_bIQFeedConnected( false ),
  m_sSymbolName( "GLD" ), m_contractidUnderlying( 0 ),
  m_nCalls( 0 ), m_nPuts( 0 ),
  m_bWatchingOptions( false )
{

  m_contract.currency = "USD";
  m_contract.exchange = "SMART";
  m_contract.secType = "OPT";
  m_contract.symbol = m_sSymbolName;
  m_contract.expiry = "20100630";

  m_tws.OnConnected.Add( MakeDelegate( this, &CProcess::HandleOnIBConnected ) );
  m_tws.OnDisconnected.Add( MakeDelegate( this, &CProcess::HandleOnIBDisconnected ) );

  m_iqfeed.OnConnected.Add( MakeDelegate( this, &CProcess::HandleOnIQFeedConnected ) );
  m_iqfeed.OnDisconnected.Add( MakeDelegate( this, &CProcess::HandleOnIQFeedDisconnected ) );
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

void CProcess::IQFeedConnect( void ) {
  if ( !m_bIQFeedConnected ) {
    
    m_iqfeed.Connect();
    m_bIQFeedConnected = true;
  }
}

void CProcess::IQFeedDisconnect( void ) {
  if ( m_bIQFeedConnected ) {
    m_iqfeed.Disconnect();
    m_bIQFeedConnected = false;
  }
}

void CProcess::HandleOnIBConnected(int e) {
  // obtain strike list of underlying instrument

  m_contract.right = "CALL";

  m_vStrikes.clear(); /// horribly buggy this way.

  m_tws.SetOnContractDetailsHandler( MakeDelegate( this, &CProcess::HandleStrikeListing1 ) );
  m_tws.SetOnContractDetailsDoneHandler( MakeDelegate( this, &CProcess::HandleStrikeListing1Done ) );
  m_tws.RequestContractDetails( m_contract );

}

void CProcess::StartWatch( void ) {

  std::sort( m_vStrikes.begin(), m_vStrikes.end() );
  std::sort( m_vCrossOverPoints.begin(), m_vCrossOverPoints.end() );

  m_iterOILatestGammaSelect = m_vStrikes.end();  // initialized for beginning of trading

  // start contract acquisition for calls
  m_iterStrikes = m_vStrikes.begin();

  m_contract.secType = "OPT";
  m_contract.right = "CALL";

  m_contract.strike = m_iterStrikes->Strike();

  m_tws.SetOnContractDetailsHandler( MakeDelegate( this, &CProcess::HandleStrikeListing3 ) );
  m_tws.SetOnContractDetailsDoneHandler( MakeDelegate( this, &CProcess::HandleStrikeListing3Done ) );
  m_tws.RequestContractDetails( m_contract );
}

void CProcess::StopWatch( void ) {
  if ( m_bWatchingOptions ) {
    m_bWatchingOptions = false;
    for ( std::vector<COptionInfo>::iterator iter = m_iterOILowestWatch; iter != m_iterOIHighestWatch; ++iter ) {
      m_tws.RemoveQuoteHandler( iter->CallSymbol()->GetId(), MakeDelegate( &(*iter), &COptionInfo::HandleCallQuote ) );
      m_tws.RemoveTradeHandler( iter->CallSymbol()->GetId(), MakeDelegate( &(*iter), &COptionInfo::HandleCallTrade ) );
      m_tws.RemoveQuoteHandler( iter->PutSymbol()->GetId(), MakeDelegate( &(*iter), &COptionInfo::HandlePutQuote ) );
      m_tws.RemoveTradeHandler( iter->PutSymbol()->GetId(), MakeDelegate( &(*iter), &COptionInfo::HandlePutTrade ) );
    }
  }
}

// --- listing 1 listing of strikes


void CProcess::HandleStrikeListing1( const ContractDetails& details ) {
  m_vCrossOverPoints.push_back( details.summary.strike );
  COptionInfo oi( details.summary.strike );
  m_vStrikes.push_back( oi );
}

void CProcess::HandleStrikeListing1Done(  ) {
  m_ss.str( "" );
  m_ss << "#strikes: " << m_vStrikes.size() << std::endl;
  OutputDebugString( m_ss.str().c_str() );

  m_contract.secType = "STK";

  m_tws.SetOnContractDetailsHandler( MakeDelegate( this, &CProcess::HandleStrikeListing2 ) );
  m_tws.SetOnContractDetailsDoneHandler( MakeDelegate( this, &CProcess::HandleStrikeListing2Done ) );
  m_tws.RequestContractDetails( m_contract );
}

// --- listing 2

void CProcess::HandleStrikeListing2( const ContractDetails& details ) {
  m_contractidUnderlying = details.summary.conId;
  try {
    pUnderlying = m_tws.GetSymbol( m_contractidUnderlying );
  }
  catch ( std::out_of_range& e ) {
    CIBTWS::pInstrument_t instrument = m_tws.BuildInstrumentFromContract( details.summary );
    pUnderlying = m_tws.GetSymbol( instrument );
  }

}

void CProcess::HandleStrikeListing2Done(  ) {
}

// --- listing 3

void CProcess::HandleStrikeListing3( const ContractDetails& details ) {
  CIBSymbol* pSymbol;
  try {
    pSymbol = m_tws.GetSymbol( details.summary.conId );
  }
  catch ( std::out_of_range& e ) {
    CIBTWS::pInstrument_t instrument = m_tws.BuildInstrumentFromContract( details.summary );
    pSymbol = m_tws.GetSymbol( instrument );
  }
  m_iterStrikes->CallSymbol( pSymbol );

//  m_tws.AddQuoteHandler( pSymbol->GetId(), MakeDelegate( &(*m_iterStrikes), &COptionInfo::HandleCallQuote ) );
//  m_tws.AddTradeHandler( pSymbol->GetId(), MakeDelegate( &(*m_iterStrikes), &COptionInfo::HandleCallTrade ) );

  ++m_iterStrikes;
  if ( m_vStrikes.end() != m_iterStrikes ) {
    m_contract.strike = m_iterStrikes->Strike();
    m_tws.RequestContractDetails( m_contract );
  }
  else {
    m_iterStrikes = m_vStrikes.begin();
    m_contract.right = "PUT";

    m_contract.strike = m_iterStrikes->Strike();

    m_tws.SetOnContractDetailsHandler( MakeDelegate( this, &CProcess::HandleStrikeListing4 ) );
    m_tws.SetOnContractDetailsDoneHandler( MakeDelegate( this, &CProcess::HandleStrikeListing4Done ) );
    m_tws.RequestContractDetails( m_contract );
  }
}

void CProcess::HandleStrikeListing3Done(  ) {
}

// --- listing 4

void CProcess::HandleStrikeListing4( const ContractDetails& details ) {
  CIBSymbol* pSymbol;
  try {
    pSymbol = m_tws.GetSymbol( details.summary.conId );
  }
  catch ( std::out_of_range& e ) {
    CIBTWS::pInstrument_t instrument = m_tws.BuildInstrumentFromContract( details.summary );
    pSymbol = m_tws.GetSymbol( instrument );
  }
  m_iterStrikes->PutSymbol( pSymbol );

//  m_tws.AddQuoteHandler( pSymbol->GetId(), MakeDelegate( &(*m_iterStrikes), &COptionInfo::HandlePutQuote ) );
//  m_tws.AddTradeHandler( pSymbol->GetId(), MakeDelegate( &(*m_iterStrikes), &COptionInfo::HandlePutTrade ) );

  ++m_iterStrikes;
  if ( m_vStrikes.end() != m_iterStrikes ) {
    m_contract.strike = m_iterStrikes->Strike();
    m_tws.RequestContractDetails( m_contract );
  }
  else {
    // all done
    m_ss.str( "" );
    m_ss << "Option Acquisition Complete" << std::endl;
    OutputDebugString( m_ss.str().c_str() );

    m_tws.AddQuoteHandler( pUnderlying->GetId(), MakeDelegate( this, &CProcess::HandleMainQuote ) );
    m_tws.AddTradeHandler( pUnderlying->GetId(), MakeDelegate( this, &CProcess::HandleMainTrade1 ) );

  }
}

void CProcess::HandleStrikeListing4Done(  ) {
}

void CProcess::HandleOnIBDisconnected(int e) {
}

void CProcess::HandleOnIQFeedConnected(int e) {
  CIQFeedHistoryQuery<CProcess>::Connect();  
}

void CProcess::HandleOnIQFeedDisconnected(int e) {
}

void CProcess::OnHistoryConnected( void ) {
  CIQFeedHistoryQuery<CProcess>::RetrieveNEndOfDays( m_sSymbolName, 1 );
}

void CProcess::OnHistorySummaryData( structSummary* pDP ) {
  m_Bar.Open( pDP->Open );
  m_Bar.Close( pDP->Close );
  m_Bar.High( pDP->High );
  m_Bar.Low( pDP->Low );
  m_Bar.Volume( pDP->PeriodVolume );
  m_Bar.DateTime( pDP->DateTime );
  ReQueueSummary( pDP );
}

void CProcess::OnHistoryRequestDone( void ) {
  CIQFeedHistoryQuery<CProcess>::Disconnect();  
  CPivotSet pivots;
  pivots.CalcPivots( m_sSymbolName, m_Bar.High(), m_Bar.Low(), m_Bar.Close() );
  m_vCrossOverPoints.push_back( pivots.GetPivotValue( CPivotSet::R3 ) );
  m_vCrossOverPoints.push_back( pivots.GetPivotValue( CPivotSet::R2 ) );
  m_vCrossOverPoints.push_back( pivots.GetPivotValue( CPivotSet::R1 ) );
  m_vCrossOverPoints.push_back( pivots.GetPivotValue( CPivotSet::PV ) );
  m_vCrossOverPoints.push_back( pivots.GetPivotValue( CPivotSet::S1 ) );
  m_vCrossOverPoints.push_back( pivots.GetPivotValue( CPivotSet::S2 ) );
  m_vCrossOverPoints.push_back( pivots.GetPivotValue( CPivotSet::S3 ) );
}

void CProcess::StartTrading( void ) {
}

void CProcess::StopTrading( void ) {
}

void CProcess::HandleMainQuote( const CQuote& quote ) {
//  m_ss.str( "" );
//  m_ss << "Quote: " << quote.Bid() << "/" << quote.Ask() << std::endl;
//  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::HandleMainTrade1( const CTrade& trade ) {

  double dblTrade = trade.Trade();

  m_ss.str( "" );
  m_ss << "Trade 1: " << trade.Volume() << "@" << trade.Trade() << std::endl;
  OutputDebugString( m_ss.str().c_str() );

  m_tws.RemoveTradeHandler( pUnderlying->GetId(), MakeDelegate( this, &CProcess::HandleMainTrade1 ) );
     m_tws.AddTradeHandler( pUnderlying->GetId(), MakeDelegate( this, &CProcess::HandleMainTradeN ) );

  // set iterators for center of the pack:
  m_iterAboveCrossOver = m_vCrossOverPoints.begin();
  while ( dblTrade >= *m_iterAboveCrossOver ) {
    ++m_iterAboveCrossOver;
  }
  m_iterBelowCrossOver = m_iterAboveCrossOver;
  while ( dblTrade <= *m_iterBelowCrossOver ) {
    --m_iterBelowCrossOver;
  }

  m_ss.str( "" );
  m_ss << "Trade start " << *m_iterBelowCrossOver << ", " << dblTrade << ", " << *m_iterAboveCrossOver << std::endl;
  OutputDebugString( m_ss.str().c_str() );

  m_iterOIHighestWatch = m_vStrikes.begin();
  while ( *m_iterOIHighestWatch <= dblTrade ) {
    ++m_iterOIHighestWatch;
  }
  m_iterOILowestWatch = m_iterOIHighestWatch;
  --m_iterOILowestWatch;
  for ( int i = 0; i < 10; ++i ) { // set watch 10 strikes above and below current trade
    if ( m_vStrikes.begin() != m_iterOILowestWatch ) {
      --m_iterOILowestWatch;
    }
    if ( m_vStrikes.end() != m_iterOIHighestWatch ) {
      ++m_iterOIHighestWatch;
    }
  }

  m_bWatchingOptions = true;
  for ( std::vector<COptionInfo>::iterator iter = m_iterOILowestWatch; iter != m_iterOIHighestWatch; ++iter ) {
    m_tws.AddQuoteHandler( iter->CallSymbol()->GetId(), MakeDelegate( &(*iter), &COptionInfo::HandleCallQuote ) );
    m_tws.AddTradeHandler( iter->CallSymbol()->GetId(), MakeDelegate( &(*iter), &COptionInfo::HandleCallTrade ) );
    m_tws.AddQuoteHandler( iter->PutSymbol()->GetId(), MakeDelegate( &(*iter), &COptionInfo::HandlePutQuote ) );
    m_tws.AddTradeHandler( iter->PutSymbol()->GetId(), MakeDelegate( &(*iter), &COptionInfo::HandlePutTrade ) );
  }

}

void CProcess::HandleMainTradeN( const CTrade& trade ) {

  bool bCrossedOver = false;
  double dblTrade = trade.Trade();
  if ( dblTrade > *m_iterAboveCrossOver ) {
    // execute adjustment
    if ( ( 0 != m_nCalls ) && ( 0 != m_nPuts ) ) {
      double dblCallPrice = m_iterOILatestGammaSelect->CallAsk();
      double dblPutPrice = m_iterOILatestGammaSelect->PutAsk();
      double dblCallDiff = ( dblCallPrice - m_dblCallPrice ) * m_nCalls;
      double dblPutDiff = ( dblPutPrice - m_dblPutPrice ) * m_nPuts;
      double dblDiff = dblCallDiff + dblPutDiff;
      m_ss.str( "" );
      m_ss << "Sell Up: " << dblCallDiff << "+" << dblPutDiff << "=" << dblDiff;
      if ( 0 < dblDiff ) {
        m_nCalls = m_nPuts = 0;
        m_ss << " (sold)";
      }
      m_ss << std::endl;
      OutputDebugString( m_ss.str().c_str() );
    }
    // migrate upwards
    ++m_iterAboveCrossOver;  // only step by one step per trade
    m_iterBelowCrossOver = m_iterAboveCrossOver;
    while ( dblTrade <= *m_iterBelowCrossOver ) {
      --m_iterBelowCrossOver;
    }
    m_ss.str( "" );
    m_ss << "Trade upwards " << *m_iterBelowCrossOver << ", " << dblTrade << ", " << *m_iterAboveCrossOver << std::endl;
    OutputDebugString( m_ss.str().c_str() );
    bCrossedOver = true;
  }
  else {
    if ( dblTrade < *m_iterBelowCrossOver ) {
      // execute adjustment
      if ( ( 0 != m_nCalls ) && ( 0 != m_nPuts ) ) {
        double dblCallPrice = m_iterOILatestGammaSelect->CallBid();
        double dblPutPrice = m_iterOILatestGammaSelect->PutBid();
        double dblCallDiff = ( dblCallPrice - m_dblCallPrice ) * m_nCalls;
        double dblPutDiff = ( dblPutPrice - m_dblPutPrice ) * m_nPuts;
        double dblDiff = dblCallDiff + dblPutDiff;
        m_ss.str( "" );
        m_ss << "Sell Dn: " << dblCallDiff << "+" << dblPutDiff << "=" << dblDiff;
        if ( 0 < dblDiff ) {
          m_nCalls = m_nPuts = 0;
          m_ss << " (sold)";
        }
        m_ss << std::endl;
        OutputDebugString( m_ss.str().c_str() );
      }
      // migrate downwards
      --m_iterBelowCrossOver;  // only step by one step per trade
      m_iterAboveCrossOver = m_iterBelowCrossOver;
      while ( dblTrade >= *m_iterAboveCrossOver ) {
        ++m_iterAboveCrossOver;
      }
    m_ss.str( "" );
    m_ss << "Trade downwards " << *m_iterBelowCrossOver << ", " << dblTrade << ", " << *m_iterAboveCrossOver << std::endl;
    OutputDebugString( m_ss.str().c_str() );
    bCrossedOver = true;
    }
  }

  if ( bCrossedOver ) {
    if ( m_vStrikes.end() != m_iterOILatestGammaSelect ) {
      // output greeks for previous acquisition
      m_ss.str( "" );
      m_ss << "Prev Xvr: " 
        << m_iterOILatestGammaSelect->Strike() << " -- "
        << m_iterOILatestGammaSelect->CallSymbol()->OptionPrice() << ", "
        << m_iterOILatestGammaSelect->CallSymbol()->Delta() << ", "
        << m_iterOILatestGammaSelect->CallSymbol()->Gamma() << " - "
        << m_iterOILatestGammaSelect->PutSymbol()->OptionPrice() << ", "
        << m_iterOILatestGammaSelect->PutSymbol()->Delta() << ", "
        << m_iterOILatestGammaSelect->PutSymbol()->Gamma() 
        << std::endl;
      OutputDebugString( m_ss.str().c_str() );
    }
    double gamma = 0;
    m_iterOILatestGammaSelect = m_iterOIHighestWatch;
    for ( std::vector<COptionInfo>::iterator iter = m_iterOILowestWatch; iter != m_iterOIHighestWatch; ++iter ) {
      double tmp1 = iter->CallSymbol()->Gamma();
      double tmp2 = iter->PutSymbol()->Gamma();
      if ( gamma < tmp1 ) {
        gamma = tmp1;
        m_iterOILatestGammaSelect = iter;
      }
    }
    if ( m_iterOIHighestWatch == m_iterOILatestGammaSelect ) {
      // problems
    }
    else {
      // buy in
      m_dblCallPrice = m_iterOILatestGammaSelect->CallAsk();
      m_dblPutPrice = m_iterOILatestGammaSelect->PutAsk();
      m_nCalls = ( 5000.0 / m_iterOILatestGammaSelect->CallSymbol()->Delta() ) / 100;
      m_nPuts = ( 5000.0 / m_iterOILatestGammaSelect->PutSymbol()->Delta() ) / 100;
      m_ss.str( "" );
      m_ss << "Bought: C" << m_nCalls << "@" << m_dblCallPrice << " for " << m_nCalls * m_dblCallPrice
                 << ", P" << m_nPuts << "@" << m_dblPutPrice << " for " << m_nPuts * m_dblPutPrice << std::endl;
      OutputDebugString( m_ss.str().c_str() );
      // status out
      m_ss.str( "" );
      m_ss << "Cur Xvr: " 
        << m_iterOILatestGammaSelect->Strike() << " -- "
        << m_iterOILatestGammaSelect->CallSymbol()->OptionPrice() << ", "
        << m_iterOILatestGammaSelect->CallSymbol()->Delta() << ", "
        << m_iterOILatestGammaSelect->CallSymbol()->Gamma() << " - "
        << m_iterOILatestGammaSelect->PutSymbol()->OptionPrice() << ", "
        << m_iterOILatestGammaSelect->PutSymbol()->Delta() << ", "
        << m_iterOILatestGammaSelect->PutSymbol()->Gamma() 
        << std::endl;
      OutputDebugString( m_ss.str().c_str() );
    }
  }

}

// need to worry about rogue trades... trades out side of the normal trading range
// capture quotes and trades and write to datebase afterwards
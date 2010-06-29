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

CNakedOption::CNakedOption( double dblStrike ) 
: m_dblBid( 0 ), m_dblAsk( 0 ),
  m_dblStrike( dblStrike ),
  m_dblImpliedVolatility( 0 ),
  m_dblDelta( 0 ), m_dblGamma( 0 ), m_dblVega( 0 ), m_dblTheta( 0 ),
  m_bWatching( false ),
  m_pSymbol( NULL )
{
}

CNakedOption::CNakedOption( const CNakedOption& rhs ) 
: m_dblBid( rhs.m_dblBid ), m_dblAsk( rhs.m_dblAsk ),
  m_dblStrike( rhs.m_dblStrike ),
  m_dblImpliedVolatility( 0 ),
  m_dblDelta( rhs.m_dblDelta ), m_dblGamma( rhs.m_dblGamma ), m_dblVega( rhs.m_dblVega ), m_dblTheta( rhs.m_dblVega ),
  m_bWatching( false ),
  m_pSymbol( rhs.m_pSymbol )
{
  assert( !rhs.m_bWatching );
}

CNakedOption& CNakedOption::operator=( const CNakedOption& rhs ) {
  assert( !rhs.m_bWatching );
  assert( !m_bWatching );
  m_dblStrike = rhs.m_dblStrike;
  m_dblImpliedVolatility = rhs.m_dblImpliedVolatility;
  m_dblDelta = rhs.m_dblDelta;
  m_dblGamma = rhs.m_dblGamma;
  m_dblVega = rhs.m_dblVega;
  m_dblTheta = rhs.m_dblTheta;
  m_pSymbol = rhs.m_pSymbol;
  return *this;
}

void CNakedOption::HandleQuote( const CQuote& quote ) {
  m_dblBid = quote.Bid();
  m_dblAsk = quote.Ask();
}

void CNakedOption::HandleTrade( const CTrade& trade ) {
  m_ss.str( "" );
  m_ss << "Trade(" << m_sSide << m_dblStrike << "): " << trade.Volume() << "@" << trade.Trade() << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CNakedOption::HandleGreeks( double ImplVol, double Delta, double Gamma, double Vega, double Theta ) {
  m_dblImpliedVolatility = ImplVol;
  m_dblDelta = Delta;
  m_dblGamma = Gamma;
  m_dblVega = Vega;
  m_dblTheta = Theta;
}

//
// ==================
//

CNakedCall::CNakedCall( double dblStrike )
: CNakedOption( dblStrike )
{
  m_sSide = "C";
}

//
// ==================
//

CNakedPut::CNakedPut( double dblStrike )
: CNakedOption( dblStrike )
{
  m_sSide = "P";
}


//
// ==================
//

COptionInfo::COptionInfo( double dblStrike ) 
: m_dblStrike( dblStrike ),
  m_call( dblStrike ), m_put( dblStrike ),
  m_bWatching( false )
{
}

COptionInfo::COptionInfo( const COptionInfo& rhs ) 
: m_dblStrike( rhs.m_dblStrike ),
  m_call( rhs.m_call ), m_put( rhs.m_put ),
  m_bWatching( false )
{ 
  assert( !rhs.m_bWatching );
}

COptionInfo::~COptionInfo( void ) {
}

COptionInfo& COptionInfo::operator=( const COptionInfo& rhs ) {
  assert( !rhs.m_bWatching );
  assert( !m_bWatching );
  m_dblStrike = rhs.m_dblStrike;
  m_call = rhs.m_call;
  m_put = rhs.m_put;
  return *this;
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
  m_bWatchingOptions( false ), m_bTrading( false )
{

  m_contract.currency = "USD";
  m_contract.exchange = "SMART";
  m_contract.secType = "OPT";
  m_contract.symbol = m_sSymbolName;
  m_contract.expiry = "20100716";

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

  m_iterOILatestGammaSelectCall = m_vStrikes.end();  // initialized for beginning of trading
  m_iterOILatestGammaSelectPut = m_vStrikes.end();  // initialized for beginning of trading

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
      m_tws.RemoveQuoteHandler( iter->Call()->Symbol()->GetId(), MakeDelegate( iter->Call(), &CNakedOption::HandleQuote ) );
      m_tws.RemoveTradeHandler( iter->Call()->Symbol()->GetId(), MakeDelegate( iter->Call(), &CNakedOption::HandleTrade ) );
      m_tws.RemoveQuoteHandler( iter->Put()->Symbol()->GetId(), MakeDelegate( iter->Put(), &CNakedOption::HandleQuote ) );
      m_tws.RemoveTradeHandler( iter->Put()->Symbol()->GetId(), MakeDelegate( iter->Put(), &CNakedOption::HandleTrade ) );
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

// --- listing 3 -- Call Contracts

void CProcess::HandleStrikeListing3( const ContractDetails& details ) {
  CIBSymbol* pSymbol;
  try {
    pSymbol = m_tws.GetSymbol( details.summary.conId );
  }
  catch ( std::out_of_range& e ) {
    CIBTWS::pInstrument_t instrument = m_tws.BuildInstrumentFromContract( details.summary );
    pSymbol = m_tws.GetSymbol( instrument );
  }
  m_iterStrikes->Call()->Symbol( pSymbol );

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

// --- listing 4 -- Put Contracts

void CProcess::HandleStrikeListing4( const ContractDetails& details ) {
  CIBSymbol* pSymbol;
  try {
    pSymbol = m_tws.GetSymbol( details.summary.conId );
  }
  catch ( std::out_of_range& e ) {
    CIBTWS::pInstrument_t instrument = m_tws.BuildInstrumentFromContract( details.summary );
    pSymbol = m_tws.GetSymbol( instrument );
  }
  m_iterStrikes->Put()->Symbol( pSymbol );

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

void CProcess::OpenPosition( void ) {

  // assert( !m_bTrading );

  // if no position, create a zero delta position.
  if ( ( 0 == m_nCalls ) && ( 0 == m_nPuts ) ) {
    double gammaCall = 0;
    double gammaPut = 0;
    double gamma;
    m_iterOILatestGammaSelectCall = m_iterOILowestWatch;
    m_iterOILatestGammaSelectPut = m_iterOIHighestWatch;
    // find highest gamma option for call, put
    for ( std::vector<COptionInfo>::iterator iter = m_iterOILowestWatch; iter != m_iterOIHighestWatch; ++iter ) {
      gamma = iter->Call()->Symbol()->Gamma();
      if ( gammaCall < gamma ) { 
        gammaCall = gamma;
        m_iterOILatestGammaSelectCall = iter;
      }
      gamma = iter->Put()->Symbol()->Gamma();
      if ( gammaPut < gamma ) { 
        gammaPut = gamma;
        m_iterOILatestGammaSelectPut = iter;
      }
    }

    // check that SelectCall != LowestWatch and SelectPut != HighestWatch?

    m_dblCallPrice = m_iterOILatestGammaSelectCall->Call()->Ask();
    m_dblPutPrice = m_iterOILatestGammaSelectPut->Put()->Ask();

    m_nCalls = ( 1000.0 / m_iterOILatestGammaSelectCall->Call()->Symbol()->Delta() ) / 100;
    m_nPuts = -1 * ( 1000.0 / m_iterOILatestGammaSelectPut->Put()->Symbol()->Delta() ) / 100;

    if ( ( 0 == m_nCalls ) || ( 0 == m_nPuts ) ) {
      // don't buy anything if either side is zero
      m_ss.str( "" );
      m_ss << "puts or calls are zero" << std::endl;
      OutputDebugString( m_ss.str().c_str() );
      m_nCalls = m_nPuts = 0;
    }
    else {
      COrder* pOrder;
      pOrder = new COrder( m_iterOILatestGammaSelectCall->Call()->Symbol()->GetInstrument(), 
        OrderType::Market, OrderSide::Buy, m_nCalls );
      m_tws.PlaceOrder( pOrder );
      pOrder = new COrder( m_iterOILatestGammaSelectPut->Put()->Symbol()->GetInstrument(), 
        OrderType::Market, OrderSide::Buy, m_nPuts );
      m_tws.PlaceOrder( pOrder );

      m_ss.str( "" );
      m_ss << "Opening Trade: C" << m_nCalls << "@" << m_dblCallPrice << " for " << 100 * m_nCalls * m_dblCallPrice
                        << ", P" << m_nPuts  << "@" << m_dblPutPrice  << " for " << 100 * m_nPuts  * m_dblPutPrice 
                        << std::endl;
      OutputDebugString( m_ss.str().c_str() );
    }
  }

  // status out
  m_ss.str( "" );
  m_ss << "Cur Xvr: " 
    << " Strk "  << m_iterOILatestGammaSelectCall->Strike()
    << " Call "  << m_iterOILatestGammaSelectCall->Call()->Symbol()->OptionPrice()
    << " Delta " << m_iterOILatestGammaSelectCall->Call()->Symbol()->Delta()
    << " Gamma " << m_iterOILatestGammaSelectCall->Call()->Symbol()->Gamma() << " - "
    << " Put "   << m_iterOILatestGammaSelectPut->Put()->Symbol()->OptionPrice()
    << " Delta " << m_iterOILatestGammaSelectPut->Put()->Symbol()->Delta()
    << " Gamma " << m_iterOILatestGammaSelectPut->Put()->Symbol()->Gamma() 
    << std::endl 
    << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::StartTrading( void ) {

  m_bTrading = true;

  OpenPosition();

}

void CProcess::StopTrading( void ) {

  COrder* pOrder;
  if ( 0 != m_nCalls ) {
    pOrder = new COrder( m_iterOILatestGammaSelectCall->Call()->Symbol()->GetInstrument(), 
      OrderType::Market, OrderSide::Sell, m_nCalls );
    m_tws.PlaceOrder( pOrder );
  }
  if ( 0 != m_nPuts ) {
    pOrder = new COrder( m_iterOILatestGammaSelectPut->Put()->Symbol()->GetInstrument(), 
      OrderType::Market, OrderSide::Sell, m_nPuts );
    m_tws.PlaceOrder( pOrder );
  }

  m_ss.str( "" );
  m_ss << "Opening Trade: C" << m_nCalls << "@" << m_dblCallPrice << " for " << 100 * m_nCalls * m_dblCallPrice
                    << ", P" << m_nPuts  << "@" << m_dblPutPrice  << " for " << 100 * m_nPuts  * m_dblPutPrice 
                    << std::endl;
  OutputDebugString( m_ss.str().c_str() );

  m_bTrading = false;
}

void CProcess::HandleMainQuote( const CQuote& quote ) {
//  m_ss.str( "" );
//  m_ss << "Quote: " << quote.Bid() << "/" << quote.Ask() << std::endl;
//  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::HandleMainTrade1( const CTrade& trade ) {

  double dblTrade = trade.Trade();

  // comment our starting trade of the day
  m_ss.str( "" );
  m_ss << "Trade 1: " << trade.Volume() << "@" << trade.Trade() << std::endl;
  OutputDebugString( m_ss.str().c_str() );

  // after initialization in this method, hand off subsequent procesing ot main handler
  m_tws.RemoveTradeHandler( pUnderlying->GetId(), MakeDelegate( this, &CProcess::HandleMainTrade1 ) );
     m_tws.AddTradeHandler( pUnderlying->GetId(), MakeDelegate( this, &CProcess::HandleMainTradeN ) );

  // set iterators for center of the pack ( crossovers are above and below trade):
  m_iterAboveCrossOver = m_vCrossOverPoints.begin();
  while ( dblTrade >= *m_iterAboveCrossOver ) {
    ++m_iterAboveCrossOver;
  }
  m_iterBelowCrossOver = m_iterAboveCrossOver;
  while ( dblTrade <= *m_iterBelowCrossOver ) {
    --m_iterBelowCrossOver;
  }

  // comment our crossover points
  m_ss.str( "" );
  m_ss << "Trade start " << *m_iterBelowCrossOver << ", " << dblTrade << ", " << *m_iterAboveCrossOver << std::endl;
  OutputDebugString( m_ss.str().c_str() );

  // calculate where to have put/call option watches,
  //   have a range of 10 strikes above and below current trade (have maximum 100 watches available)
  m_iterOIHighestWatch = m_vStrikes.begin();
  while ( *m_iterOIHighestWatch <= dblTrade ) {
    ++m_iterOIHighestWatch;
  }
  m_iterOILowestWatch = m_iterOIHighestWatch;
  --m_iterOILowestWatch;
  for ( int i = 0; i < 20; ++i ) {
    if ( m_vStrikes.begin() != m_iterOILowestWatch ) {
      --m_iterOILowestWatch;
    }
    if ( m_vStrikes.end() != m_iterOIHighestWatch ) {
      ++m_iterOIHighestWatch;
    }
  }

  // set the actual watches
  m_bWatchingOptions = true;
  for ( std::vector<COptionInfo>::iterator iter = m_iterOILowestWatch; iter != m_iterOIHighestWatch; ++iter ) {
    m_tws.AddQuoteHandler( iter->Call()->Symbol()->GetId(), MakeDelegate( iter->Call(), &CNakedOption::HandleQuote ) );
    m_tws.AddTradeHandler( iter->Call()->Symbol()->GetId(), MakeDelegate( iter->Call(), &CNakedOption::HandleTrade ) );
    m_tws.AddQuoteHandler( iter->Put()->Symbol()->GetId(), MakeDelegate( iter->Put(), &CNakedOption::HandleQuote ) );
    m_tws.AddTradeHandler( iter->Put()->Symbol()->GetId(), MakeDelegate( iter->Put(), &CNakedOption::HandleTrade ) );
  }

}

void CProcess::HandleMainTradeN( const CTrade& trade ) {

  bool bCrossedOver = false;
  double dblTrade = trade.Trade();
  if ( dblTrade > *m_iterAboveCrossOver ) {
    // execute adjustment
    if ( ( 0 != m_nCalls ) && ( 0 != m_nPuts ) ) {
      double dblCallPrice = m_iterOILatestGammaSelectCall->Call()->Ask();
      double dblPutPrice = m_iterOILatestGammaSelectPut->Put()->Ask();
      double dblCallDiff = ( dblCallPrice - m_dblCallPrice ) * m_nCalls;
      double dblPutDiff = ( dblPutPrice - m_dblPutPrice ) * m_nPuts;
      double dblDiff = dblCallDiff + dblPutDiff;
      m_ss.str( "" );
      m_ss << "Sell Up Calc: " << dblCallDiff << "+" << dblPutDiff << "=" << dblDiff;
      if ( 0 < dblDiff ) {
        m_ss << " (sold)";

          if ( ( 0 < m_nCalls ) && ( 0 < m_nPuts ) ) {
            COrder* pOrder;
            pOrder = new COrder( m_iterOILatestGammaSelectCall->Call()->Symbol()->GetInstrument(), 
              OrderType::Market, OrderSide::Sell, m_nCalls );
            m_tws.PlaceOrder( pOrder );
            pOrder = new COrder( m_iterOILatestGammaSelectPut->Put()->Symbol()->GetInstrument(), 
              OrderType::Market, OrderSide::Sell, m_nPuts );
            m_tws.PlaceOrder( pOrder );
          }

        m_nCalls = m_nPuts = 0;
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
        double dblNewCallPrice = m_iterOILatestGammaSelectCall->Call()->Bid();
        double dblNewPutPrice = m_iterOILatestGammaSelectPut->Put()->Bid();
        double dblCallDiff = ( dblNewCallPrice - m_dblCallPrice ) * m_nCalls;
        double dblPutDiff = ( dblNewPutPrice - m_dblPutPrice ) * m_nPuts;
        double dblDiff = dblCallDiff + dblPutDiff;
        m_ss.str( "" );
        m_ss << "Sell Dn Calc: " << dblCallDiff << "+" << dblPutDiff << "=" << dblDiff;
        if ( 0 < dblDiff ) {
          m_ss << " (sold)";

          if ( ( 0 < m_nCalls ) && ( 0 < m_nPuts ) ) {
            COrder* pOrder;
            pOrder = new COrder( m_iterOILatestGammaSelectCall->Call()->Symbol()->GetInstrument(), 
              OrderType::Market, OrderSide::Sell, m_nCalls );
            m_tws.PlaceOrder( pOrder );
            pOrder = new COrder( m_iterOILatestGammaSelectPut->Put()->Symbol()->GetInstrument(), 
              OrderType::Market, OrderSide::Sell, m_nPuts );
            m_tws.PlaceOrder( pOrder );
          }

          m_nCalls = m_nPuts = 0;
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
    if ( m_vStrikes.end() != m_iterOILatestGammaSelectCall ) {
      // output greeks for previous acquisition
      m_ss.str( "" );
      m_ss << "Prev Xvr: " 
        << " Strk "  << m_iterOILatestGammaSelectCall->Strike()
        << " Call "  << m_iterOILatestGammaSelectCall->Call()->Symbol()->OptionPrice()
        << " Delta " << m_iterOILatestGammaSelectCall->Call()->Symbol()->Delta()
        << " Gamma " << m_iterOILatestGammaSelectCall->Call()->Symbol()->Gamma() << " - "
        << " Put "   << m_iterOILatestGammaSelectPut->Put()->Symbol()->OptionPrice()
        << " Delta " << m_iterOILatestGammaSelectPut->Put()->Symbol()->Delta()
        << " Gamma " << m_iterOILatestGammaSelectPut->Put()->Symbol()->Gamma() 
        << std::endl;
      OutputDebugString( m_ss.str().c_str() );
    }
  }

  if ( bCrossedOver && ( 0 == m_nCalls ) && ( 0 == m_nPuts ) ) {  // don't do anything unless we've cleared the slate
    OpenPosition();
  }

}

// need to worry about rogue trades... trades out side of the normal trading range
// capture quotes, trades, greeks and write to database afterwards
// handle executions
// manage multiple position, add a new position on each cross over.
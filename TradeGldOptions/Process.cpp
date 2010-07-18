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

#include <math.h>

#include "Process.h"

#include <LibIndicators/Pivots.h>
#include <LibHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <LibHDF5TimeSeries/HDF5DataManager.h>

//
// ==================
//

CNakedOption::CNakedOption( double dblStrike ) 
: m_dblBid( 0 ), m_dblAsk( 0 ), m_dblTrade( 0 ),
  m_dblStrike( dblStrike ),
  m_bWatching( false ),
  m_sSide( "-" ),
  m_pSymbol( NULL )
{
}

CNakedOption::CNakedOption( const CNakedOption& rhs ) 
: m_dblBid( rhs.m_dblBid ), m_dblAsk( rhs.m_dblAsk ), m_dblTrade( rhs.m_dblTrade ),
  m_dblStrike( rhs.m_dblStrike ),
  m_greek( rhs.m_greek ),
  m_bWatching( false ),
  m_sSide( rhs.m_sSide ),
  m_pSymbol( rhs.m_pSymbol )
{
  assert( !rhs.m_bWatching );
}

CNakedOption& CNakedOption::operator=( const CNakedOption& rhs ) {
  assert( !rhs.m_bWatching );
  assert( !m_bWatching );
  m_dblStrike = rhs.m_dblStrike;
  m_greek = rhs.m_greek;
  m_sSide = rhs.m_sSide;
  m_pSymbol = rhs.m_pSymbol;
  return *this;
}

void CNakedOption::HandleQuote( const CQuote& quote ) {
  m_dblBid = quote.Bid();
  m_dblAsk = quote.Ask();
  m_quotes.Append( quote );
}

void CNakedOption::HandleTrade( const CTrade& trade ) {
  m_dblTrade = trade.Trade();
  m_trades.Append( trade );
}

void CNakedOption::HandleGreek( const CGreek& greek ) {
  m_greek = greek;
  m_greeks.Append( greek );
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

CStrikeInfo::CStrikeInfo( double dblStrike ) 
: m_dblStrike( dblStrike ),
  m_call( dblStrike ), m_put( dblStrike ),
  m_bWatching( false )
{
}

CStrikeInfo::CStrikeInfo( const CStrikeInfo& rhs ) 
: m_dblStrike( rhs.m_dblStrike ),
  m_call( rhs.m_call ), m_put( rhs.m_put ),
  m_bWatching( false )
{ 
  assert( !rhs.m_bWatching );
}

CStrikeInfo::~CStrikeInfo( void ) {
}

CStrikeInfo& CStrikeInfo::operator=( const CStrikeInfo& rhs ) {
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
  m_nCalls( 0 ), m_nPuts( 0 ), m_nLongPut( 0 ), m_nLongUnderlying( 0 ),
  m_bWatchingOptions( false ), m_bTrading( false ),
  m_dblBaseDelta( 1000.0 ), m_dblBaseDeltaIncrement( 100.0 ),
  m_TradingState( ETSFirstPass ), 
  m_dtMarketOpen( time_duration( 10, 30, 0 ) ),
  m_dtMarketOpeningOrder( time_duration( 10, 31, 0 ) ),
  m_dtMarketClosingOrder( time_duration( 16, 56, 0 ) ),
  m_dtMarketClose( time_duration( 17, 0, 0 ) ),
  m_sPathForSeries( "/strategy/deltaneutral1" )
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
    for ( std::vector<CStrikeInfo>::iterator iter = m_iterOILowestWatch; iter != m_iterOIHighestWatch; ++iter ) {

      m_tws.RemoveQuoteHandler( iter->Call()->Symbol()->GetId(), MakeDelegate( iter->Call(), &CNakedCall::HandleQuote ) );
      m_tws.RemoveTradeHandler( iter->Call()->Symbol()->GetId(), MakeDelegate( iter->Call(), &CNakedCall::HandleTrade ) );
      m_tws.RemoveGreekHandler( iter->Call()->Symbol()->GetId(), MakeDelegate( iter->Call(), &CNakedCall::HandleGreek ) );

      m_tws.RemoveQuoteHandler( iter->Put()->Symbol()->GetId(),  MakeDelegate( iter->Put(),  &CNakedPut::HandleQuote ) );
      m_tws.RemoveTradeHandler( iter->Put()->Symbol()->GetId(),  MakeDelegate( iter->Put(),  &CNakedPut::HandleTrade ) );
      m_tws.RemoveGreekHandler( iter->Put()->Symbol()->GetId(),  MakeDelegate( iter->Put(),  &CNakedPut::HandleGreek ) );
    }
  }
}

// --- listing 1 listing of strikes


void CProcess::HandleStrikeListing1( const ContractDetails& details ) {
  m_vCrossOverPoints.push_back( details.summary.strike );
  CStrikeInfo oi( details.summary.strike );
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

//  m_tws.AddQuoteHandler( pSymbol->GetId(), MakeDelegate( &(*m_iterStrikes), &CStrikeInfo::HandleCallQuote ) );
//  m_tws.AddTradeHandler( pSymbol->GetId(), MakeDelegate( &(*m_iterStrikes), &CStrikeInfo::HandleCallTrade ) );

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

//  m_tws.AddQuoteHandler( pSymbol->GetId(), MakeDelegate( &(*m_iterStrikes), &CStrikeInfo::HandlePutQuote ) );
//  m_tws.AddTradeHandler( pSymbol->GetId(), MakeDelegate( &(*m_iterStrikes), &CStrikeInfo::HandlePutTrade ) );

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

    m_tws.AddQuoteHandler( pUnderlying->GetId(), MakeDelegate( this, &CProcess::HandleUnderlyingQuote ) );
    m_tws.AddTradeHandler( pUnderlying->GetId(), MakeDelegate( this, &CProcess::HandleUnderlyingTrade ) );

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
    // find highest gamma option for call and for put
    for ( std::vector<CStrikeInfo>::iterator iter = m_iterOILowestWatch; iter != m_iterOIHighestWatch; ++iter ) {
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

    // todo?  check that SelectCall != LowestWatch and SelectPut != HighestWatch

    m_dblCallPrice = m_iterOILatestGammaSelectCall->Call()->Ask();
    m_dblPutPrice = m_iterOILatestGammaSelectPut->Put()->Ask();

    // depending upon gammas, could be straddle or strangle?
    m_nCalls = (int) floor(      ( m_dblBaseDelta / m_iterOILatestGammaSelectCall->Call()->Symbol()->Delta() ) / 100 );
    m_nPuts  = (int) floor( -1 * ( m_dblBaseDelta / m_iterOILatestGammaSelectPut ->Put() ->Symbol()->Delta() ) / 100 );

    // a normal delta neutral with long underlying and long put
    m_nLongPut = m_nPuts;
    m_nLongUnderlying = m_dblBaseDelta;

    // generate orders
    if ( ( 0 == m_nCalls ) || ( 0 == m_nPuts ) ) {
      // don't buy anything if either side is zero
      m_ss.str( "" );
      m_ss << "puts or calls are zero" << std::endl;
      OutputDebugString( m_ss.str().c_str() );
      m_nCalls = m_nPuts = 0;
    }
    else {
      COrder* pOrder;
      // orders for normal delta neutral
      pOrder = new COrder( pUnderlying->GetInstrument(), OrderType::Market, OrderSide::Buy, m_nLongUnderlying );
      m_tws.PlaceOrder( pOrder );
      pOrder = new COrder( m_iterOILatestGammaSelectPut->Put()->Symbol()->GetInstrument(), 
        OrderType::Market, OrderSide::Buy, m_nLongPut );
      m_tws.PlaceOrder( pOrder );

      m_ss.str( "" );
      m_ss << "Opening Delta N:  U" << m_nLongUnderlying << "@" << m_dblUnderlyingPrice << " for " << 100 * m_nLongUnderlying * m_dblUnderlyingPrice
                           << ", P" << m_nLongPut        << "@" << m_dblPutPrice        << " for " << 100 * m_nLongPut * m_dblPutPrice 
                           << std::endl;
      OutputDebugString( m_ss.str().c_str() );
    }
  }

  // status out
  PrintGreeks();
}

void CProcess::StartTrading( void ) {

}

void CProcess::StopTrading( void ) {

}

void CProcess::HandleUnderlyingQuote( const CQuote& quote ) {
//  m_ss.str( "" );
//  m_ss << "Quote: " << quote.Bid() << "/" << quote.Ask() << std::endl;
//  OutputDebugString( m_ss.str().c_str() );
  m_quotes.Append( quote );
}

void CProcess::HandleUnderlyingTrade( const CTrade& trade ) {

  m_dblUnderlyingPrice = trade.Trade();
  m_trades.Append( trade );

  switch ( m_TradingState ) {
    case ETSTrading:
      HandleTSTrading( trade );
      break;
    case ETSFirstPass:
      HandleTSFirstPass( trade );
      break;
    case ETSPreMarket:
      HandleTSPreMarket( trade );
      break;
//    case ETSMarketOpened: // first call from within HandleTSPreMarket
//      HandleTSMarketopened( trade );
//      break;
    case ETSFirstTrade:
      HandleTSOpeningOrder( trade );
      break;
    case ETSCloseOrders:
      HandleTSCloseOrders( trade );
      break;
    case ETSAfterMarket:
      HandleAfterMarket( trade );
      break;
    default:
      throw std::out_of_range( "CProcess::HandleUnderlyingTrade" );
      break;
  };

}

void CProcess::HandleTSFirstPass( const CTrade& trade ) {
  // may need to open portfoloio and evaluate existing positions here
  m_TradingState = ETSPreMarket;
  m_ss.str( "" );
  m_ss << m_ts.External();
  m_ss << " State:  First Pass -> Pre Market." << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::HandleTSPreMarket( const CTrade& trade ) {
  ptime dt;
  m_ts.External( &dt );
  if ( dt.time_of_day() >= m_dtMarketOpen ) {
    m_ss.str( "" );
    m_ss << m_ts.External();
    m_ss << " State:  Market Opened." << std::endl;
    OutputDebugString( m_ss.str().c_str() );
    m_TradingState = ETSMarketOpened;
    HandleTSMarketOpened( trade );
  }
}

void CProcess::HandleTSMarketOpened( const CTrade& trade ) {

  double dblTrade = trade.Trade();

  // comment our starting trade of the day
  m_ss.str( "" );
  m_ss << m_ts.External();
  m_ss << " Trade 1: " << trade.Volume() << "@" << trade.Trade() << std::endl;
  OutputDebugString( m_ss.str().c_str() );

  // after initialization in this method, hand off subsequent procesing to main handler
//  m_tws.RemoveTradeHandler( pUnderlying->GetId(), MakeDelegate( this, &CProcess::HandleMainTrade1 ) );
//     m_tws.AddTradeHandler( pUnderlying->GetId(), MakeDelegate( this, &CProcess::HandleMainTradeN ) );

  // set iterators for center of the pack (crossovers are above and below trade):
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
  //   have a range of strikes above and below current trade (have maximum 100 watches available)
  m_iterOIHighestWatch = m_vStrikes.begin();
  while ( *m_iterOIHighestWatch <= dblTrade ) {
    ++m_iterOIHighestWatch;
  }
  m_iterOILowestWatch = m_iterOIHighestWatch;
  --m_iterOILowestWatch;
  for ( int i = 0; i < 15; ++i ) {
    if ( m_vStrikes.begin() != m_iterOILowestWatch ) {
      --m_iterOILowestWatch;
    }
    if ( m_vStrikes.end() != m_iterOIHighestWatch ) {
      ++m_iterOIHighestWatch;
    }
  }

  // set the actual watches
  m_bWatchingOptions = true;
  for ( std::vector<CStrikeInfo>::iterator iter = m_iterOILowestWatch; iter != m_iterOIHighestWatch; ++iter ) {

    m_tws.AddQuoteHandler( iter->Call()->Symbol()->GetId(), MakeDelegate( iter->Call(), &CNakedCall::HandleQuote ) );
    m_tws.AddTradeHandler( iter->Call()->Symbol()->GetId(), MakeDelegate( iter->Call(), &CNakedCall::HandleTrade ) );
    m_tws.AddGreekHandler( iter->Call()->Symbol()->GetId(), MakeDelegate( iter->Call(), &CNakedCall::HandleGreek ) );

    m_tws.AddQuoteHandler( iter->Put()->Symbol()->GetId(),  MakeDelegate( iter->Put(),  &CNakedPut::HandleQuote ) );
    m_tws.AddTradeHandler( iter->Put()->Symbol()->GetId(),  MakeDelegate( iter->Put(),  &CNakedPut::HandleTrade ) );
    m_tws.AddGreekHandler( iter->Put()->Symbol()->GetId(),  MakeDelegate( iter->Put(),  &CNakedPut::HandleGreek ) );
  }

  m_TradingState = ETSFirstTrade;
}

void CProcess::HandleTSOpeningOrder( const CTrade& trade ) {

  ptime dt;
  m_ts.External( &dt );
  if ( dt.time_of_day() >= m_dtMarketOpeningOrder ) {
    m_ss.str( "" );
    m_ss << m_ts.External();
    m_ss << " State:  Opening Order." << std::endl;
    OutputDebugString( m_ss.str().c_str() );

    m_bTrading = true;
    OpenPosition();
    m_TradingState = ETSTrading;
  }
}

void CProcess::HandleTSTrading( const CTrade& trade ) {

//  m_dblCallPrice = m_iterOILatestGammaSelectCall->Call()->Ask();
//  m_dblPutPrice = m_iterOILatestGammaSelectPut->Put()->Ask();

  ptime dt;
  m_ts.External( &dt );
  if ( dt.time_of_day() >= m_dtMarketClosingOrder ) {
    m_ss.str( "" );
    m_ss << m_ts.External();
    m_ss << " State:  Close Orders." << std::endl;
    OutputDebugString( m_ss.str().c_str() );

    m_TradingState = ETSCloseOrders;
  }
  else {

    double dblDeltaPut  = m_iterOILatestGammaSelectPut->Put()->Symbol()->Delta() * m_nPuts * 100;
    double dblDeltaCall = m_iterOILatestGammaSelectCall->Call()->Symbol()->Delta() * m_nCalls * 100;

    bool bTraded = false;
    int nOptions = 0;
    COrder* pOrder;

    double dblDeltaHi = m_dblBaseDelta + m_dblBaseDeltaIncrement;
    double dblDeltaLo = m_dblBaseDelta - m_dblBaseDeltaIncrement;

    try {

      // todo:  while implied volatility is rising, hold back on exiting?
      // todo:  while implied volatility is falling, hold back on entering?


    }
    catch ( std::logic_error &e ) {
      // just catch, don't do anything
    }

    if ( bTraded ) {
      PrintGreeks();
    }
  }
}

void CProcess::HandleTSCloseOrders( const CTrade& trade ) {

  if ( m_bTrading ) {

    m_ss.str( "" );
    m_ss << "Closing Delta N:  U" << m_nLongUnderlying << "@" << m_dblUnderlyingPrice << " for " << 100 * m_nLongUnderlying * m_dblUnderlyingPrice
                         << ", P" << m_nLongPut        << "@" << m_dblPutPrice        << " for " << 100 * m_nLongPut * m_dblPutPrice 
                          << std::endl;
    OutputDebugString( m_ss.str().c_str() );

    COrder* pOrder;

    // orders for normal delta neutral
    pOrder = new COrder( pUnderlying->GetInstrument(), OrderType::Market, OrderSide::Sell, m_nLongUnderlying );
    m_tws.PlaceOrder( pOrder );
    pOrder = new COrder( m_iterOILatestGammaSelectPut->Put()->Symbol()->GetInstrument(), OrderType::Market, OrderSide::Sell, m_nLongPut );
    m_tws.PlaceOrder( pOrder );

    m_bTrading = false;
  }

  ptime dt;
  m_ts.External( &dt );
  if ( dt.time_of_day() >= m_dtMarketClose ) {
    m_ss.str( "" );
    m_ss << m_ts.External();
    m_ss << " State:  After Market." << std::endl;
    OutputDebugString( m_ss.str().c_str() );
    m_TradingState = ETSAfterMarket;
  }
}

void CProcess::HandleAfterMarket( const CTrade& trade ) {
}

void CProcess::PrintGreeks( void ) {
  m_ss.str( "" );
  m_ss << "Greeks: " 
    << m_ts.External()
    << " Strk "  << m_iterOILatestGammaSelectCall->Strike()
    << " Call "  << m_iterOILatestGammaSelectCall->Call()->Symbol()->OptionPrice()
    << " ImpVo " << m_iterOILatestGammaSelectCall->Call()->Symbol()->ImpliedVolatility()
    << " Delta " << m_iterOILatestGammaSelectCall->Call()->Symbol()->Delta()
    << " Gamma " << m_iterOILatestGammaSelectCall->Call()->Symbol()->Gamma() << " - "
    << " Strk "  << m_iterOILatestGammaSelectPut->Strike()
    << " Put "   << m_iterOILatestGammaSelectPut->Put()->Symbol()->OptionPrice()
    << " ImpVo " << m_iterOILatestGammaSelectPut->Put()->Symbol()->ImpliedVolatility()
    << " Delta " << m_iterOILatestGammaSelectPut->Put()->Symbol()->Delta()
    << " Gamma " << m_iterOILatestGammaSelectPut->Put()->Symbol()->Gamma() 
    << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::SaveSeries( void ) {

  m_ss.str( "" );
  m_ss << m_ts.External();

  std::string sPathName;

  CHDF5WriteTimeSeries<CQuotes, CQuote> wtsQuotes;
  CHDF5WriteTimeSeries<CTrades, CTrade> wtsTrades;
  CHDF5WriteTimeSeries<CGreeks, CGreek> wtsGreeks;

  try {
    if ( 0 != m_quotes.Size() ) {
      sPathName = m_sPathForSeries + "/" + m_ss.str() + "/quotes/" + m_sSymbolName;
      wtsQuotes.Write( sPathName, &m_quotes );
    }

    if ( 0 != m_trades.Size() ) {
      sPathName = m_sPathForSeries + "/" + m_ss.str() + "/trades/" + m_sSymbolName;
      wtsTrades.Write( sPathName, &m_trades );
    }
  }
  catch (...) {
  }

  for ( std::vector<CStrikeInfo>::iterator iter = m_iterOILowestWatch; iter != m_iterOIHighestWatch; ++iter ) {

    try {
      if ( 0 != iter->Call()->Quotes()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/quotes/" + iter->Call()->Symbol()->GetInstrument()->GetSymbolName();
        wtsQuotes.Write( sPathName, iter->Call()->Quotes() );
      }

      if ( 0 != iter->Call()->Trades()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/trades/" + iter->Call()->Symbol()->GetInstrument()->GetSymbolName();
        wtsTrades.Write( sPathName, iter->Call()->Trades() );
      }

      if ( 0 != iter->Call()->Greeks()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/greeks/" + iter->Call()->Symbol()->GetInstrument()->GetSymbolName();
        wtsGreeks.Write( sPathName, iter->Call()->Greeks() );
      }

      if ( 0 != iter->Put()->Quotes()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/quotes/" + iter->Put()->Symbol()->GetInstrument()->GetSymbolName();
        wtsQuotes.Write( sPathName, iter->Put()->Quotes() );
      }

      if ( 0 != iter->Put()->Trades()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/trades/" + iter->Put()->Symbol()->GetInstrument()->GetSymbolName();
        wtsTrades.Write( sPathName, iter->Put()->Trades() );
      }

      if ( 0 != iter->Put()->Greeks()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/greeks/" + iter->Put()->Symbol()->GetInstrument()->GetSymbolName();
        wtsGreeks.Write( sPathName, iter->Put()->Greeks() );
      }
    }
    catch (...) {
    }

  }

  m_ss << " done writing." << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

// need to worry about rogue trades... trades out side of the normal trading range
// capture quotes, trades, greeks and write to database afterwards
// handle executions
// manage multiple position, add a new position on each cross over, or when ever 
//  hightest gamma moves to another strike
// implied volatility appears to keep around for what a medium day should be, but on large 
//  moves on the underlying, will increase, and then come back to the daily mean
// on increasing implied volatility, or when it exceeds current maximums, hold on trades until new
//   maximum implied volatility is established, then sell at that point, to take 
//   advantage of increased premiums: 
//      sell high implied volatility, buy low implied volatility
// add hdf5 link cross links for different groupings


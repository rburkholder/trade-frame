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

#include <LibCommon/TimeSource.h>
#include <LibIndicators/Pivots.h>
#include <LibHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <LibHDF5TimeSeries/HDF5DataManager.h>
#include <LibHDF5TimeSeries/HDF5IterateGroups.h>

#include "Process.h"

//
// ==================
//

CNakedOption::CNakedOption( pInstrument_t pInstrument ) 
: m_pInstrument( pInstrument ),
  m_dblBid( 0 ), m_dblAsk( 0 ), m_dblTrade( 0 ),
  m_dblStrike( pInstrument->GetStrike() ),
  m_bWatching( false ),
  m_sSide( "-" )
{
}

CNakedOption::CNakedOption( const CNakedOption& rhs ) 
: m_dblBid( rhs.m_dblBid ), m_dblAsk( rhs.m_dblAsk ), m_dblTrade( rhs.m_dblTrade ),
  m_dblStrike( rhs.m_dblStrike ),
  m_greek( rhs.m_greek ),
  m_bWatching( false ),
  m_sSide( rhs.m_sSide ),
  m_pInstrument( rhs.m_pInstrument )
{
  assert( !rhs.m_bWatching );
}

CNakedOption& CNakedOption::operator=( const CNakedOption& rhs ) {
  assert( !rhs.m_bWatching );
  assert( !m_bWatching );
  m_dblStrike = rhs.m_dblStrike;
  m_greek = rhs.m_greek;
  m_sSide = rhs.m_sSide;
  m_pInstrument = rhs.m_pInstrument;
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

CNakedCall::CNakedCall( pInstrument_t pInstrument )
: CNakedOption( pInstrument )
{
  m_sSide = "C";
}

//
// ==================
//

CNakedPut::CNakedPut( pInstrument_t pInstrument )
: CNakedOption( pInstrument )
{
  m_sSide = "P";
}


//
// ==================
//

CStrikeInfo::CStrikeInfo( double dblStrike ) 
: m_dblStrike( dblStrike ),
//  m_call( NULL ), m_put( NULL ),
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
  m_bIBConnected( false ), m_bIQFeedConnected( false ), m_bSimConnected( false ),
  m_sSymbolName( "GLD" ), m_contractidUnderlying( 0 ),
  m_nCalls( 0 ), m_nPuts( 0 ), m_nLongPut( 0 ), m_nLongUnderlying( 0 ),
  m_bWatchingOptions( false ), m_bTrading( false ),
  m_dblBaseDelta( 1000.0 ), m_dblBaseDeltaIncrement( 100.0 ),
  m_TradingState( ETSFirstPass ), 
  m_dtMarketOpen( time_duration( 10, 30, 0 ) ),
  m_dtMarketOpeningOrder( time_duration( 10, 31, 0 ) ),
  m_dtMarketClosingOrder( time_duration( 16, 56, 0 ) ),
  m_dtMarketClose( time_duration( 17, 0, 0 ) ),
  m_sPathForSeries( "/strategy/deltaneutral1" ),
  m_sDesiredSimTradingDay( "2010-Jul-15 20:03:35.687500" ),
  m_bProcessSimTradingDayGroup( false ),
  //m_tws( "U215226" ), m_iqfeed()
  m_tws( new CIBTWS( "U215226" ) ), m_iqfeed( new CIQFeedProvider() ), m_sim( new CSimulationProvider() ),
  m_ProcessingState( EPSLive )
{

  m_contract.currency = "USD";
  m_contract.exchange = "SMART";
  m_contract.symbol = m_sSymbolName;
  m_contract.expiry = "20100917";

  m_pPortfolio.reset( new CPortfolio( "DeltaNeutral" ) );

  // this is where we select which provider we will be working with on this run

  switch ( m_ProcessingState ) {
    case EPSSimulation:
      m_pExecutionProvider = m_sim;
      m_pDataProvider = m_sim;
      break;
    case EPSLive:
      m_pExecutionProvider = m_tws;
      m_pDataProvider = m_tws;
      break;
  }

  m_pExecutionProvider->OnConnected.Add( MakeDelegate( this, &CProcess::HandleOnExecConnected ) );
  m_pExecutionProvider->OnDisconnected.Add( MakeDelegate( this, &CProcess::HandleOnExecDisconnected ) );

  m_pDataProvider->OnConnected.Add( MakeDelegate( this, &CProcess::HandleOnDataConnected ) );
  m_pDataProvider->OnDisconnected.Add( MakeDelegate( this, &CProcess::HandleOnDataDisconnected ) );

  m_iqfeed->OnConnected.Add( MakeDelegate( this, &CProcess::HandleOnData2Connected ) );
  m_iqfeed->OnDisconnected.Add( MakeDelegate( this, &CProcess::HandleOnData2Disconnected ) );
}

CProcess::~CProcess(void)
{
}

// Simulation Engine Connect/Disconnect
void CProcess::SimConnect( void ) {
  if ( !m_bSimConnected ) {
    
    m_sim->Connect();
    m_bSimConnected = true;

    AcquireSimulationSymbols();
  }
}

void CProcess::SimDisconnect( void ) {
  if ( m_bSimConnected ) {
    m_sim->Disconnect();
    m_bSimConnected = false;
  }
}

// Interactive Brokers Connect/Disconnect
void CProcess::IBConnect( void ) {
  if ( !m_bIBConnected ) {
    
    m_tws->Connect();
    m_bIBConnected = true;
  }
}

void CProcess::IBDisconnect( void ) {
  if ( m_bIBConnected ) {
    m_tws->Disconnect();
    m_bIBConnected = false;
  }
}

// IQFeed Connect/Disconnect
void CProcess::IQFeedConnect( void ) {
  if ( !m_bIQFeedConnected ) {
    
    m_iqfeed->Connect();
    m_bIQFeedConnected = true;
  }
}

void CProcess::IQFeedDisconnect( void ) {
  if ( m_bIQFeedConnected ) {
    m_iqfeed->Disconnect();
    m_bIQFeedConnected = false;
  }
}

void CProcess::HandleOnDataConnected(int e) {
  CIQFeedHistoryQuery<CProcess>::Connect();  
}

void CProcess::HandleOnDataDisconnected(int e) {
}

void CProcess::HandleOnData2Connected(int e) {
  CIQFeedHistoryQuery<CProcess>::Connect();  
}

void CProcess::HandleOnData2Disconnected(int e) {
}

void CProcess::OnHistoryConnected( void ) {
  CIQFeedHistoryQuery<CProcess>::RetrieveNEndOfDays( m_sSymbolName, 1 );
}

void CProcess::HandleOnExecConnected(int e) {

  // (1) create underlying symbol, then (2) get strike list
  m_vStrikes.clear(); /// horribly buggy this way.
  m_vCrossOverPoints.clear();

  switch ( m_ProcessingState ) {
    case EPSSimulation:
      break;
    case EPSLive:
      m_contract.secType = "STK";
      m_tws->SetOnContractDetailsHandler( MakeDelegate( this, &CProcess::HandleStrikeListing1 ) );
      m_tws->SetOnContractDetailsDoneHandler( MakeDelegate( this, &CProcess::HandleStrikeListing1Done ) );
      m_tws->RequestContractDetails( m_contract );
      break;
  }
}

void CProcess::HandleOnExecDisconnected(int e) {
  m_ss.str( "" );
  m_ss << "Exec disconnected." << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

// --- listing 1 -- Uhderlying Contract

void CProcess::HandleStrikeListing1( const ContractDetails& details ) {
  m_contractidUnderlying = details.summary.conId;
  try {
    m_pUnderlying = m_tws->GetSymbol( m_contractidUnderlying )->GetInstrument();
  }
  catch ( std::out_of_range& e ) {
    CIBTWS::pInstrument_t pInstrument = m_tws->BuildInstrumentFromContract( details.summary );
    m_pUnderlying = m_tws->GetSymbol( pInstrument )->GetInstrument();  // create the symbol, then get the instrument again
  }
}

void CProcess::AcquireSimulationSymbols( void ) {
  HDF5IterateGroups scan;
  scan.SetOnHandleObject( MakeDelegate( this, &CProcess::HandleHDF5Object ) );
  scan.SetOnHandleGroup( MakeDelegate( this, &CProcess::HandleHDF5Group ) );
  scan.Start( m_sPathForSeries );
}

void CProcess::HandleHDF5Object( const std::string& sPath, const std::string& sName) {

  if ( m_bProcessSimTradingDayGroup ) {
    m_ss.str( "" );
    m_ss << "Object: \"" << sPath << "\"" << std::endl;
    OutputDebugString( m_ss.str().c_str() );
  }
}

void CProcess::HandleHDF5Group( const std::string& sPath, const std::string& sName) {

  if ( 18 < sName.length() ) {
    m_bProcessSimTradingDayGroup = ( sName == m_sDesiredSimTradingDay );
  }
  
  m_ss.str( "" );
  m_ss << "Group: \"" << sPath << "\"";
  if ( m_bProcessSimTradingDayGroup ) 
    m_ss << "*";
  m_ss << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::HandleStrikeListing1Done(  ) {

  m_ss.str( "" );
  m_ss << "Uhderlying Contract Done" << std::endl;
  OutputDebugString( m_ss.str().c_str() );

  // now request contract info for strike listing 
  m_contract.secType = "OPT";
  m_contract.right = "CALL";
  m_tws->SetOnContractDetailsHandler( MakeDelegate( this, &CProcess::HandleStrikeListing2 ) );
  m_tws->SetOnContractDetailsDoneHandler( MakeDelegate( this, &CProcess::HandleStrikeListing2Done ) );
  m_tws->RequestContractDetails( m_contract );

}

// --- listing 2 -- listing of strikes, listing of calls

void CProcess::HandleStrikeListing2( const ContractDetails& details ) {

  // create strike entries
  m_vCrossOverPoints.push_back( details.summary.strike );

  // process contract as a call
  CIBSymbol::pSymbol_t pSymbol;
  CIBTWS::pInstrument_t pInstrument;
  try {
    pSymbol = m_tws->GetSymbol( details.summary.conId );
    pInstrument = pSymbol->GetInstrument();
  }
  catch ( std::out_of_range& e ) {
    pInstrument = m_tws->BuildInstrumentFromContract( details.summary );
    pSymbol = m_tws->GetSymbol( pInstrument );  // creates symbol in provider map
  }

  // create strike entry
  CStrikeInfo oi( details.summary.strike );
  m_vStrikes.push_back( oi );
  m_vStrikes.back().AssignCall( pInstrument );  // needs to come after push

}

void CProcess::HandleStrikeListing2Done(  ) {

  // strike listing is complete
  m_ss.str( "" );
  m_ss << "#strikes: " << m_vStrikes.size() << std::endl;
  OutputDebugString( m_ss.str().c_str() );

  // request puts
  m_iterStrikes = m_vStrikes.begin();

  if ( m_vStrikes.end() != m_iterStrikes ) {
    m_contract.strike = m_iterStrikes->Strike();
    m_contract.right = "PUT";

    m_tws->SetOnContractDetailsHandler( MakeDelegate( this, &CProcess::HandleStrikeListing3 ) );
    m_tws->SetOnContractDetailsDoneHandler( MakeDelegate( this, &CProcess::HandleStrikeListing3Done ) );
    m_tws->RequestContractDetails( m_contract );
  }
}

// --- listing 3 -- Put Contracts

void CProcess::HandleStrikeListing3( const ContractDetails& details ) {
  CIBSymbol::pSymbol_t pSymbol;
  CIBTWS::pInstrument_t pInstrument;
  try {
    pSymbol = m_tws->GetSymbol( details.summary.conId );
    pInstrument = pSymbol->GetInstrument();
  }
  catch ( std::out_of_range& e ) {
    pInstrument = m_tws->BuildInstrumentFromContract( details.summary );
    pSymbol = m_tws->GetSymbol( pInstrument );  // creates symbol in provider map
  }
  m_iterStrikes->AssignPut( pInstrument );

  ++m_iterStrikes;
  if ( m_vStrikes.end() != m_iterStrikes ) {
    m_contract.strike = m_iterStrikes->Strike();
    m_tws->RequestContractDetails( m_contract );
  }
  else {
    // all done
    m_ss.str( "" );
    m_ss << "Option Acquisition Complete" << std::endl;
    OutputDebugString( m_ss.str().c_str() );
  }
}

void CProcess::HandleStrikeListing3Done(  ) {
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

void CProcess::StartWatch( void ) {

  std::sort( m_vStrikes.begin(), m_vStrikes.end() );
  std::sort( m_vCrossOverPoints.begin(), m_vCrossOverPoints.end() );

  m_iterOILatestGammaSelectCall = m_vStrikes.end();  // initialized for beginning of trading
  m_iterOILatestGammaSelectPut = m_vStrikes.end();  // initialized for beginning of trading

  m_pDataProvider->AddQuoteHandler( m_pUnderlying, MakeDelegate( this, &CProcess::HandleUnderlyingQuote ) );
  m_pDataProvider->AddTradeHandler( m_pUnderlying, MakeDelegate( this, &CProcess::HandleUnderlyingTrade ) );

}

void CProcess::StopWatch( void ) {

  m_pDataProvider->RemoveQuoteHandler( m_pUnderlying, MakeDelegate( this, &CProcess::HandleUnderlyingQuote ) );
  m_pDataProvider->RemoveTradeHandler( m_pUnderlying, MakeDelegate( this, &CProcess::HandleUnderlyingTrade ) );

  if ( m_bWatchingOptions ) {
    m_bWatchingOptions = false;
    for ( std::vector<CStrikeInfo>::iterator iter = m_iterOILowestWatch; iter != m_iterOIHighestWatch; ++iter ) {

      m_pDataProvider->RemoveQuoteHandler( iter->Call()->GetInstrument(), MakeDelegate( iter->Call(), &CNakedCall::HandleQuote ) );
      m_pDataProvider->RemoveTradeHandler( iter->Call()->GetInstrument(), MakeDelegate( iter->Call(), &CNakedCall::HandleTrade ) );
      m_pDataProvider->RemoveGreekHandler( iter->Call()->GetInstrument(), MakeDelegate( iter->Call(), &CNakedCall::HandleGreek ) );

      m_pDataProvider->RemoveQuoteHandler( iter->Put()->GetInstrument(),  MakeDelegate( iter->Put(),  &CNakedPut::HandleQuote ) );
      m_pDataProvider->RemoveTradeHandler( iter->Put()->GetInstrument(),  MakeDelegate( iter->Put(),  &CNakedPut::HandleTrade ) );
      m_pDataProvider->RemoveGreekHandler( iter->Put()->GetInstrument(),  MakeDelegate( iter->Put(),  &CNakedPut::HandleGreek ) );
    }
  }
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
      gamma = iter->Call()->Gamma();
      if ( gammaCall < gamma ) { 
        gammaCall = gamma;
        m_iterOILatestGammaSelectCall = iter;
      }
      gamma = iter->Put()->Gamma();
      if ( gammaPut < gamma ) { 
        gammaPut = gamma;
        m_iterOILatestGammaSelectPut = iter;
      }
    }

    // todo?  check that SelectCall != LowestWatch and SelectPut != HighestWatch

    m_dblCallPrice = m_iterOILatestGammaSelectCall->Call()->Ask();
    m_dblPutPrice = m_iterOILatestGammaSelectPut->Put()->Ask();

    // depending upon gammas, could be straddle or strangle?
    m_nCalls = (int) floor(      ( m_dblBaseDelta / m_iterOILatestGammaSelectCall->Call()->Delta() ) / 100 );
    m_nPuts  = (int) floor( -1 * ( m_dblBaseDelta / m_iterOILatestGammaSelectPut ->Put() ->Delta() ) / 100 );

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
      // orders for normal delta neutral

      m_posUnderlying.reset( new CPosition( m_pUnderlying, m_pExecutionProvider, m_pDataProvider, "Underlying" ) );
      m_posUnderlying->PlaceOrder( OrderType::Market, OrderSide::Buy, m_nLongUnderlying );
      m_pPortfolio->AddPosition( "Underlying", m_posUnderlying );

      m_posPut.reset( new CPosition( m_iterOILatestGammaSelectPut->Put()->GetInstrument(), m_pExecutionProvider, m_pDataProvider, "Put" ) );
      m_posPut->PlaceOrder( OrderType::Market, OrderSide::Buy, m_nLongPut );
      m_pPortfolio->AddPosition( "Put", m_posPut );

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
  m_ss << CTimeSource::Instance().External();
  m_ss << " State:  First Pass -> Pre Market." << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::HandleTSPreMarket( const CTrade& trade ) {
  ptime dt;
  CTimeSource::Instance().External( &dt );
  if ( dt.time_of_day() >= m_dtMarketOpen ) {
    m_ss.str( "" );
    m_ss << dt;
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
  m_ss << CTimeSource::Instance().External();
  m_ss << " Trade 1: " << trade.Volume() << "@" << trade.Trade() << std::endl;
  OutputDebugString( m_ss.str().c_str() );

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

    m_pDataProvider->AddQuoteHandler( iter->Call()->GetInstrument(), MakeDelegate( iter->Call(), &CNakedCall::HandleQuote ) );
    m_pDataProvider->AddTradeHandler( iter->Call()->GetInstrument(), MakeDelegate( iter->Call(), &CNakedCall::HandleTrade ) );
    m_pDataProvider->AddGreekHandler( iter->Call()->GetInstrument(), MakeDelegate( iter->Call(), &CNakedCall::HandleGreek ) );

    m_pDataProvider->AddQuoteHandler( iter->Put()->GetInstrument(),  MakeDelegate( iter->Put(),  &CNakedPut::HandleQuote ) );
    m_pDataProvider->AddTradeHandler( iter->Put()->GetInstrument(),  MakeDelegate( iter->Put(),  &CNakedPut::HandleTrade ) );
    m_pDataProvider->AddGreekHandler( iter->Put()->GetInstrument(),  MakeDelegate( iter->Put(),  &CNakedPut::HandleGreek ) );
  }

  m_TradingState = ETSFirstTrade;
}

void CProcess::HandleTSOpeningOrder( const CTrade& trade ) {

  ptime dt;
  CTimeSource::Instance().External( &dt );
  if ( dt.time_of_day() >= m_dtMarketOpeningOrder ) {
    m_ss.str( "" );
    m_ss << CTimeSource::Instance().External();
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
  CTimeSource::Instance().External( &dt );
  if ( dt.time_of_day() >= m_dtMarketClosingOrder ) {
    m_ss.str( "" );
    m_ss << dt;
    m_ss << " State:  Close Orders." << std::endl;
    OutputDebugString( m_ss.str().c_str() );

    m_TradingState = ETSCloseOrders;
  }
  else {

    double dblDeltaPut  = m_iterOILatestGammaSelectPut->Put()->Delta() * m_nPuts * 100;
    double dblDeltaCall = m_iterOILatestGammaSelectCall->Call()->Delta() * m_nCalls * 100;

    bool bTraded = false;
    int nOptions = 0;
    //COrder::pOrder_t pOrder;

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

    // orders for normal delta neutral
//    m_posUnderlying->CancelOrders();
//    m_posUnderlying->ClosePosition();
//    m_posPut->CancelOrders();
//    m_posPut->ClosePosition();

    m_bTrading = false;
  }

  ptime dt;
  CTimeSource::Instance().External( &dt );
  if ( dt.time_of_day() >= m_dtMarketClose ) {
    m_ss.str( "" );
    m_ss << dt;
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
    << CTimeSource::Instance().External()
    << " Strk "  << m_iterOILatestGammaSelectCall->Strike()
//    << " Call "  << m_iterOILatestGammaSelectCall->Call()->OptionPrice()
    << " ImpVo " << m_iterOILatestGammaSelectCall->Call()->ImpliedVolatility()
    << " Delta " << m_iterOILatestGammaSelectCall->Call()->Delta()
    << " Gamma " << m_iterOILatestGammaSelectCall->Call()->Gamma() << " - "
    << " Strk "  << m_iterOILatestGammaSelectPut->Strike()
//    << " Put "   << m_iterOILatestGammaSelectPut->Put()->OptionPrice()
    << " ImpVo " << m_iterOILatestGammaSelectPut->Put()->ImpliedVolatility()
    << " Delta " << m_iterOILatestGammaSelectPut->Put()->Delta()
    << " Gamma " << m_iterOILatestGammaSelectPut->Put()->Gamma() 
    << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::SaveSeries( void ) {

  m_ss.str( "" );
  m_ss << CTimeSource::Instance().External();

  if ( CProviderInterfaceBase::EProviderSimulator == m_pDataProvider->ID() ) {
    m_ss << " simulator stores nothing." << std::endl;
    OutputDebugString( m_ss.str().c_str() );
    return;
  }

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
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/quotes/" + iter->Call()->GetInstrument()->GetInstrumentName();
        wtsQuotes.Write( sPathName, iter->Call()->Quotes() );
      }

      if ( 0 != iter->Call()->Trades()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/trades/" + iter->Call()->GetInstrument()->GetInstrumentName();
        wtsTrades.Write( sPathName, iter->Call()->Trades() );
      }

      if ( 0 != iter->Call()->Greeks()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/greeks/" + iter->Call()->GetInstrument()->GetInstrumentName();
        wtsGreeks.Write( sPathName, iter->Call()->Greeks() );
      }

      if ( 0 != iter->Put()->Quotes()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/quotes/" + iter->Put()->GetInstrument()->GetInstrumentName();
        wtsQuotes.Write( sPathName, iter->Put()->Quotes() );
      }

      if ( 0 != iter->Put()->Trades()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/trades/" + iter->Put()->GetInstrument()->GetInstrumentName();
        wtsTrades.Write( sPathName, iter->Put()->Trades() );
      }

      if ( 0 != iter->Put()->Greeks()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/greeks/" + iter->Put()->GetInstrument()->GetInstrumentName();
        wtsGreeks.Write( sPathName, iter->Put()->Greeks() );
      }
    }
    catch (...) {
    }

  }

  m_ss << " done writing." << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::EmitStats( void ) {
  m_ss.str( "" );
  m_ss << CTimeSource::Instance().External();
  m_ss << ": ";
  m_pPortfolio->EmitStats( m_ss );
  m_ss << std::endl;
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


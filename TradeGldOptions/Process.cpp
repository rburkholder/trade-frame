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

#include <boost/lexical_cast.hpp>

#include <OUCommon/TimeSource.h>

#include <TFIndicators/Pivots.h>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>

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
  // assert instrument is a call
  m_sSide = "C";
}

//
// ==================
//

CNakedPut::CNakedPut( pInstrument_t pInstrument )
: CNakedOption( pInstrument )
{
  // assert instrument is a put
  m_sSide = "P";
}


//
// ==================
//

CStrikeInfo::CStrikeInfo( void ) 
: m_dblStrike( 0 ),
//  m_call( NULL ), m_put( NULL ),
  m_bWatching( false )
{
  //assert( false );  // see if it actaully gets called
}

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

// 2011/03/06
// when day to day trading commences, will need to automatically calculate days to expiry
//   and exit current expiry and start new expiry when within 30 days of expiry.

// 2011/03/08
/*
Buy on the way down
Sell only when above average price
Work with 2000 delta to start
Below average price, buy anytime on delta change, except on high end
implied volatility
Above average price, sell on delta change
At strike: vertical spread into new put (don't do anything, change strike
when changing calendar)
 * on way down, keep current delta (no)
 * on way up, normalize delta: (buy puts, buy underlying) (no)
Vertical spread when within n days of expiry - maybe not, see above
On delta change for upside sell, if current multiple doesn't work, wait
till next multiple to ensure a ways above average price
*/

// gamma scalping:  http://www.schwab.com/public/schwab/research_strategies/market_insight/investing_strategies/options/gamma_scalping.html
// in addition to staying delta neutral, watch the gamma, it may indicate when to reverse the position.
// http://www.discoveroptions.com/mixed/content/education/articles/longsyntheticstraddle.html

// Essentially, when my delta equaled my gamma, I would flatten my deltas. 
// http://www.optionpit.com/option-education/gamma-scalping-part-two-deltagamma-ratio-hedging

// The key to a good gamma position is that the option trader wants the underlying to have a great realized volatility higher than the implied volatility the trade bought.  
// http://www.optionpit.com/blog/setting-greeks-long-gamma-or-gamma-scalp-trade
// http://www.optionpit.com/option-education/gamma-scalping-part-one-pay-the-day   ********

// ***** At day’s end, I always zero out deltas. 

#define testing

CProcess::CProcess(void)
:
  m_bIBConnected( false ), m_bIQFeedConnected( false ), m_bSimConnected( false ),
  m_contractidUnderlying( 0 ),
  m_bWatchingOptions( false ), m_bTrading( false ),
  m_dblBaseDelta( 2000.0 ), m_dblBaseDeltaIncrement( 100.0 ),
  m_TradingState( ETSFirstPass ), 
#ifdef testing
  m_sSymbolName( "DELL" ), 
  m_dtMarketOpen( time_duration( 0, 30, 0 ) ),
  m_dtMarketOpeningOrder( time_duration( 0, 31, 0 ) ),
  m_dtMarketClosingOrder( time_duration( 23, 56, 0 ) ),
  m_dtMarketClose( time_duration( 23, 59, 59 ) ),
#else
  m_sSymbolName( "GLD" ), 
  m_dtMarketOpen( time_duration( 10, 30, 0 ) ),
  m_dtMarketOpeningOrder( time_duration( 10, 30, 20 ) ),
  m_dtMarketClosingOrder( time_duration( 16, 56, 0 ) ),
  m_dtMarketClose( time_duration( 17, 0, 0 ) ),
#endif
  m_sPathForSeries( "/strategy/deltaneutral2" ),
  m_sDesiredSimTradingDay( "2010-Sep-10 20:10:25.562500" ),
  m_bProcessSimTradingDayGroup( false ),
  m_tws( new CIBTWS( "U215226" ) ), m_iqfeed( new CIQFeedProvider() ), m_sim( new CSimulationProvider() ),
  m_bWaitingForTradeCompletion( false ), m_dblDeltaTotalPut( 0 ), m_dblDeltaTotalUnderlying( 0 ),
  m_eMode( EModeLive ),
  //m_eMode( EModeSimulation )
  m_bExecConnected( false ), m_bDataConnected( false ), m_bData2Connected( false ), m_bConnectDone( false )
{

  boost::gregorian::date dToday = ou::CTimeSource::Instance().Internal().date();
  m_dExpiry = ou::tf::options::Next3rdFriday( dToday );
  while ( boost::gregorian::date_period( dToday, m_dExpiry ).length().days() < 8 ) { // some say use front month for scalping
    boost::gregorian::date_duration dd( 1 );
    boost::gregorian::date d = m_dExpiry + dd;
    m_dExpiry = ou::tf::options::Next3rdFriday( d );
  }

  m_contract.currency = "USD";
  m_contract.exchange = "SMART";
  m_contract.symbol = m_sSymbolName;

  // will need to use the expiry date to look at existing positions to see if a calendar spread is needed for rolling over

  m_contract.expiry = boost::gregorian::to_iso_string( m_dExpiry );

  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &CProcess::HandlePopulateDatabase ) );

  m_idPortfolio = "dn01-" + m_sSymbolName + "-" + m_contract.expiry;  // needs to come before database open

  // Implement the calendar roll over at some point

  // this is where we select which provider we will be working with on this run
  // providers need to be registered in order for portfolio/position loading to function properly
  // key needs to match to account
  CProviderManager::Instance().Register( "ib01", static_cast<pProvider_t>( m_tws ) );
  CProviderManager::Instance().Register( "iq01", static_cast<pProvider_t>( m_iqfeed ) );
  CProviderManager::Instance().Register( "sim01", static_cast<pProvider_t>( m_sim ) );

  std::string sDbName;

  switch ( m_eMode ) {
    case EModeSimulation:
      sDbName = ":memory:";
      m_pExecutionProvider = m_sim;
      m_pDataProvider = m_sim;
      break;
    case EModeLive:
      sDbName = "TradeGldOptions.db";
      m_pExecutionProvider = m_tws;
      m_pDataProvider = m_tws;
      m_db.Open( sDbName );
      break;
  }

  m_pExecutionProvider->OnConnected.Add( MakeDelegate( this, &CProcess::HandleOnExecConnected ) );
  m_pExecutionProvider->OnDisconnected.Add( MakeDelegate( this, &CProcess::HandleOnExecDisconnected ) );

  m_pDataProvider->OnConnected.Add( MakeDelegate( this, &CProcess::HandleOnDataConnected ) );
  m_pDataProvider->OnDisconnected.Add( MakeDelegate( this, &CProcess::HandleOnDataDisconnected ) );

  m_iqfeed->OnConnected.Add( MakeDelegate( this, &CProcess::HandleOnData2Connected ) );
  m_iqfeed->OnDisconnected.Add( MakeDelegate( this, &CProcess::HandleOnData2Disconnected ) );
}

CProcess::~CProcess(void) {

  m_posUnderlying.reset();
  m_posPut.reset();

  m_pPortfolio.reset();

  m_pExecutionProvider->OnConnected.Remove( MakeDelegate( this, &CProcess::HandleOnExecConnected ) );
  m_pExecutionProvider->OnDisconnected.Remove( MakeDelegate( this, &CProcess::HandleOnExecDisconnected ) );

  m_pDataProvider->OnConnected.Remove( MakeDelegate( this, &CProcess::HandleOnDataConnected ) );
  m_pDataProvider->OnDisconnected.Remove( MakeDelegate( this, &CProcess::HandleOnDataDisconnected ) );

  m_iqfeed->OnConnected.Remove( MakeDelegate( this, &CProcess::HandleOnData2Connected ) );
  m_iqfeed->OnDisconnected.Remove( MakeDelegate( this, &CProcess::HandleOnData2Disconnected ) );

  m_db.Close();
}

void CProcess::SetMode( enumMode mode ) {
}

void CProcess::SetDataConnection( enumDataConnection dc ) {
}

// Simulation Engine Connect/Disconnect
void CProcess::SimConnect( void ) {
  if ( !m_bSimConnected ) {
    
    m_sim->Connect();
    m_bSimConnected = true;

    AcquireSimulationSymbols();
  }
}

void CProcess::SimStart( void ) {
  m_sim->Run();
}

void CProcess::SimStop( void ) {
  m_sim->Stop();
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
  m_bDataConnected = true;
  HandleOnConnected(e);
//  CIQFeedHistoryQuery<CProcess>::Connect();  
}

void CProcess::HandleOnDataDisconnected(int e) {
  m_bDataConnected = false;
  HandleOnConnected(e);
}

void CProcess::HandleOnData2Connected(int e) {
  CIQFeedHistoryQuery<CProcess>::Connect();  
  m_bData2Connected = true;
  HandleOnConnected( e );
}

void CProcess::HandleOnData2Disconnected(int e) {
  m_bData2Connected = false;
  HandleOnConnected( e );
}

void CProcess::OnHistoryConnected( void ) {
  CIQFeedHistoryQuery<CProcess>::RetrieveNEndOfDays( m_sSymbolName, 1 );
}

void CProcess::HandleOnExecConnected(int e) {

  // (1) create underlying symbol, then (2) get strike list
  m_mapStrikeInfo.clear(); /// horribly buggy this way.
  m_vCrossOverPoints.clear();

  switch ( m_eMode ) {
    case EModeSimulation:
      break;
    case EModeLive:
      // try load from database first
      try {
        m_pUnderlying = ou::tf::CInstrumentManager::Instance().Get( m_sSymbolName );
        // need to also prime TWS with symbol
        m_tws->GetSymbol( m_pUnderlying );  // preload symbol
        HandleUnderlyingListingDone();
      }
      catch (...) {
        // otherwise request the contract information
        m_contract.secType = "STK";
        m_tws->SetOnContractDetailsHandler( MakeDelegate( this, &CProcess::HandleUnderlyingListing ) );
        m_tws->SetOnContractDetailsDoneHandler( MakeDelegate( this, &CProcess::HandleUnderlyingListingDone ) );
        m_tws->RequestContractDetails( m_contract );
      }
      break;
  }

  m_bExecConnected = true;
  HandleOnConnected( e );
}

void CProcess::HandleOnExecDisconnected(int e) {
  m_bExecConnected = false;
  HandleOnConnected(e);
  m_ss.str( "" );
  m_ss << "Exec disconnected." << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::HandleOnConnected( int e ) {

  if ( m_bConnectDone ) {
  }
  else {
    if ( m_bExecConnected && m_bDataConnected ) {
      m_bConnectDone = true;

      try {
        m_pPortfolio = CPortfolioManager::Instance().GetPortfolio( m_idPortfolio );

        // need to load the positions: underlying plus covering option  long+put or short+call\
        // if positions are coming from the database, any other stuff to link up?  take a look at Opening Order
        // need to assume orders are closed and fully executed

        // note that position loading have call backs:   instrument, execution, data
        // note: providers need to be in the provider map, so need to be constructed from or registered with the ProviderManager
        int nPositionsOpened( 0 );
        try {
          m_posUnderlying = CPortfolioManager::Instance().GetPosition( m_idPortfolio, "U" ); // underlying
          ++nPositionsOpened;
        }
        catch (...) {
        }

        try {
          m_posPut = CPortfolioManager::Instance().GetPosition( m_idPortfolio, "O" );  // option
          ++nPositionsOpened;
        }
        catch (...) {
        }

        if ( ( 0 != nPositionsOpened ) && ( 2 != nPositionsOpened ) ) {
          throw std::runtime_error( "wrong number of positions available" );
        }

      }
      catch (...) {
      } // catch
    }
  }
}

void CProcess::AcquireSimulationSymbols( void ) {
  HDF5IterateGroups scan;
  scan.SetOnHandleObject( MakeDelegate( this, &CProcess::HandleHDF5Object ) );
  scan.SetOnHandleGroup( MakeDelegate( this, &CProcess::HandleHDF5Group ) );
  scan.Start( m_sPathForSeries );

  if ( 0 != m_mapStrikeInfo.size() ) 
    throw std::runtime_error( "CProcess::AcquireSimulationSymbols strikes already set" );
  for ( strikes_iterator_t iter = m_mapStrikes.begin(); iter != m_mapStrikes.end(); ++ iter ) {

    CStrikeInfo oi( iter->first );
    m_mapStrikeInfo[ iter->first ] = oi;
    CStrikeInfo& poi = m_mapStrikeInfo.find( iter->first )->second;

    poi.AssignCall( iter->second.first );
    // change simulator sometime to accept these mid run
    m_sim->AddQuoteHandler( iter->second.first, MakeDelegate( poi.Call(), &CNakedCall::HandleQuote ) );
    m_sim->AddTradeHandler( iter->second.first, MakeDelegate( poi.Call(), &CNakedCall::HandleTrade ) );
    m_sim->AddGreekHandler( iter->second.first, MakeDelegate( poi.Call(), &CNakedCall::HandleGreek ) );

    poi.AssignPut( iter->second.second );
    // change simulator sometime to accept these mid run
    m_sim->AddQuoteHandler( iter->second.second, MakeDelegate( poi.Put(), &CNakedPut::HandleQuote ) );
    m_sim->AddTradeHandler( iter->second.second, MakeDelegate( poi.Put(), &CNakedPut::HandleTrade ) );
    m_sim->AddGreekHandler( iter->second.second, MakeDelegate( poi.Put(), &CNakedPut::HandleGreek ) );

    m_vCrossOverPoints.push_back( iter->first );
  }

  m_ss.str( "" );
  m_ss << "Simulation Symbols Acquired." << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::HandleHDF5Object( const std::string& sPath, const std::string& sName) {

  if ( m_bProcessSimTradingDayGroup ) {
    std::string underlying;
    unsigned short yy;
    unsigned short mm;
    unsigned short dd;
    char side;
    double strike;
    
    m_ss.str( "" );
    m_ss << "Object: \"" << sPath << "\"" << std::endl;
    OutputDebugString( m_ss.str().c_str() );
    if ( 6 >= sName.size() ) {  // process as stock
      if ( !CInstrumentManager::Instance().Exists( sName ) ) {
        m_pUnderlying = CInstrumentManager::Instance().ConstructInstrument( sName, "Sim", InstrumentType::Stock );
        m_sim->Add( m_pUnderlying );
      }
    }
    else { // process as option
      underlying = sName.substr(0,sName.find(' '));
      yy = boost::lexical_cast<unsigned short>( sName.substr(6,2) );
      yy += 2000;
      mm = boost::lexical_cast<unsigned short>( sName.substr(8,2) );
      dd = boost::lexical_cast<unsigned short>( sName.substr(10,2) );
      side = sName[12];
      strike = boost::lexical_cast<double>( sName.substr(13,8) ) / 1000.0;

      if ( !CInstrumentManager::Instance().Exists( sName ) ) {
        if ( !CInstrumentManager::Instance().Exists( underlying ) ) {
          m_pUnderlying = CInstrumentManager::Instance().ConstructInstrument( underlying, "Sim", InstrumentType::Stock );
          m_sim->Add( m_pUnderlying );
        }
        pInstrument_t instCall;
        pInstrument_t instPut;
        strikes_iterator_t iter = m_mapStrikes.find( strike );
        if ( m_mapStrikes.end() == iter ) {
          m_mapStrikes.insert( std::pair<double,option_pair_t>( strike, option_pair_t( instCall, instPut ) ) );
          iter = m_mapStrikes.find( strike );
        }
        switch ( side ) {
        case 'C':
          instCall = CInstrumentManager::Instance().ConstructOption( 
            sName, "Sim", yy, mm, dd, m_pUnderlying, static_cast<OptionSide::enumOptionSide>( side ), strike );
          iter->second.first = instCall;
          m_sim->Add( instCall );
          break;
        case 'P':
          instPut = CInstrumentManager::Instance().ConstructOption( 
            sName, "Sim", yy, mm, dd, m_pUnderlying, static_cast<OptionSide::enumOptionSide>( side ), strike );
          iter->second.second = instPut;
          m_sim->Add( instPut );
          break;
        }
      }
    }
  }
}

void CProcess::HandleHDF5Group( const std::string& sPath, const std::string& sName) {
  
  if ( 18 < sName.length() ) {
    m_sim->SetGroupDirectory( sPath );   // may need to do this conditionally on the following flag
    m_bProcessSimTradingDayGroup = ( sName == m_sDesiredSimTradingDay );
  }
  
  m_ss.str( "" );
  m_ss << "Group:  \"" << sPath << "\"";
  if ( m_bProcessSimTradingDayGroup ) 
    m_ss << "*";
  m_ss << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

// --- listing 1 -- Underlying Contract

void CProcess::HandleUnderlyingListing( const ContractDetails& details ) {
  m_contractidUnderlying = details.summary.conId;
  try {
    m_pUnderlying = m_tws->GetSymbol( m_contractidUnderlying )->GetInstrument();
  }
  catch ( std::out_of_range& e ) {
//    CIBTWS::pInstrument_t pInstrument = m_tws->BuildInstrumentFromContract( details.summary );
//    m_pUnderlying = m_tws->GetSymbol( pInstrument )->GetInstrument();  // create the symbol, then get the instrument again
  }
  // need to check if it exists first, might already be in database
  ou::tf::CInstrumentManager::Instance().Construct( m_pUnderlying );
}

void CProcess::HandleUnderlyingListingDone(  ) {

  m_ss.str( "" );
  m_ss << "Underlying Contract Done" << std::endl;
  OutputDebugString( m_ss.str().c_str() );

  m_db.SetOnNewInstrumentHandler( MakeDelegate( this, &CProcess::HandleStrikeFromDb ) );
  if ( m_db.LoadOptions( m_pUnderlying, m_dExpiry.year(), m_dExpiry.month(), m_dExpiry.day() ) ) {
    // options have been loaded through HandleStrikeFromDb
    HandleStrikeListingDone();
  }
  else {
    // request contract info for strike listing 
    m_contract.secType = "OPT";
    //m_contract.right = "CALL";  // get all calls and puts together
    m_tws->SetOnContractDetailsHandler( MakeDelegate( this, &CProcess::HandleStrikeFromIB ) );
    m_tws->SetOnContractDetailsDoneHandler( MakeDelegate( this, &CProcess::HandleStrikeListingDone ) );
    m_tws->RequestContractDetails( m_contract );
  }
  m_db.SetOnNewInstrumentHandler( 0 );
  
}

// --- listing of strikes, listing of calls

void CProcess::HandleStrikeFromIB( const ContractDetails& details ) {

  // create strike entries
  CIBSymbol::pSymbol_t pSymbol;
  CIBTWS::pInstrument_t pInstrument;
  try {
    pSymbol = m_tws->GetSymbol( details.summary.conId );
    pInstrument = pSymbol->GetInstrument();
    assert( ou::tf::InstrumentType::Option == pInstrument->GetInstrumentType() );
  }
  catch ( std::out_of_range& e ) {  // has been built in to TWS now
//    pInstrument = m_tws->BuildInstrumentFromContract( details.summary );
//    pSymbol = m_tws->GetSymbol( pInstrument );  // creates symbol in provider map
  }

  ou::tf::CInstrumentManager::Instance().Construct( pInstrument );
  AddOptionToStrikeInfo( pInstrument );

}

void CProcess::HandleStrikeFromDb( ou::tf::CInstrument::pInstrument_t pInstrument ) {
  m_tws->GetSymbol( pInstrument );  // preload symbol
  AddOptionToStrikeInfo( pInstrument );
}

void CProcess::AddOptionToStrikeInfo( ou::tf::CInstrument::pInstrument_t pInstrument ) {

  double dblStrike = pInstrument->GetStrike();

  mapStrikeInfo_iter_t iter = m_mapStrikeInfo.find( dblStrike );
  if ( m_mapStrikeInfo.end() == iter ) {
    CStrikeInfo oi( dblStrike );
    m_mapStrikeInfo[ dblStrike ] = oi;
    m_vCrossOverPoints.push_back( dblStrike );
  }
  
  switch ( pInstrument->GetOptionSide() ) {
    case ou::tf::OptionSide::Call:
      m_mapStrikeInfo[ dblStrike ].AssignCall( pInstrument );
      break;
    case ou::tf::OptionSide::Put:
      m_mapStrikeInfo[ dblStrike ].AssignPut( pInstrument );
      break;
  }

  m_ss.str("");
  m_ss << "Option " << pInstrument->GetInstrumentName() << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::HandleStrikeListingDone(  ) {

  std::sort( m_vCrossOverPoints.begin(), m_vCrossOverPoints.end() );

  // strike listing is complete
  m_ss.str( "" );
  m_ss << "#strikes: " << m_mapStrikeInfo.size() << std::endl;
  OutputDebugString( m_ss.str().c_str() );

  // all done
  m_ss.str( "" );
  m_ss << "Option Acquisition Complete" << std::endl;
  OutputDebugString( m_ss.str().c_str() );

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

//  std::sort( m_vStrikes.begin(), m_vStrikes.end() );
  std::sort( m_vCrossOverPoints.begin(), m_vCrossOverPoints.end() );

  m_iterOILatestGammaSelectCall = m_mapStrikeInfo.end();  // initialized for beginning of trading
  m_iterOILatestGammaSelectPut = m_mapStrikeInfo.end();  // initialized for beginning of trading

  m_pDataProvider->AddQuoteHandler( m_pUnderlying, MakeDelegate( this, &CProcess::HandleUnderlyingQuote ) );
  m_pDataProvider->AddTradeHandler( m_pUnderlying, MakeDelegate( this, &CProcess::HandleUnderlyingTrade ) );

}

void CProcess::StopWatch( void ) {

  m_pDataProvider->RemoveQuoteHandler( m_pUnderlying, MakeDelegate( this, &CProcess::HandleUnderlyingQuote ) );
  m_pDataProvider->RemoveTradeHandler( m_pUnderlying, MakeDelegate( this, &CProcess::HandleUnderlyingTrade ) );

  if ( m_bWatchingOptions ) {
    m_bWatchingOptions = false;
    for ( mapStrikeInfo_iter_t iter = m_iterOILowestWatch; iter != m_iterOIHighestWatch; ++iter ) {

      CStrikeInfo& oi = iter->second;

      m_pDataProvider->RemoveQuoteHandler( oi.Call()->GetInstrument(), MakeDelegate( oi.Call(), &CNakedCall::HandleQuote ) );
      m_pDataProvider->RemoveTradeHandler( oi.Call()->GetInstrument(), MakeDelegate( oi.Call(), &CNakedCall::HandleTrade ) );
      m_pDataProvider->RemoveGreekHandler( oi.Call()->GetInstrument(), MakeDelegate( oi.Call(), &CNakedCall::HandleGreek ) );

      m_pDataProvider->RemoveQuoteHandler( oi.Put()->GetInstrument(),  MakeDelegate( oi.Put(),  &CNakedPut::HandleQuote ) );
      m_pDataProvider->RemoveTradeHandler( oi.Put()->GetInstrument(),  MakeDelegate( oi.Put(),  &CNakedPut::HandleTrade ) );
      m_pDataProvider->RemoveGreekHandler( oi.Put()->GetInstrument(),  MakeDelegate( oi.Put(),  &CNakedPut::HandleGreek ) );
    }
  }
}

void CProcess::OpenPosition( void ) {

  // assert( !m_bTrading );

  int nPuts;
  int nLong;

  // if no position, create a zero delta position.
//  if ( ( 0 == m_nCalls ) && ( 0 == m_nPuts ) ) {
  if ( 0 == m_posPut->GetRow().nPositionActive ) {

    double gammaCall = 0;
    double gammaPut = 0;
    double gamma;

    m_iterOILatestGammaSelectCall = m_iterOILowestWatch;
    m_iterOILatestGammaSelectPut = m_iterOIHighestWatch;
    // find highest gamma option for call and for put
    for ( mapStrikeInfo_iter_t iter = m_iterOILowestWatch; iter != m_iterOIHighestWatch; ++iter ) {
      gamma = iter->second.Call()->Gamma();
      if ( gammaCall < gamma ) { 
        gammaCall = gamma;
        m_iterOILatestGammaSelectCall = iter;
      }
      gamma = iter->second.Put()->Gamma();
      if ( gammaPut < gamma ) { 
        gammaPut = gamma;
        m_iterOILatestGammaSelectPut = iter;
      }
    }

    // todo?  check that SelectCall != LowestWatch and SelectPut != HighestWatch

    m_dblCallPrice = m_iterOILatestGammaSelectCall->second.Call()->Ask();
    m_dblPutPrice = m_iterOILatestGammaSelectPut->second.Put()->Ask();

    // depending upon gammas, could be straddle or strangle?
//    m_nCalls = (int) floor(      ( m_dblBaseDelta / m_iterOILatestGammaSelectCall->second.Call()->Delta() ) / 100 );
    nPuts  = (int) floor( -1 * ( m_dblBaseDelta / m_iterOILatestGammaSelectPut ->second.Put() ->Delta() ) / 100 );

    // a normal delta neutral with long underlying and long put
    nLong = m_dblBaseDelta;

    // generate orders
//    if ( ( 0 == m_nCalls ) || ( 0 == m_nPuts ) ) {
    if ( 0 == nPuts ) {
      // don't buy anything if either side is zero
      m_ss.str( "" );
      m_ss << "puts or calls are zero" << std::endl;
      OutputDebugString( m_ss.str().c_str() );
//      m_nCalls = m_nPuts = 0;
    }
    else {

//      if ( ( 0 == m_posPut->GetRow().nPositionActive ) && ( 0 == m_posUnderlying->GetRow().nPositionActive ) ) {
      if ( (  0 == m_posPut ) || ( 0 == m_posUnderlying ) ) {  // no positions created, so create positions

        // need more finesse:  create positions if necessary, 
        // then check positions, if both are 0, then create orders

        try {
          // orders for normal delta neutral
          m_posUnderlying = CPortfolioManager::Instance().ConstructPosition( m_idPortfolio, "U", "same", "ib01", "ib01", m_pExecutionProvider, m_pDataProvider, m_pUnderlying );
          m_posUnderlying->OnExecution.Add( MakeDelegate( this, &CProcess::HandlePositionExecution ) );
          m_posUnderlying->PlaceOrder( OrderType::Market, OrderSide::Buy, nLong );
          m_dblDeltaTotalUnderlying = nLong;

          m_bWaitingForTradeCompletion = true;

          m_posPut = CPortfolioManager::Instance().ConstructPosition( m_idPortfolio, "O", "same", "ib01", "ib01", m_pExecutionProvider, m_pDataProvider, m_iterOILatestGammaSelectPut->second.Put()->GetInstrument() );
          m_posPut->OnExecution.Add( MakeDelegate( this, &CProcess::HandlePositionExecution ) );
          m_posPut->PlaceOrder( OrderType::Market, OrderSide::Buy, nPuts );
          m_dblDeltaTotalPut = nPuts * 100.0 * m_iterOILatestGammaSelectPut ->second.Put() ->Delta();

          m_ss.str( "" );
          m_ss << "Opening Delta N:  U" << nLong << "@" << m_dblUnderlyingPrice << " for " << 100 * nLong * m_dblUnderlyingPrice
                               << ", P" << nPuts << "@" << m_dblPutPrice        << " for " << 100 * nPuts * m_dblPutPrice 
                               << std::endl;
          OutputDebugString( m_ss.str().c_str() );
        }
        catch (...) {
          throw std::runtime_error( "error" );
        }
      }
      else {
        if ( ( 0 != m_posUnderlying ) && ( 0 != m_posPut ) ) {
          // don't do anything, already have positions going
        }
        else {
          throw std::runtime_error( "openposition has unmatched positions" );
        }
      }
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

  switch ( m_TradingState ) {
    case ETSTrading:
      HandleTSTrading( quote );
      break;
    case ETSFirstPass:
      HandleTSFirstPass( quote );
      break;
    case ETSPreMarket:
      HandleTSPreMarket( quote );
      break;
//    case ETSMarketOpened: // first call from within HandleTSPreMarket
//      HandleTSMarketopened( trade );
//      break;
    case ETSFirstTrade:
      HandleTSOpeningOrder( quote );
      break;
    case ETSCloseOrders:
      HandleTSCloseOrders( quote );
      break;
    case ETSAfterMarket:
      HandleAfterMarket( quote );
      break;
    default:
      throw std::out_of_range( "CProcess::HandleUnderlyingQuote" );
      break;
  };

}

void CProcess::HandleUnderlyingTrade( const CTrade& trade ) {

  m_dblUnderlyingPrice = trade.Trade();
  m_trades.Append( trade );

}

void CProcess::HandleTSFirstPass( const CQuote& quote ) {
  // may need to open portfoloio and evaluate existing positions here
  m_TradingState = ETSPreMarket;
  m_ss.str( "" );
  m_ss << ou::CTimeSource::Instance().Internal();
  m_ss << " State:  First Pass -> Pre Market." << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::HandleTSPreMarket( const CQuote& quote ) {
  ptime dt = ou::CTimeSource::Instance().Internal();
  if ( dt.time_of_day() >= m_dtMarketOpen ) {
    m_ss.str( "" );
    m_ss << dt;
    m_ss << " State:  Market Opened." << std::endl;
    OutputDebugString( m_ss.str().c_str() );
    m_TradingState = ETSMarketOpened;
    HandleTSMarketOpened( quote );
  }
}

void CProcess::HandleTSMarketOpened( const CQuote& quote ) {

  double dblOpenValue = ( quote.Bid() + quote.Ask() ) / 2.0;

  // comment our starting trade of the day
  m_ss.str( "" );
  m_ss << ou::CTimeSource::Instance().Internal();
  m_ss << " Opening mid quote: " << dblOpenValue << std::endl;
  OutputDebugString( m_ss.str().c_str() );

  // set iterators for center of the pack (crossovers are above and below trade):
  m_iterAboveCrossOver = m_vCrossOverPoints.begin();
  while ( dblOpenValue >= *m_iterAboveCrossOver ) {
    ++m_iterAboveCrossOver;
  }
  m_iterBelowCrossOver = m_iterAboveCrossOver;
  while ( dblOpenValue <= *m_iterBelowCrossOver ) {
    --m_iterBelowCrossOver;
  }

  // comment our crossover points
  m_ss.str( "" );
  m_ss << "Trade start " << *m_iterBelowCrossOver << ", " << dblOpenValue << ", " << *m_iterAboveCrossOver << std::endl;
  OutputDebugString( m_ss.str().c_str() );

  // calculate where to have put/call option watches,
  //   have a range of strikes above and below current trade (have maximum 100 watches available)
  m_iterOIHighestWatch = m_mapStrikeInfo.begin();
  while ( (m_iterOIHighestWatch->first) <= dblOpenValue ) {
    ++m_iterOIHighestWatch;
  }
  m_iterOILowestWatch = m_iterOIHighestWatch;
  --m_iterOILowestWatch;
  for ( int i = 0; i < 15; ++i ) {
    if ( m_mapStrikeInfo.begin() != m_iterOILowestWatch ) {
      --m_iterOILowestWatch;
    }
    if ( m_mapStrikeInfo.end() != m_iterOIHighestWatch ) {
      ++m_iterOIHighestWatch;
    }
  }

  // set the actual watches
  m_bWatchingOptions = true;
  if ( keytypes::EProviderSimulator != m_pDataProvider->ID() ) {
    for ( mapStrikeInfo_iter_t iter = m_iterOILowestWatch; iter != m_iterOIHighestWatch; ++iter ) {

      CStrikeInfo& oi = iter->second;

      m_pDataProvider->AddQuoteHandler( oi.Call()->GetInstrument(), MakeDelegate( oi.Call(), &CNakedCall::HandleQuote ) );
      m_pDataProvider->AddTradeHandler( oi.Call()->GetInstrument(), MakeDelegate( oi.Call(), &CNakedCall::HandleTrade ) );
      m_pDataProvider->AddGreekHandler( oi.Call()->GetInstrument(), MakeDelegate( oi.Call(), &CNakedCall::HandleGreek ) );

      m_pDataProvider->AddQuoteHandler( oi.Put()->GetInstrument(),  MakeDelegate( oi.Put(),  &CNakedPut::HandleQuote ) );
      m_pDataProvider->AddTradeHandler( oi.Put()->GetInstrument(),  MakeDelegate( oi.Put(),  &CNakedPut::HandleTrade ) );
      m_pDataProvider->AddGreekHandler( oi.Put()->GetInstrument(),  MakeDelegate( oi.Put(),  &CNakedPut::HandleGreek ) );
    }
  }

  m_TradingState = ETSFirstTrade;
}

void CProcess::HandleTSOpeningOrder( const CQuote& quote ) {

  ptime dt = ou::CTimeSource::Instance().Internal();
  if ( dt.time_of_day() >= m_dtMarketOpeningOrder ) {
    m_ss.str( "" );
    m_ss << ou::CTimeSource::Instance().Internal();
    m_ss << " State:  Opening Order." << std::endl;
    OutputDebugString( m_ss.str().c_str() );

    m_bTrading = true;
    OpenPosition();
    m_TradingState = ETSTrading;
  }
}

void CProcess::HandlePositionExecution( CPosition::execution_delegate_t pair ) {
  m_ss.str( "" );
  ptime dt = ou::CTimeSource::Instance().Internal();
  m_ss << dt;
  m_ss << " Execution: " << pair.first.GetInstrument()->GetInstrumentName() << " " 
    << OrderSide::Name[ pair.second.GetOrderSide() ] << " " 
    << pair.second.GetSize() << "@" << pair.second.GetPrice()
    << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::HandleTSTrading( const CQuote& quote ) {

//  m_dblCallPrice = m_iterOILatestGammaSelectCall->Call()->Ask();
//  m_dblPutPrice = m_iterOILatestGammaSelectPut->Put()->Ask();

  ptime dt = ou::CTimeSource::Instance().Internal();
  if ( dt.time_of_day() >= m_dtMarketClosingOrder ) {
    m_ss.str( "" );
    m_ss << dt;
    m_ss << " State:  Close Orders." << std::endl;
    OutputDebugString( m_ss.str().c_str() );

    m_TradingState = ETSCloseOrders;
  }
  else {

    double dblMidQuote = ( quote.Bid() + quote.Ask() ) / 2.0;

    double dblDeltaPut  = m_iterOILatestGammaSelectPut->second.Put()->Delta() * m_posPut->GetRow().nPositionActive * 100;
    //double dblDeltaCall = m_iterOILatestGammaSelectCall->second.Call()->Delta() * m_nCalls * 100;

    bool bTraded = false;
    int nOptions = 0;

    double dblDeltaHi = m_dblBaseDelta + m_dblBaseDeltaIncrement;
    double dblDeltaLo = m_dblBaseDelta - m_dblBaseDeltaIncrement;

    try {

      // todo:  while implied volatility is rising, hold back on exiting?
      // todo:  while implied volatility is falling, hold back on entering?

      double dblDeltaDif = dblDeltaPut + m_dblDeltaTotalUnderlying;
      if ( dblDeltaDif > m_dblBaseDeltaIncrement ) { // sell underlying to get closer to put delta
        //m_posUnderlying->PlaceOrder( OrderType::Market, OrderSide::Sell, m_dblBaseDeltaIncrement / 100 ); // <<=== temporary fix for this simulation set
        m_posUnderlying->PlaceOrder( OrderType::Market, OrderSide::Sell, m_dblBaseDeltaIncrement );
        bTraded = true;
        m_dblDeltaTotalUnderlying -= m_dblBaseDeltaIncrement;
        m_ss.str( "" );
        m_ss << dt;
        m_ss << " Underlying Sell " << m_dblBaseDeltaIncrement << ", trigger @" << dblMidQuote << std::endl;
        OutputDebugString( m_ss.str().c_str() );
      }
      else {
        if ( dblDeltaDif < -m_dblBaseDeltaIncrement ) { // buy underlying to get closer to put delta
          //m_posUnderlying->PlaceOrder( OrderType::Market, OrderSide::Buy, m_dblBaseDeltaIncrement / 100 ); // <<=== temporary fix for this simulation set
          m_posUnderlying->PlaceOrder( OrderType::Market, OrderSide::Buy, m_dblBaseDeltaIncrement );
          bTraded = true;
          m_dblDeltaTotalUnderlying += m_dblBaseDeltaIncrement;
          m_ss.str( "" );
          m_ss << dt;
          m_ss << " Underlying Buy " << m_dblBaseDeltaIncrement << ", trigger @" << dblMidQuote << std::endl;
          OutputDebugString( m_ss.str().c_str() );
        }
      }

    }
    catch ( std::logic_error &e ) {
      // just catch, don't do anything
    }

    if ( bTraded ) {
      PrintGreeks();
    }
  }
}

void CProcess::HandleTSCloseOrders( const CQuote& quote ) {

  if ( m_bTrading ) {

    int nPuts = m_posPut->GetRow().nPositionActive;
    int nLong = m_posUnderlying->GetRow().nPositionActive;

    m_ss.str( "" );
    m_ss << "Closing Delta N:  U" << nLong << "@" << m_dblUnderlyingPrice << " for " << 100 * nLong * m_dblUnderlyingPrice
                         << ", P" << nPuts             << "@" << m_dblPutPrice        << " for " << 100 * nPuts * m_dblPutPrice 
                          << std::endl;
    OutputDebugString( m_ss.str().c_str() );

    // orders for normal delta neutral
    m_posUnderlying->CancelOrders();

//    m_posUnderlying->ClosePosition();
    m_posPut->CancelOrders();
//    m_posPut->ClosePosition();

    PrintGreeks();

    m_bTrading = false;
  }

  ptime dt = ou::CTimeSource::Instance().Internal();
  if ( dt.time_of_day() >= m_dtMarketClose ) {
    m_ss.str( "" );
    m_ss << dt;
    m_ss << " State:  After Market." << std::endl;
    OutputDebugString( m_ss.str().c_str() );
    m_TradingState = ETSAfterMarket;
  }
}

void CProcess::HandleAfterMarket( const CQuote& quote ) {
}

void CProcess::PrintGreeks( void ) {
  m_ss.str( "" );
  m_ss << "Greeks: " 
    << ou::CTimeSource::Instance().Internal()
    << " Strk "  << m_iterOILatestGammaSelectCall->second.Strike()
//    << " Call "  << m_iterOILatestGammaSelectCall->Call()->OptionPrice()
    << " ImpVo " << m_iterOILatestGammaSelectCall->second.Call()->ImpliedVolatility()
    << " Delta " << m_iterOILatestGammaSelectCall->second.Call()->Delta()
    << " Gamma " << m_iterOILatestGammaSelectCall->second.Call()->Gamma() << " - "
    << " Strk "  << m_iterOILatestGammaSelectPut->second.Strike()
//    << " Put "   << m_iterOILatestGammaSelectPut->Put()->OptionPrice()
    << " ImpVo " << m_iterOILatestGammaSelectPut->second.Put()->ImpliedVolatility()
    << " Delta " << m_iterOILatestGammaSelectPut->second.Put()->Delta()
    << " Gamma " << m_iterOILatestGammaSelectPut->second.Put()->Gamma() 
    << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::SaveSeries( void ) {

  m_ss.str( "" );
  m_ss << ou::CTimeSource::Instance().Internal();

  if ( keytypes::EProviderSimulator == m_pDataProvider->ID() ) {
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
      CHDF5Attributes attributes( sPathName, InstrumentType::Stock );
      attributes.SetProviderType( m_pDataProvider->ID() );
    }
  }
  catch (...) {
  }

  try {
    if ( 0 != m_trades.Size() ) {
      sPathName = m_sPathForSeries + "/" + m_ss.str() + "/trades/" + m_sSymbolName;
      wtsTrades.Write( sPathName, &m_trades );
      CHDF5Attributes attributes( sPathName, InstrumentType::Stock );
      attributes.SetProviderType( m_pDataProvider->ID() );
    }
  }
  catch (...) {
  }

  for ( mapStrikeInfo_iter_t iter = m_iterOILowestWatch; iter != m_iterOIHighestWatch; ++iter ) {

    CStrikeInfo& oi = iter->second;

    try {
      if ( 0 != oi.Call()->Quotes()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/quotes/" + oi.Call()->GetInstrument()->GetInstrumentName();
        wtsQuotes.Write( sPathName, oi.Call()->Quotes() );
        CHDF5Attributes::structOption option( oi.Call()->GetInstrument()->GetStrike(), 
          oi.Call()->GetInstrument()->GetExpiryYear(), oi.Call()->GetInstrument()->GetExpiryMonth(), oi.Call()->GetInstrument()->GetExpiryDay(),
          oi.Call()->GetInstrument()->GetOptionSide() );
        CHDF5Attributes attributes( sPathName, option );
        attributes.SetProviderType( m_pDataProvider->ID() );
      }

      if ( 0 != oi.Call()->Trades()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/trades/" + oi.Call()->GetInstrument()->GetInstrumentName();
        wtsTrades.Write( sPathName, oi.Call()->Trades() );
        CHDF5Attributes::structOption option( oi.Call()->GetInstrument()->GetStrike(), 
          oi.Call()->GetInstrument()->GetExpiryYear(), oi.Call()->GetInstrument()->GetExpiryMonth(), oi.Call()->GetInstrument()->GetExpiryDay(),
          oi.Call()->GetInstrument()->GetOptionSide() );
        CHDF5Attributes attributes( sPathName, option );
        attributes.SetProviderType( m_pDataProvider->ID() );
      }

      if ( 0 != oi.Call()->Greeks()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/greeks/" + oi.Call()->GetInstrument()->GetInstrumentName();
        wtsGreeks.Write( sPathName, oi.Call()->Greeks() );
        CHDF5Attributes::structOption option( oi.Call()->GetInstrument()->GetStrike(), 
          oi.Call()->GetInstrument()->GetExpiryYear(), oi.Call()->GetInstrument()->GetExpiryMonth(), oi.Call()->GetInstrument()->GetExpiryDay(),
          oi.Call()->GetInstrument()->GetOptionSide() );
        CHDF5Attributes attributes( sPathName, option );
        attributes.SetProviderType( m_pDataProvider->ID() );
      }

      if ( 0 != oi.Put()->Quotes()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/quotes/" + oi.Put()->GetInstrument()->GetInstrumentName();
        wtsQuotes.Write( sPathName, oi.Put()->Quotes() );
        CHDF5Attributes::structOption option( oi.Put()->GetInstrument()->GetStrike(), 
          oi.Put()->GetInstrument()->GetExpiryYear(), oi.Put()->GetInstrument()->GetExpiryMonth(), oi.Put()->GetInstrument()->GetExpiryDay(),
          oi.Put()->GetInstrument()->GetOptionSide() );
        CHDF5Attributes attributes( sPathName, option );
        attributes.SetProviderType( m_pDataProvider->ID() );
      }

      if ( 0 != oi.Put()->Trades()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/trades/" + oi.Put()->GetInstrument()->GetInstrumentName();
        wtsTrades.Write( sPathName, oi.Put()->Trades() );
        CHDF5Attributes::structOption option( oi.Put()->GetInstrument()->GetStrike(), 
          oi.Put()->GetInstrument()->GetExpiryYear(), oi.Put()->GetInstrument()->GetExpiryMonth(), oi.Put()->GetInstrument()->GetExpiryDay(),
          oi.Put()->GetInstrument()->GetOptionSide() );
        CHDF5Attributes attributes( sPathName, option );
        attributes.SetProviderType( m_pDataProvider->ID() );
      }

      if ( 0 != oi.Put()->Greeks()->Size() ) {
        sPathName = m_sPathForSeries + "/" + m_ss.str() + "/greeks/" + oi.Put()->GetInstrument()->GetInstrumentName();
        wtsGreeks.Write( sPathName, oi.Put()->Greeks() );
        CHDF5Attributes::structOption option( oi.Put()->GetInstrument()->GetStrike(), 
          oi.Put()->GetInstrument()->GetExpiryYear(), oi.Put()->GetInstrument()->GetExpiryMonth(), oi.Put()->GetInstrument()->GetExpiryDay(),
          oi.Put()->GetInstrument()->GetOptionSide() );
        CHDF5Attributes attributes( sPathName, option );
        attributes.SetProviderType( m_pDataProvider->ID() );
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
  m_ss << ou::CTimeSource::Instance().Internal();
  m_ss << ": ";
  m_pPortfolio->EmitStats( m_ss );
  if ( EModeSimulation == m_eMode ) {
    m_sim->EmitStats( m_ss );
  }
  m_ss << std::endl;

  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::HandlePopulateDatabase( void ) {

  ou::tf::CAccountManager::pAccountAdvisor_t pAccountAdvisor 
    = ou::tf::CAccountManager::Instance().ConstructAccountAdvisor( "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::CAccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::CAccountManager::Instance().ConstructAccountOwner( "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::CAccountManager::pAccount_t pAccountIB
    = ou::tf::CAccountManager::Instance().ConstructAccount( "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::CAccountManager::pAccount_t pAccountIQFeed
    = ou::tf::CAccountManager::Instance().ConstructAccount( "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::CPortfolioManager::pPortfolio_t pPortfolio
    = ou::tf::CPortfolioManager::Instance().ConstructPortfolio( m_idPortfolio, "aoRay", "TradeGldOptions" );

}

// need to worry about rogue trades... trades out side of the normal trading range
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

/*
Some individual stocks carry a small edge for gamma scalping : unusual intraday hi-lo / close-to-close ratio. While an average is around 1.5 , a few (SNDK , MSTR) hitting 2. Stocks with high monthly numbers of up(down) grades are great too.
So, you can only earn from this method if you do not adjust too often. You have to profit from the delta being non-zero, letting gamma do it's work. The profits are represented by the holes between the original delta-curve and the broken line formed by the adjusted position.
Actually you probably could make a very good living gamma scalping stocks the last week of expiration. Due in part to the fact that volatility will have very little effect on the option premiums.
It has nothing to do with model frequency. It's purely psychological factors. If statvol is proven to be > implied than the trade will be a winner, provided the trader doesn't fcuk it up.

*/
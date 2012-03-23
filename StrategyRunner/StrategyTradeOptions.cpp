/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// 2012/03/05:  keep track of implied volatility, at each strike when fixing stuff up, 
//   if current IV  is larger (had a large jump) than previous IV, then do a sell.  If lower, then do a buy.
//   or stay out until IV starts to go down again.

// 2012/03/06:  add stochastic in for the day.  re-adjust deltas on each swing

// 2012/03/12:  need to remove asserts and find some graceful way of handling begin/end of mapOptions table,
//              perhaps ignore any possible trades out side the boundaries (will be issues with flash crashes and such)

// 2012/03/12:  rotate far term options to next couple of months once within 30 days of expiry
//              perform on high low volatility, or high volatility?  or can sell high and buy low?
//              track the volatility, try to sell high volatility, buy low volatility

// 2012/03/12:  track open interest and buy/sell ratios on the tracked options
//              to see if there is useful predictive information

// 2012/03/12:  bayesian prediction machine to use the various factors

// 2012/03/17:  by rebalancing at the pivot, another opportunity for profit taking
//              watch volatility during the day:  weight buying on low volatility, weight selling on high volatility
//              can pivots be done on implied volatility?
//              run volatility smiles.  does that provide any trading opportunities
//              need panel for selecting option to perform a calendar roll over, and add in to position for cost recovery
//              put in %d with %k in stochastic once tools from Intro to HF are created

#include <sstream>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <boost/lexical_cast.hpp>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include <OUCommon/TimeSource.h>

#include <TFTrading/InstrumentManager.h>

#include "StrategyTradeOptions.h"

namespace StrategyTradeOptionsConstants {
  std::string sPortfolioName = "pflioOptions";
  bool bTesting = false;
}

StrategyTradeOptions::StrategyTradeOptions( pProvider_t pExecutionProvider, pProvider_t pData1Provider, pProvider_t pData2Provider ) :
  m_pExecutionProvider( pExecutionProvider ), m_pData1Provider( pData1Provider ), m_pData2Provider( pData2Provider ),
    m_TradeStates( EPreOpen ), m_paramWorkingDelta( 2000.0 ), 
    m_stoch1( m_quotes, 1 * 14 * 60 ), 
    m_stoch2( m_quotes, 2 * 14 * 60 ), 
    m_stoch3( m_quotes, 3 * 14 * 60 ), 
    m_stoch4( m_quotes, 4 * 14 * 60 )
{
  if ( ou::tf::keytypes::EProviderIQF == m_pData1Provider->ID() ) {
    m_pData1ProviderIQFeed = boost::shared_dynamic_cast<ou::tf::CIQFeedProvider>( m_pData1Provider );
  }
  if ( ou::tf::keytypes::EProviderIB == m_pData2Provider->ID() ) {
    m_pData2ProviderIB = boost::shared_dynamic_cast<ou::tf::CIBTWS>( m_pData2Provider );
  }
  if ( ou::tf::keytypes::EProviderIB == pExecutionProvider->ID() ) {
    m_pExecutionProviderIB = boost::shared_dynamic_cast<ou::tf::CIBTWS>( pExecutionProvider );
  }

  if ( StrategyTradeOptionsConstants::bTesting ) {
    m_timeOpeningBell = time_duration(  0,  0,  1 );
    m_timeClosingBell = time_duration( 23, 59, 59 );
  }
  else {
    m_timeOpeningBell = time_duration( 10, 30, 0 );
    m_timeClosingBell = time_duration( 17,  0, 0 );
  }
  m_timeCancel = m_timeClosingBell - time_duration( 0, 6, 0 );
  m_timeClose =  m_timeClosingBell - time_duration( 0, 3, 0 );

  std::cout << ou::CTimeSource::Instance().Internal() << " Starting State: EPreOpen(" <<  StrategyTradeOptionsConstants::bTesting << ")" << std::endl;
}

StrategyTradeOptions::~StrategyTradeOptions(void) {
}

void StrategyTradeOptions::Start( const std::string& sUnderlying, boost::gregorian::date dateOptionNearDate, boost::gregorian::date dateOptionFarDate ) {

  assert( dateOptionNearDate < dateOptionFarDate );
  assert( "" != sUnderlying );

  m_dateOptionNearDate = dateOptionNearDate;
  m_dateOptionFarDate = dateOptionFarDate;
  m_sUnderlying = sUnderlying;

  ou::tf::CInstrumentManager& mgr( ou::tf::CInstrumentManager::Instance() );

  if ( 0 != m_pData1ProviderIQFeed.get() ) {  // risk free rate for IV calcs
    pInstrument_t p10YT;
    if ( mgr.Exists( "TNX.XO" ) ) {
      p10YT = mgr.Get( "TNX.XO" );
    }
    else {
      p10YT = bundle10YrTreasury.pInstrument = mgr.ConstructInstrument( "TNX.XO", "DTN", ou::tf::InstrumentType::Index );
    }
    bundle10YrTreasury.pInstrument = p10YT;
    
    m_pData1ProviderIQFeed->AddQuoteHandler( p10YT, MakeDelegate( &bundle10YrTreasury, &bundle_t::HandleQuote ) );
    m_pData1ProviderIQFeed->AddTradeHandler( p10YT, MakeDelegate( &bundle10YrTreasury, &bundle_t::HandleTrade ) );
  }

  if ( mgr.Exists( sUnderlying ) ) { // instruments should already exist so load from database

    LoadExistingInstrumentsAndPortfolios( sUnderlying );

  }
  else {
    if ( 0 == m_pData2Provider.get() ) { // probably simulation, so maybe use LoadExistingInstruments() ?
      //m_pUnderlying = new ou::tf::InstrumentData( mgr.ConstructInstrument( sUnderlying, "SMART", ou::tf::InstrumentType::Stock ) );
      //HandleUnderlyingContractDetailsDone();
      // we need info from IB to do anything so this is pretty much null code
      throw std::runtime_error( "not a good code branch" );
    }
    else {  // initialize instruments from scratch
      ou::tf::CIBTWS::Contract contract;
      contract.symbol = sUnderlying;
      contract.exchange = "SMART";
      contract.secType = "STK";
      contract.currency = "USD";
      m_pData2ProviderIB->RequestContractDetails( 
        contract,
        MakeDelegate( this, &StrategyTradeOptions::HandleUnderlyingContractDetails ),
        MakeDelegate( this, &StrategyTradeOptions::HandleUnderlyingContractDetailsDone )
        );
    }
  }

}

void StrategyTradeOptions::Stop( void ) {
//  m_pUnderlying->RemoveQuoteHandler( m_pData1Provider );
//  m_pUnderlying->RemoveTradeHandler( m_pData1Provider );
}

void StrategyTradeOptions::HandleUnderlyingContractDetails( const ou::tf::CIBTWS::ContractDetails& cd, ou::tf::CIBTWS::pInstrument_t& pInstrument ) {
  ou::tf::CInstrumentManager& mgr( ou::tf::CInstrumentManager::Instance() );
  mgr.Register( pInstrument );
//  m_pUnderlying = new ou::tf::InstrumentData( pInstrument );
}

void StrategyTradeOptions::HandleUnderlyingContractDetailsDone( void ) {

//  m_pUnderlying->AddQuoteHandler( m_pData1Provider );
//  m_pUnderlying->AddTradeHandler( m_pData1Provider );

  // obtain near date contracts
  ou::tf::CIBTWS::Contract contract;
  contract.symbol = m_sUnderlying;
  contract.exchange = "SMART";
  contract.secType = "OPT";
  contract.currency = "USD";
  contract.expiry = boost::gregorian::to_iso_string( m_dateOptionNearDate );
  m_pData2ProviderIB->RequestContractDetails( 
    contract,
    MakeDelegate( this, &StrategyTradeOptions::HandleNearDateContractDetails ),
    MakeDelegate( this, &StrategyTradeOptions::HandleNearDateContractDetailsDone )
    );

}

void StrategyTradeOptions::HandleNearDateContractDetails( const ou::tf::CIBTWS::ContractDetails&, ou::tf::CIBTWS::pInstrument_t& pInstrument ) {
  ou::tf::CInstrumentManager& mgr( ou::tf::CInstrumentManager::Instance() );
  if ( 0 != m_pData1ProviderIQFeed.get() ) {
    m_pData1ProviderIQFeed->SetAlternateInstrumentName( pInstrument );
  }
  mgr.Register( pInstrument );
}

void StrategyTradeOptions::HandleNearDateContractDetailsDone( void ) {
  // obtain far date contracts
  ou::tf::CIBTWS::Contract contract;
  contract.symbol = m_sUnderlying;
  contract.exchange = "SMART";
  contract.secType = "OPT";
  contract.currency = "USD";
  contract.expiry = boost::gregorian::to_iso_string( m_dateOptionFarDate );
  m_pData2ProviderIB->RequestContractDetails( 
    contract,
    MakeDelegate( this, &StrategyTradeOptions::HandleFarDateContractDetails ),
    MakeDelegate( this, &StrategyTradeOptions::HandleFarDateContractDetailsDone )
    );
}

void StrategyTradeOptions::HandleFarDateContractDetails( const ou::tf::CIBTWS::ContractDetails&, ou::tf::CIBTWS::pInstrument_t& pInstrument ) {
  ou::tf::CInstrumentManager& mgr( ou::tf::CInstrumentManager::Instance() );
  if ( 0 != m_pData1ProviderIQFeed.get() ) {
    m_pData1ProviderIQFeed->SetAlternateInstrumentName( pInstrument );
  }
  mgr.Register( pInstrument );
}

void StrategyTradeOptions::HandleFarDateContractDetailsDone( void ) {
  LoadExistingInstrumentsAndPortfolios( m_sUnderlying );
}

void StrategyTradeOptions::LoadExistingInstrumentsAndPortfolios( const std::string& sUnderlying ) {

  std::stringstream ss;
  ss << ou::CTimeSource::Instance().External();
  m_sTimeStampWatchStarted = "/app/strategy/options/" + ss.str();  // will need to make this generic if need some for multiple providers.

  ou::tf::CInstrumentManager& mgr( ou::tf::CInstrumentManager::Instance() );
  m_pUnderlying = mgr.Get( sUnderlying );
  mgr.ScanOptions( 
    boost::phoenix::bind( &StrategyTradeOptions::HandleNearOptionsLoad, this, boost::phoenix::arg_names::arg1 ), 
    sUnderlying,
    m_dateOptionNearDate.year(), m_dateOptionNearDate.month(), m_dateOptionNearDate.day() 
    );
  mgr.ScanOptions( 
    boost::phoenix::bind( &StrategyTradeOptions::HandleFarOptionsLoad, this, boost::phoenix::arg_names::arg1 ), 
    sUnderlying,
    m_dateOptionFarDate.year(), m_dateOptionFarDate.month(), m_dateOptionFarDate.day() 
    );
  m_pData1Provider->AddQuoteHandler( m_pUnderlying, MakeDelegate( this, &StrategyTradeOptions::HandleQuote ) );
  m_pData1Provider->AddTradeHandler( m_pUnderlying, MakeDelegate( this, &StrategyTradeOptions::HandleTrade ) );

  // Load the portfolios, if any
  ou::tf::CPortfolioManager& mgrPortfolios( ou::tf::CPortfolioManager::Instance() );
  m_pPortfolio = mgrPortfolios.GetPortfolio( StrategyTradeOptionsConstants::sPortfolioName );  // from StrategyRunner::HandlePopulateDatabase
  mgrPortfolios.ScanPositions( 
    StrategyTradeOptionsConstants::sPortfolioName, 
    boost::phoenix::bind( &StrategyTradeOptions::HandlePositionsLoad, this, boost::phoenix::arg_names::arg1 )
    );

}

void StrategyTradeOptions::HandleNearOptionsLoad( pInstrument_t pInstrument ) {
  double strike = pInstrument->GetStrike();
  mapOptions_t::iterator iter = m_mapOptions.find( strike );
  if ( m_mapOptions.end() == iter ) {
    m_mapOptions[ strike ].strike = strike;
  }
  switch ( pInstrument->GetOptionSide() ) {
  case ou::tf::OptionSide::Call:
    m_mapOptions[ strike ].optionNearDateCall.pOption.reset( new ou::tf::option::Call( pInstrument, m_pData1Provider, m_pData2Provider ) );
    break;
  case ou::tf::OptionSide::Put:
    m_mapOptions[ strike ].optionNearDatePut.pOption.reset( new ou::tf::option::Put( pInstrument, m_pData1Provider, m_pData2Provider ) );
    break;
  }
}

void StrategyTradeOptions::HandleFarOptionsLoad( pInstrument_t pInstrument ) {
  double strike = pInstrument->GetStrike();
  mapOptions_t::iterator iter = m_mapOptions.find( strike );
  if ( m_mapOptions.end() == iter ) {
    m_mapOptions[ strike ].strike = strike;
  }
  switch ( pInstrument->GetOptionSide() ) {
  case ou::tf::OptionSide::Call:
    m_mapOptions[ strike ].optionFarDateCall.pOption.reset( new ou::tf::option::Call( pInstrument, m_pData1Provider, m_pData2Provider ) );
    break;
  case ou::tf::OptionSide::Put:
    m_mapOptions[ strike ].optionFarDatePut.pOption.reset( new ou::tf::option::Put( pInstrument, m_pData1Provider, m_pData2Provider ) );
    break;
  }
}

void StrategyTradeOptions::HandleTrade( const ou::tf::CTrade& trade ) {
  m_trades.Append( trade );
}

void StrategyTradeOptions::HandleQuote( const ou::tf::CQuote& quote ) {

  if ( !quote.IsValid() ) return;  // ** check this for options as well
  
  m_quotes.Append( quote );
  double midpoint = quote.Midpoint();

  time_duration tod( quote.DateTime().time_of_day() );

  switch ( m_TradeStates ) {
  case EPreOpen:
    if ( m_timeOpeningBell <= tod ) {
      m_TradeStates = EBellHeard;
      std::cout << ou::CTimeSource::Instance().Internal() << " New State: EBellHeard" << std::endl;
    }
    break;
  case EBellHeard:  // will need to wait for a bit for options to settle
    assert( 0 != m_mapOptions.size() );
    m_iterMapOptionsAbove1 = m_mapOptions.begin();
    m_iterMapOptionsAbove2 
      = m_iterMapOptionsMiddle
      = m_iterMapOptionsBelow1 = m_iterMapOptionsBelow2 
      = m_mapOptions.end();
    SetPointersFirstTime( quote );
    m_timePauseForQuotes = tod + time_duration( 0, 0, 30 );
    m_TradeStates = EPauseForQuotes;
    std::cout << ou::CTimeSource::Instance().Internal() << " New State: EPauseForQuotes" << std::endl;
    break;
  case EPauseForQuotes:
    if ( m_timePauseForQuotes <= tod ) {
      m_timePauseForQuotes = tod + time_duration( 0, 0, 30 );
      m_TradeStates = EAfterBell;
      std::cout << ou::CTimeSource::Instance().Internal() << " New State: EAfterBell" << std::endl;
    }
    break;
  case EAfterBell:
    AdjustThePointers( quote );  // keep everything adjusted for when ready to adjust the options
    if ( m_timePauseForQuotes <= tod ) {
      std::cout << ou::CTimeSource::Instance().Internal() << " EAfterBell: Adjust Options" << std::endl;
      AdjustTheOptions( quote );  // opening balance
      m_TradeStates = ETrading;
      std::cout << ou::CTimeSource::Instance().Internal() << " New State: ETrading" << std::endl;
    }
    break;
  case ETrading:
    // two stages:  profit taking, then re-balancing
    // * profit taking on anything not with current strike
    if ( m_timeCancel <= tod ) {
      m_TradeStates = ECancelling;
      std::cout << ou::CTimeSource::Instance().Internal() << " New State: ECancelling" << std::endl;
    }
    else {
      if ( AdjustThePointers( quote ) ) {
        std::cout << ou::CTimeSource::Instance().Internal() << " Adjusting Options" << std::endl;
        AdjustTheOptions( quote );  // rebalance
      }
    }
    break;
  case ECancelling:
    AdjustThePointers( quote );
    if ( m_timeClose <= tod ) {
      m_TradeStates = EGoingNeutral;
      std::cout << ou::CTimeSource::Instance().Internal() << " New State: EGoingNeutral" << std::endl;
    }
    break;
  case EGoingNeutral: 
    AdjustThePointers( quote );
    std::cout << ou::CTimeSource::Instance().Internal() << " Final Options Go Neutral" << std::endl;
    AdjustTheOptions( quote );  // rebalance
    m_TradeStates = EClosing;
    break;
  case EClosing:
    if ( m_timeClosingBell <= tod ) {
      m_TradeStates = EAfterHours;
      std::cout << ou::CTimeSource::Instance().Internal() << " New State: EAfterHours" << std::endl;
    }
    break;
  case EAfterHours:
    break;
  }
}

void StrategyTradeOptions::AdjustTheOptions( const ou::tf::CQuote& quote ) {
  // add up put delta, call delta, rebalance each side to working delta
  // use m_iterMapOptionsMiddle as the current strike adjustment
  // might be more efficient to have an active list of options and positions to make it easy to scan and update for:
  //   check over all delta
  //   see if something is at current strike for adjustment

  // 2012/03/12  don't rebalance during large directional movement (something to check in the state machine above)

  greeks_t greekCalls;
  greeks_t greekPuts;

  statsOptions statsCalls;
  statsOptions statsPuts;

  for ( mapOptions_iter_t iter = m_mapOptions.begin(); m_mapOptions.end() != iter; ++iter ) {
    // use visitor pattern to access each call and put
    iter->second.ScanCallOptions( greekCalls, statsCalls, boost::phoenix::bind( &StrategyTradeOptions::ProcessOptions<call_t>, this, boost::phoenix::arg_names::arg1, boost::phoenix::arg_names::arg2, boost::phoenix::arg_names::arg3 ) );
    iter->second.ScanPutOptions(  greekPuts,  statsPuts, boost::phoenix::bind( &StrategyTradeOptions::ProcessOptions<put_t>,  this, boost::phoenix::arg_names::arg1, boost::phoenix::arg_names::arg2, boost::phoenix::arg_names::arg3 ) );
  }

  double deltaCall( greekCalls.delta * 100.0 );
  double deltaPut( greekPuts.delta *  100.0 );

  int nToBuy;

  // after call delta and put delta tallied up, rebalance the portfolio
//  if ( m_paramWorkingDelta < m_deltaCall ) {
    // something wierd
//  }
//  else {
  std::cout << ou::CTimeSource::Instance().Internal() << " calls: " << deltaCall << ", " << m_iterMapOptionsMiddle->second.optionFarDateCall.pOption->Delta() << std::endl;
    nToBuy = ( ( m_paramWorkingDelta - deltaCall ) / 100.0 )
      / m_iterMapOptionsMiddle->second.optionFarDateCall.pOption->Delta();
    if ( 0 < nToBuy ) {
      if ( m_mapOptions.end() == m_iterMapOptionsMiddle ) {
        std::cout << "deltaCall Problem" << std::endl;
      }
      else {
        if ( 0 == m_iterMapOptionsMiddle->second.optionFarDateCall.pPosition.get() ) {
          // create the position
          m_iterMapOptionsMiddle->second.optionFarDateCall.pPosition =  
            ou::tf::CPortfolioManager::Instance().ConstructPosition( StrategyTradeOptionsConstants::sPortfolioName, 
            "callfar" + boost::lexical_cast<std::string>( m_iterMapOptionsMiddle->first ), "far call buy",
            m_pExecutionProvider->GetName(), m_pData1Provider->GetName(), 
            m_pExecutionProvider, m_pData1Provider,
            m_iterMapOptionsMiddle->second.optionFarDateCall.pOption->GetInstrument() );
        }
        std::cout << ou::CTimeSource::Instance().Internal() << " place far call buy" << std::endl;
        m_iterMapOptionsMiddle->second.optionFarDateCall.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, nToBuy );
      }
    }
//  }

//  if ( m_paramWorkingDelta < -m_deltaPut ) {
//    std::cout << "deltaPut Problem" << std::endl;
//  }
//  else {
    std::cout << ou::CTimeSource::Instance().Internal() << "  puts: " << deltaPut << ", " << m_iterMapOptionsMiddle->second.optionFarDatePut.pOption->Delta() << std::endl;
    nToBuy = ( ( m_paramWorkingDelta + deltaPut ) / 100.0 )
      / -m_iterMapOptionsMiddle->second.optionFarDatePut.pOption->Delta();
    if ( 0 < nToBuy ) {
      if ( m_mapOptions.end() == m_iterMapOptionsMiddle ) {
        std::cout << "deltaPut Problem" << std::endl;
      }
      else {
        if ( 0 == m_iterMapOptionsMiddle->second.optionFarDatePut.pPosition.get() ) {
          // create the position
          m_iterMapOptionsMiddle->second.optionFarDatePut.pPosition =  
            ou::tf::CPortfolioManager::Instance().ConstructPosition( StrategyTradeOptionsConstants::sPortfolioName, 
            "putfar" + boost::lexical_cast<std::string>( m_iterMapOptionsMiddle->first ), "far put buy",
            m_pExecutionProvider->GetName(), m_pData1Provider->GetName(), 
            m_pExecutionProvider, m_pData1Provider,
            m_iterMapOptionsMiddle->second.optionFarDatePut.pOption->GetInstrument() );
        }
        std::cout << ou::CTimeSource::Instance().Internal() << " place far put buy" << std::endl;
        m_iterMapOptionsMiddle->second.optionFarDatePut.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, nToBuy );
      }
    }
//  }
  
}

bool StrategyTradeOptions::AdjustThePointers( const ou::tf::CQuote& quote ) {
  // turn on and turn off monitoring as move over the options
  bool bReturn = false;
  double midpoint = quote.Midpoint();
  if ( midpoint >= m_iterMapOptionsAbove1->first ) {  // adjust upwards
    ++m_iterMapOptionsAbove2;
    assert( m_mapOptions.end() != m_iterMapOptionsAbove2 );
    m_iterMapOptionsAbove2->second.StartWatch();
    ++m_iterMapOptionsAbove1;
    ++m_iterMapOptionsMiddle;
    ++m_iterMapOptionsBelow1;
    m_iterMapOptionsBelow2->second.StopWatch();
    ++m_iterMapOptionsBelow2;
    //m_iterMapOptionsBelow2->second.StartWatch();  // is there a delay in trading necessary until first quotes arrive?

    std::cout << ou::CTimeSource::Instance().Internal() << " strikes upwards: " 
    << m_iterMapOptionsBelow2->first << ", " 
    << m_iterMapOptionsBelow1->first << ", " 
    << m_iterMapOptionsMiddle->first << ", " 
    << m_iterMapOptionsAbove1->first << ", " 
    << m_iterMapOptionsAbove2->first
      << std::endl;
    bReturn = true;
  }
  else {
    if ( midpoint <= m_iterMapOptionsBelow1->first ) { // adjust downwards
      assert( m_mapOptions.begin() != m_iterMapOptionsBelow2 );
      --m_iterMapOptionsBelow2;
      m_iterMapOptionsBelow2->second.StartWatch();
      --m_iterMapOptionsBelow1;
      --m_iterMapOptionsMiddle;
      --m_iterMapOptionsAbove1;
      m_iterMapOptionsAbove2->second.StopWatch();
      --m_iterMapOptionsAbove2;
      //m_iterMapOptionsAbove2->second.StartWatch();  // is there a delay in trading necessary until first quotes arrive?

      std::cout << ou::CTimeSource::Instance().Internal() << " strikes downwards: " 
      << m_iterMapOptionsBelow2->first << ", " 
      << m_iterMapOptionsBelow1->first << ", " 
      << m_iterMapOptionsMiddle->first << ", " 
      << m_iterMapOptionsAbove1->first << ", " 
      << m_iterMapOptionsAbove2->first
        << std::endl;
        bReturn = true;
    }
  }
  return bReturn;
}

bool StrategyTradeOptions::SetPointersFirstTime( const ou::tf::CQuote& quote ) {

  // get close to the current strike to start collecting greek information

  bool bReturn = false;

  double midpoint = quote.Midpoint();
  while ( midpoint >= m_iterMapOptionsAbove1->first ) {
    ++m_iterMapOptionsAbove1;
    assert( m_mapOptions.end() != m_iterMapOptionsAbove1 );
  }

  unsigned int cnt = 0;
  m_iterMapOptionsBelow1 = m_iterMapOptionsAbove1;
  while ( midpoint <= m_iterMapOptionsBelow1->first ) {
    assert( m_mapOptions.begin() != m_iterMapOptionsBelow1 );
    --m_iterMapOptionsBelow1;
    if ( 0 == cnt ) {
      if ( midpoint == m_iterMapOptionsBelow1->first ) {
        m_iterMapOptionsMiddle = m_iterMapOptionsBelow1;
        bReturn = true;
      }
    }
    ++cnt;
  }

  // take average then adjust upwards or downwards based upon in upper half or in lower half
  if ( m_mapOptions.end() == m_iterMapOptionsMiddle ) {
    double average = ( m_iterMapOptionsAbove1->first + m_iterMapOptionsBelow1->first ) / 2.0;
    if ( midpoint >= average ) {
      m_iterMapOptionsMiddle = m_iterMapOptionsAbove1;
      ++m_iterMapOptionsAbove1;
      assert( m_mapOptions.end() != m_iterMapOptionsAbove1 );
    }
    else {
      assert( m_mapOptions.begin() != m_iterMapOptionsBelow1 );
      m_iterMapOptionsMiddle = m_iterMapOptionsBelow1;
      --m_iterMapOptionsBelow1;
    }
  }

  assert( m_mapOptions.begin() != m_iterMapOptionsBelow1 );
  m_iterMapOptionsBelow2 = m_iterMapOptionsBelow1;
  --m_iterMapOptionsBelow2;

  m_iterMapOptionsAbove2 = m_iterMapOptionsAbove1;
  ++m_iterMapOptionsAbove2;
  assert( m_mapOptions.end() != m_iterMapOptionsAbove2 );

  m_iterMapOptionsAbove2->second.StartWatch();
  m_iterMapOptionsAbove1->second.StartWatch();
  m_iterMapOptionsMiddle->second.StartWatch();
  m_iterMapOptionsBelow1->second.StartWatch();
  m_iterMapOptionsBelow2->second.StartWatch();

  std::cout << ou::CTimeSource::Instance().Internal() << " opening strikes: " 
    << m_iterMapOptionsBelow2->first << ", " 
    << m_iterMapOptionsBelow1->first << ", " 
    << m_iterMapOptionsMiddle->first << ", " 
    << m_iterMapOptionsAbove1->first << ", " 
    << m_iterMapOptionsAbove2->first
    << std::endl;

  return bReturn;
}

void StrategyTradeOptions::HandlePositionsLoad( pPosition_t pPosition ) {

//  if ( 0 == pPosition->GetRow().nPositionActive ) return; // ignore zero size positions

  ou::tf::CInstrument::pInstrument_cref pInstrument( pPosition->GetInstrument() );
  assert( pInstrument->IsOption() );  // ensure everything is an option, underlying is being tracked but not traded, yet

  double strike = pInstrument->GetStrike();
  mapOptions_iter_t iter = m_mapOptions.find( strike );
  assert( m_mapOptions.end() != iter );

  switch( pInstrument->GetOptionSide() ) {
  case ou::tf::OptionSide::Call:
    if ( m_dateOptionNearDate == pInstrument->GetExpiry() ) {
      iter->second.optionNearDateCall.pPosition = pPosition;
      call_t call( iter->second.optionNearDateCall.pOption, pPosition );
      m_vCalls.push_back( call );
      call.pOption->StartWatch();
    }
    else {
      if ( m_dateOptionFarDate == pInstrument->GetExpiry() ) {
        iter->second.optionFarDateCall.pPosition = pPosition;
        call_t call( iter->second.optionFarDateCall.pOption, pPosition );
        m_vCalls.push_back( call );
        call.pOption->StartWatch();
      }
      else {
        call_t::pOption_t pOption( new ou::tf::option::Call( pPosition->GetInstrument(), m_pData1Provider, m_pData2Provider ) );
        call_t call( pOption, pPosition );
        iter->second.vOtherCalls.push_back( call );
        m_vCalls.push_back( call );
        call.pOption->StartWatch();
      }
    }
    break;
  case ou::tf::OptionSide::Put:
    if ( m_dateOptionNearDate == pInstrument->GetExpiry() ) {
      iter->second.optionNearDatePut.pPosition = pPosition;
      put_t put( iter->second.optionNearDatePut.pOption, pPosition );
      m_vPuts.push_back( put );
      put.pOption->StartWatch();
    }
    else {
      if ( m_dateOptionFarDate == pInstrument->GetExpiry() ) {
        iter->second.optionFarDatePut.pPosition = pPosition;
        put_t put( iter->second.optionFarDatePut.pOption, pPosition );
        m_vPuts.push_back( put );
        put.pOption->StartWatch();
      }
      else {
        put_t::pOption_t pOption( new ou::tf::option::Put( pPosition->GetInstrument(), m_pData1Provider, m_pData2Provider ) );
        put_t put( pOption, pPosition );
        iter->second.vOtherPuts.push_back( put );
        m_vPuts.push_back( put );
        put.pOption->StartWatch();
      }
    }
    break;
  }
}

void StrategyTradeOptions::Save( const std::string& sPrefix ) {

  // stop watch, wait for a sec, then save?

  std::string sPathName;

  try {

    std::cout << "Saving Quotes:" << std::endl;

    if ( 0 != m_quotes.Size() ) {
      sPathName = sPrefix + "/quotes/" + m_pUnderlying->GetInstrumentName();
      ou::tf::CHDF5WriteTimeSeries<ou::tf::CQuotes, ou::tf::CQuote> wtsQuotes;
      wtsQuotes.Write( sPathName, &m_quotes );
      ou::tf::CHDF5Attributes attrQuotes( sPathName, m_pUnderlying->GetInstrumentType() );
      attrQuotes.SetMultiplier( m_pUnderlying->GetMultiplier() );
      attrQuotes.SetSignificantDigits( m_pUnderlying->GetSignificantDigits() );
      attrQuotes.SetProviderType( m_pData1Provider->ID() );
    }

    if ( ( 0 != m_pData1ProviderIQFeed.get() ) && ( 0 != bundle10YrTreasury.quotes.Size() ) ) {
      sPathName = sPrefix + "/quotes/" + bundle10YrTreasury.pInstrument->GetInstrumentName();
      ou::tf::CHDF5WriteTimeSeries<ou::tf::CQuotes, ou::tf::CQuote> wtsQuotes;
      wtsQuotes.Write( sPathName, &bundle10YrTreasury.quotes );
    }

    std::cout << "Saving Trades:" << std::endl;

    if ( 0 != m_trades.Size() ) {
      sPathName = sPrefix + "/trades/" + m_pUnderlying->GetInstrumentName();
      ou::tf::CHDF5WriteTimeSeries<ou::tf::CTrades, ou::tf::CTrade> wtsTrades;
      wtsTrades.Write( sPathName, &m_trades );
      ou::tf::CHDF5Attributes attrTrades( sPathName, m_pUnderlying->GetInstrumentType() );
      attrTrades.SetMultiplier( m_pUnderlying->GetMultiplier() );
      attrTrades.SetSignificantDigits( m_pUnderlying->GetSignificantDigits() );
      attrTrades.SetProviderType( m_pData1Provider->ID() );
    }

    if ( ( 0 != m_pData1ProviderIQFeed.get() ) && ( 0 != bundle10YrTreasury.trades.Size() ) ) {
      sPathName = sPrefix + "/trades/" + bundle10YrTreasury.pInstrument->GetInstrumentName();
      ou::tf::CHDF5WriteTimeSeries<ou::tf::CTrades, ou::tf::CTrade> wtsTrades;
      wtsTrades.Write( sPathName, &bundle10YrTreasury.trades );
    }

    std::cout << "Saving Options:" << std::endl;

    for ( mapOptions_iter_t iter = m_mapOptions.begin(); m_mapOptions.end() != iter; ++iter ) {
      iter->second.Save( sPrefix );
    }

  }
  catch (...) {
    std::cout << "Saving Error." << std::endl;
  }

  std::cout << "Saving Done." << std::endl;

}
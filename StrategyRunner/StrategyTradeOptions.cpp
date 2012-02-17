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

#include <sstream>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include <OUCommon/TimeSource.h>

#include <TFTrading/InstrumentManager.h>

#include "StrategyTradeOptions.h"

StrategyTradeOptions::StrategyTradeOptions( pProvider_t pExecutionProvider, pProvider_t pData1Provider, pProvider_t pData2Provider ) :
  m_pExecutionProvider( pExecutionProvider ), m_pData1Provider( pData1Provider ), m_pData2Provider( pData2Provider ),
    m_TradeStates( EPreOpen ),
    m_timeOpeningBell( 10, 0, 0 ), m_timeCancel( 16, 50, 0 ), m_timeClose( 16, 51, 0 ), m_timeClosingBell( 17, 0, 0 ),
    m_paramWorkingDelta( 2000 )
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
}

StrategyTradeOptions::~StrategyTradeOptions(void) {
}

void StrategyTradeOptions::Start( const std::string& sUnderlying, boost::gregorian::date dateOptionNearDate, boost::gregorian::date dateOptionFarDate ) {
  assert( dateOptionNearDate < dateOptionFarDate );
  assert( "" != sUnderlying );

  m_dateOptionNearDate = dateOptionNearDate;
  m_dateOptionFarDate = dateOptionFarDate;
  m_sUnderlying = sUnderlying;

  ou::tf::CPortfolioManager& mgrPortfolios( ou::tf::CPortfolioManager::Instance() );
  m_pPortfolio = mgrPortfolios.GetPortfolio( "pflioOptions" );  // from StrategyRunner::HandlePopulateDatabase
  mgrPortfolios.ScanPositions( 
    "pflioOptions", 
    boost::phoenix::bind( &StrategyTradeOptions::HandlePositionsLoad, this, boost::phoenix::arg_names::arg1 )
    );

  ou::tf::CInstrumentManager& mgrInstruments( ou::tf::CInstrumentManager::Instance() );

  if ( mgrInstruments.Exists( sUnderlying ) ) { // instruments should already exist so load from database
    LoadExistingInstruments( sUnderlying );
  }
  else {
    if ( 0 == m_pData2ProviderIB.get() ) { // probably simulation, so maybe use LoadExistingInstruments() ?
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
  m_pData1ProviderIQFeed->SetAlternateInstrumentName( pInstrument );
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
  m_pData1ProviderIQFeed->SetAlternateInstrumentName( pInstrument );
  mgr.Register( pInstrument );
}

void StrategyTradeOptions::HandleFarDateContractDetailsDone( void ) {
  LoadExistingInstruments( m_sUnderlying );
}

void StrategyTradeOptions::LoadExistingInstruments( const std::string& sUnderlying ) {

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
  m_pData1ProviderIQFeed->AddQuoteHandler( m_pUnderlying, MakeDelegate( this, &StrategyTradeOptions::HandleQuote ) );
  m_pData1ProviderIQFeed->AddTradeHandler( m_pUnderlying, MakeDelegate( this, &StrategyTradeOptions::HandleTrade ) );
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

  m_quotes.Append( quote );
  double midpoint = quote.Midpoint();

  time_duration dt( quote.DateTime().time_of_day() );

  switch ( m_TradeStates ) {
  case EPreOpen:
    if ( m_timeOpeningBell <= dt ) {
      m_TradeStates = EBellHeard;
    }
    break;
  case EBellHeard:  // will need to wait for a bit for options to settle
    assert( 0 != m_mapOptions.size() );
    m_iterMapOptionsAbove = m_mapOptions.begin();
    AdjustThePointers( quote );
    m_TradeStates = AfterBell;
    break;
  case AfterBell:
  case ETrading:
    // two stages:  profit taking, then re-balancing
    // * profit taking on anything not with current strike
    // * wait for executions, then neutralize
    if ( m_timeCancel <= dt ) {
      m_TradeStates = ECancelling;
    }
    else {
      if ( midpoint >= m_iterMapOptionsAbove->first ) {
        AdjustTheOptions( quote );
        AdjustThePointers( quote );
      }
      else {
        if ( midpoint <= m_iterMapOptionsBelow->first ) {
          AdjustTheOptions( quote );
          AdjustThePointers( quote );
        }
      }
    }
    break;
  case ECancelling:
    if ( m_timeClose <= dt ) {
      m_TradeStates = EGoingNeutral;
    }
    break;
  case EGoingNeutral:
    m_TradeStates = EClosing;
    break;
  case EClosing:
    if ( m_timeClosingBell <= dt ) {
      m_TradeStates = EAfterHours;
    }
    break;
  case EAfterHours:
    break;
  }
}

void StrategyTradeOptions::AdjustTheOptions( const ou::tf::CQuote& quote ) {
}

void StrategyTradeOptions::AdjustThePointers( const ou::tf::CQuote& quote ) {
  // turn on and turn off monitoring as move over the options
  // non zero positions remain unaffected
  double midpoint = quote.Midpoint();
  while ( midpoint >= m_iterMapOptionsAbove->first ) {
    ++m_iterMapOptionsAbove;
    assert( m_mapOptions.end() != m_iterMapOptionsAbove );
  }
  m_iterMapOptionsBelow = m_iterMapOptionsAbove;
  while ( midpoint <= m_iterMapOptionsBelow->first ) {
    assert( m_mapOptions.begin() != m_iterMapOptionsBelow );
    --m_iterMapOptionsBelow;
  }
}

void StrategyTradeOptions::HandlePositionsLoad( pPosition_t pPosition ) {

  if ( 0 == pPosition->GetRow().nPositionActive ) return; // ignore zero size positions

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

  std::string sPathName;

  if ( 0 != m_quotes.Size() ) {
    sPathName = sPrefix + "/quotes/" + m_pUnderlying->GetInstrumentName();
    ou::tf::CHDF5Attributes attrQuotes( sPrefix, m_pUnderlying->GetInstrumentType() );
    attrQuotes.SetMultiplier( m_pUnderlying->GetMultiplier() );
    attrQuotes.SetSignificantDigits( m_pUnderlying->GetSignificantDigits() );
    attrQuotes.SetProviderType( m_pData1ProviderIQFeed->ID() );
    ou::tf::CHDF5WriteTimeSeries<ou::tf::CQuotes, ou::tf::CQuote> wtsQuotes;
    wtsQuotes.Write( sPathName, &m_quotes );
  }

  if ( 0 != m_trades.Size() ) {
    sPathName = sPrefix + "/trades/" + m_pUnderlying->GetInstrumentName();
    ou::tf::CHDF5Attributes attrTrades( sPrefix, m_pUnderlying->GetInstrumentType() );
    attrTrades.SetMultiplier( m_pUnderlying->GetMultiplier() );
    attrTrades.SetSignificantDigits( m_pUnderlying->GetSignificantDigits() );
    attrTrades.SetProviderType( m_pData1ProviderIQFeed->ID() );
    ou::tf::CHDF5WriteTimeSeries<ou::tf::CTrades, ou::tf::CTrade> wtsTrades;
    wtsTrades.Write( sPathName, &m_trades );
  }

  for ( mapOptions_iter_t iter = m_mapOptions.begin(); m_mapOptions.end() != iter; ++iter ) {
    iter->second.Save( sPrefix );
  }

}
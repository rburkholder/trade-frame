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

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <TFTrading/InstrumentManager.h>

#include "StrategyTradeOptions.h"

StrategyTradeOptions::StrategyTradeOptions( pProvider_t pExecutionProvider, pProvider_t pData1Provider, pProvider_t pData2Provider ) :
  m_pExecutionProvider( pExecutionProvider ), m_pData1Provider( pData1Provider ), m_pData2Provider( pData2Provider )
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

  ou::tf::CInstrumentManager& mgr( ou::tf::CInstrumentManager::Instance() );

  if ( mgr.Exists( sUnderlying ) ) { // instruments should already exist so load from database
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
    m_mapOptions[ strike ].pNearDateCall = new ou::tf::option::Call( pInstrument, m_pData1Provider, m_pData2Provider );
    break;
  case ou::tf::OptionSide::Put:
    m_mapOptions[ strike ].pNearDatePut = new ou::tf::option::Put( pInstrument, m_pData1Provider, m_pData2Provider );
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
    m_mapOptions[ strike ].pFarDateCall = new ou::tf::option::Call( pInstrument, m_pData1Provider, m_pData2Provider );
    break;
  case ou::tf::OptionSide::Put:
    m_mapOptions[ strike ].pFarDatePut = new ou::tf::option::Put( pInstrument, m_pData1Provider, m_pData2Provider );
    break;
  }
}

void StrategyTradeOptions::HandleTrade( const ou::tf::CTrade& trade ) {
  m_trades.Append( trade );
}

void StrategyTradeOptions::HandleQuote( const ou::tf::CQuote& quote ) {
  m_quotes.Append( quote );
}


/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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

/*
 * File:    StrategyEquityOption.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: March 11, 2034  10:58:03
 */

#include <TFIQFeed/OptionChainQuery.h>

#include <TFOptions/Engine.h>

#include <TFVuTrading/TreeItem.hpp>

#include "StrategyEquityOption.hpp"

namespace Strategy {

EquityOption::EquityOption(
  const ou::tf::config::symbol_t& config
, ou::tf::TreeItem* pTreeItem
, fBuildInstrument_t&& fBuildInstrument
, fConstructOption_t&& fConstructOption
, fRegisterOption_t&& fRegisterOption
, fStartCalc_t&& fStartCalc
, fStopCalc_t&& fStopCalc
)
: Base( config, pTreeItem )
, m_nOptionsToProcess {}
, m_pChain( nullptr )
, m_fBuildInstrument( std::move( fBuildInstrument ) )
, m_fConstructOption( std::move( fConstructOption ) )
, m_fRegisterOption( std::move( fRegisterOption ) )
, m_fStartCalc( std::move( fStartCalc ) )
, m_fStopCalc( std::move(  fStopCalc ) )
{
  assert( m_fConstructOption );
}

EquityOption::~EquityOption() {

  while ( !m_mapOptionGreeks.empty() ) {
    StopGreeks( m_mapOptionGreeks.begin()->second );
  }
  m_mapOptionRegistered.clear();

  if ( m_pPosition ) {
    pWatch_t pWatch = m_pPosition->GetWatch();
    pWatch->OnQuote.Remove( MakeDelegate( this, &EquityOption::HandleQuote ) );
    pWatch->OnTrade.Remove( MakeDelegate( this, &EquityOption::HandleTrade ) );
  }

  if ( m_pOptionChainQuery ) {
    m_pOptionChainQuery->Disconnect();
    m_pOptionChainQuery.reset();
  }
}

void EquityOption::SetPosition( pPosition_t pPosition ) {

  assert( pPosition );

  ou::tf::Instrument::pInstrument_t pInstrument( pPosition->GetInstrument() );
  assert( ou::tf::InstrumentType::Stock == pInstrument->GetInstrumentType() );
  const std::string& sIQFeedUnderlying( pInstrument->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

  Base::SetPosition( pPosition );

  pWatch_t pWatch = m_pPosition->GetWatch();
  pWatch->OnQuote.Add( MakeDelegate( this, &EquityOption::HandleQuote ) );
  pWatch->OnTrade.Add( MakeDelegate( this, &EquityOption::HandleTrade ) );

  using query_t = ou::tf::iqfeed::OptionChainQuery;
  m_pOptionChainQuery = std::make_unique<ou::tf::iqfeed::OptionChainQuery>(
    [this,&sIQFeedUnderlying](){
      m_pOptionChainQuery->QueryEquityOptionChain(
        sIQFeedUnderlying, "pc", "", "2", "", "", "",
        [this]( const query_t::OptionList& list ) {
          std::cout << "supplied list for " << list.sUnderlying << " has " << list.vSymbol.size() << " options" << std::endl;
          m_nOptionsToProcess = list.vSymbol.size();
          for ( auto& name: list.vSymbol ) {
            m_fBuildInstrument(
              name,
              [this]( pInstrument_t pInstrument ){
                m_fConstructOption(
                  pInstrument,
                  [this]( pOption_t pOption ){
                    pInstrument_t pInstrument( pOption->GetInstrument() );
                    mapChains_t::iterator iterChain = ou::tf::option::GetChain( m_mapChains, pInstrument );
                    assert( m_mapChains.end() != iterChain );
                    BuiltOption* pBuiltOption = ou::tf::option::UpdateOption<chain_t,BuiltOption>( iterChain->second, pInstrument );
                    assert( pBuiltOption );

                    pBuiltOption->pOption = pOption;
                    assert( 0 < m_nOptionsToProcess );
                    m_nOptionsToProcess--;
                    if ( 0 == m_nOptionsToProcess ) ProcessChains();
                  } );
              } );
          }
        }
      );
    }
  );
  m_pOptionChainQuery->Connect();
}

void EquityOption::ProcessChains() {
  assert( !m_mapChains.empty() );
  mapChains_t::iterator iterChain = m_mapChains.begin();
  boost::gregorian::date current( iterChain->first );
  m_pChain = &iterChain->second;
  const boost::gregorian::date expiry = current + m_config.dte;
  while ( expiry > iterChain->first ) {
    ++iterChain;
    if ( m_mapChains.end() == iterChain ) {
      break;
    }
    else {
      current = iterChain->first;
      m_pChain = &iterChain->second;
    }
  }
  std::cout
    << "chain selected: "
    << m_config.dte << " day, "
    << current << " with " << m_pChain->Size() << " options" << std::endl;
}

void EquityOption::HandleQuote( const ou::tf::Quote& quote ) {
  ptime dt( quote.DateTime() );
  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );
}

void EquityOption::HandleTrade( const ou::tf::Trade& trade ) {
  ptime dt( trade.DateTime() );
  m_ceTrade.Append( dt, trade.Price() );
  m_ceVolume.Append( dt, trade.Volume() );
}

void EquityOption::SaveWatch( const std::string& sPrefix ) {
  Base::SaveWatch( sPrefix );
}

void EquityOption::Signal( ESignal signal ) {
  switch ( signal ) {
    case ESignal::rising:
      std::cout << "EquityOption::Signal rising" << std::endl;
      break;
    case ESignal::neutral:
      std::cout << "EquityOption::Signal neutral" << std::endl;
      break;
    case ESignal::falling:
      std::cout << "EquityOption::Signal falling" << std::endl;
      break;
  }
}

void EquityOption::StartGreeks( pOption_t pOption ) {
  const std::string& sName( pOption->GetInstrumentName() );
  mapOptionRegistered_t::iterator iterRegistry = m_mapOptionRegistered.find( sName );
  if ( m_mapOptionRegistered.end() == iterRegistry ) {
    m_mapOptionRegistered.emplace( mapOptionRegistered_t::value_type( sName, pOption ) );
    m_fRegisterOption( pOption );
  }
  mapOptionGreeks_t::iterator iterGreeks = m_mapOptionGreeks.find( sName );
  if ( m_mapOptionGreeks.end() == iterGreeks ) {
    m_mapOptionGreeks.emplace( mapOptionGreeks_t::value_type( sName, pOption ) );
    m_fStartCalc( pOption, m_pPosition->GetWatch() );
  }
  else {
    std::cout << "option " << sName << " duplicate greeks request" << std::endl;
  }
}

void EquityOption::StopGreeks( pOption_t pOption ) {
  const std::string& sName( pOption->GetInstrumentName() );
  mapOptionGreeks_t::iterator iterGreeks = m_mapOptionGreeks.find( sName );
  if ( m_mapOptionGreeks.end() == iterGreeks ) {
    std::cout << "option " << sName << " no greeks in progress" << std::endl;
  }
  else {
    m_fStopCalc( iterGreeks->second, m_pPosition->GetWatch() );
    m_mapOptionGreeks.erase( iterGreeks );
  }
}

/*
  accessing open interest: auto oi = pOption->GetSummary().nOpenInterest;
*/

} // namespace Strategy

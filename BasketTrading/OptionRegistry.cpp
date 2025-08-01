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
 * File:    OptionRegistry.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrader
 * Created: March 4, 2023 19:42:36
 */

#include <boost/log/trivial.hpp>

#include <TFVuTrading/TreeItem.hpp>

#include "OptionRegistry.hpp"

OptionRegistry::OptionRegistry(
  fStartCalc_t&& fStartCalc
, fStopCalc_t&& fStopCalc
, fSetChartDataView_t&& fSetChartDataView
)
: m_fStartCalc( std::move( fStartCalc ) )
, m_fStopCalc( std::move( fStopCalc ) )
, m_fSetChartDataView( std::move( fSetChartDataView ) )
, m_ptiParent( nullptr )
{
  assert( nullptr != m_fStartCalc );
  assert( nullptr != m_fStopCalc );
}

OptionRegistry::~OptionRegistry() {
  for ( mapOption_t::value_type& vt: m_mapOption ) { // TODO: fix, isn't the best place?
    m_fStopCalc( vt.second.pOption, m_pWatchUnderlying );
    vt.second.pOptionStatistics.reset();
  }
  m_mapOptionRegistered.clear();
  m_mapOption.clear();
}

OptionRegistry::mapOption_t::iterator OptionRegistry::LookUp( pOption_t pOption ) {

  const std::string& sOptionName( pOption->GetInstrumentName() );
  mapOption_t::iterator iterOption = m_mapOption.find( sOptionName );
  if ( m_mapOption.end() == iterOption ) {

    mapOptionRegistered_t::iterator iterRegistry = m_mapOptionRegistered.find( sOptionName );
    if ( m_mapOptionRegistered.end() == iterRegistry ) {
      m_mapOptionRegistered.emplace( mapOptionRegistered_t::value_type( sOptionName, pOption ) );
    }

    auto pair = m_mapOption.emplace( sOptionName, RegistryEntry( pOption ) );
    assert( pair.second );
    iterOption = std::move( pair.first );

    assert( m_pWatchUnderlying );
    m_fStartCalc( pOption, m_pWatchUnderlying );

  }
  else {
    iterOption->second.nReference_option++;
  }

  BOOST_LOG_TRIVIAL(info) << "OptionRegistry::LookUp " << pOption->GetInstrumentName() << "," << iterOption->second.nReference_option;
  return iterOption;
}

void OptionRegistry::Add( pOption_t pOption ) {
  LookUp( pOption );
  //std::cout << "OptionRegistry::Add(simple) " << pOption->GetInstrumentName() << std::endl;
}

void OptionRegistry::Add( pOption_t pOption, const std::string& sLegName ) {
  Add_private( std::move( pOption ), sLegName );
}

void OptionRegistry::Add( pOption_t pOption, pPosition_t pPosition, const std::string& sLegName ) {
  pOptionStatistics_t pOptionStatistics = Add_private( std::move( pOption ), sLegName );
  pOptionStatistics->Set( pPosition );
}

OptionRegistry::pOptionStatistics_t OptionRegistry::Add_private( pOption_t pOption, const std::string& sLegName ) {

  mapOption_t::iterator iterOption = LookUp( pOption );

  pOptionStatistics_t pOptionStatistics;
  if ( iterOption->second.pOptionStatistics ) {
    pOptionStatistics = iterOption->second.pOptionStatistics;
    // TODO: test position is correct?
    BOOST_LOG_TRIVIAL(info)
      << "OptionRegistry::Add " << pOption->GetInstrumentName() << " has named leg " << sLegName  << std::endl;
    // Note: for future fix, the issue here was that a DltaPlsGmPls leg was in place, and the algo wanted to re-use with a SynthLong
    //   ie the two strikes matched as the SynthLong got rolled up
  }
  else {
    pOptionStatistics = OptionStatistics::Factory( pOption );
    //pOptionStatistics->Set( pPosition );
    iterOption->second.pOptionStatistics = pOptionStatistics;

    //OptionStatistics& option_stats( *pOptionStatistics );
    //option_stats.Set( pti );
  }
  iterOption->second.nReference_stats++;
  return pOptionStatistics;
}

void OptionRegistry::Remove( pOption_t pOption, bool bRemoveStatistics ) {

  const std::string& sOptionName( pOption->GetInstrumentName() );
  BOOST_LOG_TRIVIAL(info) << "OptionRegistry::Remove: " << sOptionName;

  mapOption_t::iterator iterOption = m_mapOption.find( sOptionName );

  if ( m_mapOption.end() == iterOption ) {
    BOOST_LOG_TRIVIAL(info) << "OptionRegistry::Remove error, option not found: " << sOptionName;
  }
  else {
    RegistryEntry& entry( iterOption->second );
    assert( 0 != entry.nReference_option );
    entry.nReference_option--;
    if ( 0 == entry.nReference_option ) {
      m_fStopCalc( pOption, m_pWatchUnderlying );
      if ( entry.pOptionStatistics ) {
        if ( !bRemoveStatistics ) {
          BOOST_LOG_TRIVIAL(warning) << "OptionRegistry::Remove - bRemoveStatistics not set";
          bRemoveStatistics = true;
          if ( 1 != entry.nReference_stats ) {
            BOOST_LOG_TRIVIAL(warning) << "OptionRegistry::Remove - entry.nReference_stats not 1 - is " << entry.nReference_stats;
            entry.nReference_stats = 1;
          }
        }
      }
    }
    if ( bRemoveStatistics ) {
      //assert( entry.pOptionStatistics );
      if ( !entry.pOptionStatistics ) {
        BOOST_LOG_TRIVIAL(warning)
          << "OptionRegistry: " << sOptionName << " has no OptionStatistics to remove";
      }
      else {
        assert( 0 < entry.nReference_stats );
        entry.nReference_stats--;
        if ( 0 == entry.nReference_stats ) {
          entry.pOptionStatistics.reset();
        }
      }
    }
    if ( 0 == entry.nReference_option ) {
      //assert( !entry.pOptionStatistics );  // TODO: turn this back on
      m_mapOption.erase( iterOption );
    }
  }
}

OptionRegistry::pChartDataView_t OptionRegistry::ChartDataView( pOption_t pOption ) {
  const std::string& sOptionName( pOption->GetInstrumentName() );
  mapOption_t::iterator iterOption = m_mapOption.find( sOptionName );
  pChartDataView_t pChartDataView;
  if ( m_mapOption.end() == iterOption ) {  }
  else pChartDataView = iterOption->second.pOptionStatistics->ChartDataView();
  return pChartDataView;
}

void OptionRegistry::SaveSeries( const std::string& sPrefix ) {
  for ( mapOptionRegistered_t::value_type& vt: m_mapOptionRegistered ) {
    vt.second->SaveSeries( sPrefix );
  }
}
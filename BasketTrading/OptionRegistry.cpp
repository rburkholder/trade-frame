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
    fRegisterOption_t&& fRegisterOption
  , fStartCalc_t&& fStartCalc
  , fStopCalc_t&& fStopCalc
  , fSetChartDataView_t&& fSetChartDataView
  ) :
    m_fRegisterOption( std::move( fRegisterOption ) )
  , m_fStartCalc( std::move( fStartCalc ) )
  , m_fStopCalc( std::move( fStopCalc ) )
  , m_fSetChartDataView( std::move( fSetChartDataView ) )
  {
    assert( nullptr != m_fStartCalc );
    assert( nullptr != m_fStopCalc );
    assert( nullptr != m_fRegisterOption );
  }

OptionRegistry::~OptionRegistry() {
  for ( mapOption_t::value_type& vt: m_mapOption ) { // TODO: fix, isn't the best place?
    m_fStopCalc( vt.second.pOption, m_pWatchUnderlying );
    vt.second.pOptionStatistics.reset();
  }
  m_mapOptionRegistered.clear();
  m_mapOption.clear();
}

OptionRegistry::mapOption_t::iterator OptionRegistry::Check( pOption_t pOption ) {

  const std::string& sOptionName( pOption->GetInstrument()->GetInstrumentName() );
  mapOption_t::iterator iterOption = m_mapOption.find( sOptionName );
  if ( m_mapOption.end() == iterOption ) {

    auto pair = m_mapOption.emplace( sOptionName, RegistryEntry( pOption ) );
    assert( pair.second );
    iterOption = std::move( pair.first );

    mapOptionRegistered_t::iterator iterRegistry = m_mapOptionRegistered.find( sOptionName );
    if ( m_mapOptionRegistered.end() == iterRegistry ) {
      m_mapOptionRegistered.emplace( mapOptionRegistered_t::value_type( sOptionName, pOption ) );
      try {
        m_fRegisterOption( pOption );
      }
      catch( std::runtime_error& e ) {
        BOOST_LOG_TRIVIAL(info) << "OptionRegistry::Add error: " << e.what();
        // simply telling us we are already registered, convert from error to status?
      }
    }

    assert( m_pWatchUnderlying );
    m_fStartCalc( pOption, m_pWatchUnderlying );

  }
  else {
    iterOption->second.nReference++;
  }

  BOOST_LOG_TRIVIAL(info) << "OptionRegistry::Add " << pOption->GetInstrumentName();
  return iterOption;
}

void OptionRegistry::Add( pOption_t pOption ) {
  Check( pOption );
  //std::cout << "OptionRegistry::Add(simple) " << pOption->GetInstrumentName() << std::endl;
}

void OptionRegistry::Add( pOption_t pOption, pPosition_t pPosition, const std::string& sLegName, ou::tf::option::Combo::vMenuActivation_t&& ma ) {

  const std::string& sOptionName( pOption->GetInstrument()->GetInstrumentName() );

  pOptionStatistics_t pOptionStatistics = OptionStatistics::Factory( pOption );
  OptionStatistics& ostats( *pOptionStatistics );

  ou::tf::TreeItem* pti = m_ptiParent->AppendChild(
    pOption->GetInstrumentName() + " (" + sLegName + ")",
    [this,&ostats]( ou::tf::TreeItem* ){
      m_fSetChartDataView( ostats.ChartDataView() );
    },
    [this,&sOptionName, ma_=std::move(ma)]( ou::tf::TreeItem* pti ) {
      pti->NewMenu();
      for ( const ou::tf::option::Combo::vMenuActivation_t::value_type& vt: ma_  ) {
        pti->AppendMenuItem(
          vt.sLabel,
          //[this,&sOptionName,ma_f=std::move( vt.fMenuActivation )]( ou::tf::TreeItem* pti ){
          [this,&sOptionName,ma_f=&vt.fMenuActivation]( ou::tf::TreeItem* pti ){
            (*ma_f)();
          });
      }
    }
  );
  ostats.Set( pti );
  ostats.Set( pPosition );

  mapOption_t::iterator iterOption = Check( pOption );
  assert( !iterOption->second.pOptionStatistics );
  iterOption->second.pOptionStatistics = std::move ( pOptionStatistics );

  //std::cout << "OptionRegistry::Add(stats) " << pOption->GetInstrumentName() << std::endl;

}

void OptionRegistry::Remove( pOption_t pOption, bool bRemoveStatistics ) {

  const std::string& sOptionName( pOption->GetInstrument()->GetInstrumentName() );
  BOOST_LOG_TRIVIAL(info) << "OptionRegistry::Remove: " << sOptionName;

  mapOption_t::iterator iterOption = m_mapOption.find( sOptionName );

  if ( m_mapOption.end() != iterOption ) {
    RegistryEntry& entry( iterOption->second );
    assert( 0 != entry.nReference );
    entry.nReference--;
    if ( 0 == entry.nReference ) {
      m_fStopCalc( pOption, m_pWatchUnderlying );
    }
    if ( bRemoveStatistics ) {
      assert( entry.pOptionStatistics );
      entry.pOptionStatistics.reset();
    }
    if ( 0 == entry.nReference ) {
      assert( !entry.pOptionStatistics );
      m_mapOption.erase( iterOption );
    }
  }
  else {
    BOOST_LOG_TRIVIAL(info) << "OptionRegistry::Remove error, option not found: " << sOptionName;
  }

}

/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <stdexcept>

#include <TFIQFeed/Provider.h>
//#include <TFAlpaca/Provider.hpp>
#include <TFInteractiveBrokers/IBTWS.h>
#include <TFSimulation/SimulationProvider.hpp>

#include "ProviderManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

ProviderManager::pProvider_t ProviderManager::Construct( const idProvider_t& key, keytypes::eidProvider_t type ) {
  pProvider_t pProvider;
  switch ( type ) {
  case keytypes::EProviderIB:
    pProvider = Construct<ou::tf::ib::TWS>( key );
    break;
  case keytypes::EProviderIQF:
    pProvider = Construct<ou::tf::iqfeed::Provider>( key );
    break;
  case keytypes::EProviderSimulator:
    pProvider = Construct<SimulationProvider>( key );
    break;
  //case keytypes::EProviderAlpaca: // use manual registration instead
  //  pProvider = Construct<ou::tf::alpaca::Provider>( key );
  //  break;
  case keytypes::EProviderGNDT:
    throw std::runtime_error( "GNDT not implemented" );
    break;
  }
  return pProvider;
}

void ProviderManager::Register( const idProvider_t& key, pProvider_t pProvider ) {
  if ( m_mapProviders.end() != m_mapProviders.find( key ) ) {
    throw std::runtime_error( "ProviderManager::Register, provider already exists" );
  }
  pProvider->SetName( key );
  m_mapProviders.insert( mapProviders_pair_t( key, pProvider ) );
}

void ProviderManager::Register( pProvider_t pProvider ) {

  const idProvider_t& key( pProvider->GetName() );
  assert( 0 < key.size() );

  if ( m_mapProviders.end() == m_mapProviders.find( key ) ) {
    m_mapProviders.insert( mapProviders_pair_t( key, pProvider ) );
  }
  else {
    throw std::runtime_error( "ProviderManager::Register, provider already exists" );
  }
}

void ProviderManager::Release( const idProvider_t& key ) {

  iterProviders_t iter = m_mapProviders.find( key );
  if ( m_mapProviders.end() == iter ) {
    throw std::runtime_error( "ProviderManager::Release, provider does not exist" );
  }
  m_mapProviders.erase( iter );
}

ProviderManager::pProvider_t ProviderManager::Get( const idProvider_t& key ) {

  iterProviders_t iter = m_mapProviders.find( key );
  if ( m_mapProviders.end() == iter ) {
    throw std::runtime_error( "ProviderManager::Get, provider does not exist" );
  }
  return iter->second;
}

} // namespace tf
} // namespace ou

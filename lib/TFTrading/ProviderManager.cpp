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

#include "StdAfx.h"

#include "ProviderManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

void CProviderManager::Register( const idProvider_t& key, pProvider_t pProvider ) {

  if ( m_mapProviders.end() == m_mapProviders.find( key ) ) {
    throw std::runtime_error( "CProviderManager::Register already exists" );
  }
  m_mapProviders.insert( mapProviders_pair_t( key, pProvider ) );

}

void CProviderManager::Release( const idProvider_t& key ) {

  iterProviders_t iter = m_mapProviders.find( key );
  if ( m_mapProviders.end() == iter ) {
    throw std::runtime_error( "CProviderManager::Release, provider does not exist" );
  }
  m_mapProviders.erase( iter );
}

CProviderManager::pProvider_t CProviderManager::Get( const idProvider_t& key ) {

  iterProviders_t iter = m_mapProviders.find( key );
  if ( m_mapProviders.end() == iter ) {
    throw std::runtime_error( "CProviderManager::Release, provider does not exist" );
  }
  return iter->second;
}

} // namespace tf
} // namespace ou

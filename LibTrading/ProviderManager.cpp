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

CProviderManager::CProviderManager(void) {
}

CProviderManager::~CProviderManager(void) {
}

void CProviderManager::Register( const std::string& sKey, pProvider_t pProvider ) {

  if ( m_mapProviders.end() == m_mapProviders.find( sKey ) ) {
    throw std::runtime_error( "CProviderManager::Register already exists" );
  }
  m_mapProviders.insert( providers_pair_t( sKey, pProvider ) );

}

} // namespace tf
} // namespace ou

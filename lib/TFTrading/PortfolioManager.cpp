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

#include "PortfolioManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

CPortfolioManager::pPortfolio_t CPortfolioManager::Create( const idPortfolio_t& sName ) {
  pPortfolio_t pPortfolio;
  iterPortfolio_t iter = m_mapPortfolio.find( sName );
  if ( m_mapPortfolio.end() != iter ) {
    throw std::runtime_error( "CPortfolioManager::Create, portfolio already exists" );
  }
  else {
    pPortfolio.reset( new CPortfolio( sName ) );
    m_mapPortfolio.insert( mapPortfolio_pair_t( sName, pPortfolio ) );
  }
  return pPortfolio;
}

CPortfolioManager::pPortfolio_t CPortfolioManager::GetPortfolio( const idPortfolio_t& sName, bool bCreate ) {
  pPortfolio_t pPortfolio;
  iterPortfolio_t iter = m_mapPortfolio.find( sName );
  if ( m_mapPortfolio.end() == iter ) {
    if ( bCreate ) {
      pPortfolio.reset( new CPortfolio( sName ) );
      m_mapPortfolio.insert( mapPortfolio_pair_t( sName, pPortfolio ) );
    }
    else {
      throw std::runtime_error( "CPortfolioManager::GetPortfolio, portfolio does not exist" );
    }
  }
  else {
    pPortfolio = iter->second;
  }
  
  return pPortfolio;
}

void CPortfolioManager::Delete( const idPortfolio_t& sName ) {
  iterPortfolio_t iter = m_mapPortfolio.find( sName );
  if ( m_mapPortfolio.end() == iter ) {
    throw std::runtime_error( "CPortfolioManager::Delete, portfolio does not exist" );
  }
  m_mapPortfolio.erase( iter );
}

} // namespace tf
} // namespace ou

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

CPortfolioManager::CPortfolioManager(void) {
}

CPortfolioManager::~CPortfolioManager(void) {
}

CPortfolioManager::pPortfolio_t CPortfolioManager::Create( const std::string& sName ) {
  pPortfolio_t pPortfolio;
  iterator iter = m_mapPortfolios.find( sName );
  if ( m_mapPortfolios.end() != iter ) {
    throw std::runtime_error( "CPortfolioManager::Create, portfolio already exists" );
  }
  else {
    pPortfolio.reset( new CPortfolio( sName ) );
    m_mapPortfolios.insert( m_mapPortfolios_pair( sName, pPortfolio ) );
  }
  return pPortfolio;
}

CPortfolioManager::pPortfolio_t CPortfolioManager::GetPortfolio( const std::string& sName, bool bCreate ) {
  pPortfolio_t pPortfolio;
  iterator iter = m_mapPortfolios.find( sName );
  if ( m_mapPortfolios.end() == iter ) {
    if ( bCreate ) {
      pPortfolio.reset( new CPortfolio( sName ) );
      m_mapPortfolios.insert( m_mapPortfolios_pair( sName, pPortfolio ) );
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

void CPortfolioManager::Delete( const std::string& sName ) {
  iterator iter = m_mapPortfolios.find( sName );
  if ( m_mapPortfolios.end() == iter ) {
    throw std::runtime_error( "CPortfolioManager::Delete, portfolio does not exist" );
  }
  m_mapPortfolios.erase( iter );
}


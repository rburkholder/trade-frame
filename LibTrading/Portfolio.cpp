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

#include "Portfolio.h"

CPortfolio::CPortfolio( const std::string &sPortfolioName ) 
: m_sPortfolioName( sPortfolioName )
{
}

CPortfolio::~CPortfolio(void) {
}

void CPortfolio::Add( const std::string &sName, pPosition_t pPosition ) {
  iterator iter = m_mapPositions.find( sName );
  if ( m_mapPositions.end() != iter ) {
    throw std::runtime_error( "CPortfolio::Add position already exists" );
  }
  else {
    m_mapPositions.insert( map_t_pair( sName, pPosition ) );
  }
}

void CPortfolio::Delete( const std::string& sName ) {
}

void CPortfolio::Rename( const std::string& sName ) {
}

CPortfolio::pPosition_t CPortfolio::GetPosition( const std::string& sName ) {
}

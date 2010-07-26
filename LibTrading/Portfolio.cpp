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

void CPortfolio::AddPosition( const std::string &sName, pPosition_t pPosition ) {
  iterator iter = m_mapPositions.find( sName );
  if ( m_mapPositions.end() != iter ) {
    throw std::runtime_error( "CPortfolio::Add position already exists" );
  }
  else {
    m_mapPositions.insert( map_t_pair( sName, pPosition ) );
  }
}

void CPortfolio::DeletePosition( const std::string& sName ) {
  iterator iter = m_mapPositions.find( sName );
  if ( m_mapPositions.end() == iter ) {
    throw std::runtime_error( "CPortfolio::Delete Portfolio does not exist" );
  }
  else {
    m_mapPositions.erase( iter );
  }
}

void CPortfolio::RenamePosition( const std::string& sOld, const std::string& sNew ) {

  iterator iter;

  iter = m_mapPositions.find( sNew );
  if ( m_mapPositions.end() == iter ) {
    throw std::runtime_error( "CPortfolio::Rename New Portfolio already exists" );
  }
  iter = m_mapPositions.find( sOld );
  if ( m_mapPositions.end() == iter ) {
    throw std::runtime_error( "CPortfolio::Rename Old Portfolio does not exist" );
  }

  pPosition_t pPosition( iter->second );
  m_mapPositions.erase( iter );
  m_mapPositions.insert( map_t_pair( sNew, pPosition ) );
}

CPortfolio::pPosition_t CPortfolio::GetPosition( const std::string& sName ) {

  iterator iter = m_mapPositions.find( sName );

  if ( m_mapPositions.end() == iter ) {
    throw std::runtime_error( "CPortfolio::GetPosition Portfolio does not exist" );
  }

  return iter->second;
}

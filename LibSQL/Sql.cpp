/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <stdexcept>

#include "Sql.h"

namespace ou {
namespace db {

//
// =====
//

CSqlBase::CSqlBase( IDatabase& db )
  : m_db( db ), m_pStatement( 0 )
{
}

CSqlBase::~CSqlBase(void) {
  if ( 0 != m_pStatement ) {
    m_db.CloseStatement( *m_pStatement );
    m_pStatement = 0;
  }
}

void CSqlBase::PrepareStatement( void ) {

  IDatabase::structStatement& statement = m_db.AllocateStatement();
  m_pStatement = &statement;

  ComposeStatement( m_pStatement->sSqlStatement );

  if ( m_pStatement->sSqlStatement.empty() ) {
    throw std::runtime_error( "CSqlBase::PrepareStatement has empty statement" );
  }

  m_db.PrepareStatement( statement );

}

void CSqlBase::ComposeStatement( std::string& sStatement ) {
}

void CSqlBase::ExecuteStatement( void ) {
  if ( 0 == m_pStatement ) {
    throw std::runtime_error( "CSqlBase::ExecuteStatement has no prepared statement" );
  }
  m_db.ExecuteStatement( *m_pStatement );
}

//
// =====
//


} // db
} // ou

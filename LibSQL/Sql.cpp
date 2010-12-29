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
  : m_bPrepared( false ), m_db( db )
{
}

CSqlBase::~CSqlBase(void) {
}

void CSqlBase::PrepareStatement( void ) {

  IDatabase::structStatement& statement = m_db.AllocateStatement();
  m_pStatement = &statement;

  ComposeStatement( statement.sSqlStatement );

  if ( statement.sSqlStatement.empty() ) {
    throw std::runtime_error( "CSqlBase::PrepareStatement has empty statement" );
  }

  if ( m_bPrepared ) {
  }
  else {
  }

}

void CSqlBase::ComposeStatement( std::string& sStatement ) {
  throw std::runtime_error( "CSqlBase::ComposeStatement is not overloaded" );
}

//
// =====
//


} // db
} // ou

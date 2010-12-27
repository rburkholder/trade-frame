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

CSqlBase::CSqlBase(void): m_bPrepared( false ) {
}


CSqlBase::~CSqlBase(void) {
}

void CSqlBase::PrepareStatement( void ) {

  m_sSqlStatement.clear();

  ComposeStatement( m_sSqlStatement );

  if ( m_sSqlStatement.empty() ) {
    throw std::runtime_error( "CSqlBase::PrepareStatement has empty statement" );
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

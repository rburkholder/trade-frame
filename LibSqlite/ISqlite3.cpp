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

#include <stdexcept>
#include <cassert>

#include <boost/lexical_cast.hpp>

#include "ISqlite3.h"


namespace ou {
namespace db {

ISqlite3::ISqlite3(void) 
  : m_db( 0 )
{
}

ISqlite3::~ISqlite3(void) {
}

void ISqlite3::Open( const std::string& sDbFileName, enumOpenFlags flags ) {

  int sqlite3_flags = SQLITE_OPEN_READWRITE;
  sqlite3_flags |= ( 0 < ( flags & EOpenFlagsAutoCreate ) ) ? SQLITE_OPEN_CREATE : 0;

  int rtn = sqlite3_open_v2( sDbFileName.c_str(), &m_db, sqlite3_flags, 0 );
  if ( SQLITE_OK != rtn ) {
    m_db = 0;
    throw std::runtime_error( "Db open error" );
  }

  m_sDbFileName = sDbFileName;
  m_bDbOpened = true;

}

void ISqlite3::Close( void ) {
  if ( m_bDbOpened ) {
    int rtn = sqlite3_close( m_db );
    m_db = 0;
    m_bDbOpened = false;
    assert( SQLITE_OK == rtn );
  }
}

void ISqlite3::PrepareStatement( structStatementState& statement, const std::string& sStatement ) {
  IDatabaseCommon<structStatementState>::structStatementControl* psc =
    reinterpret_cast<IDatabaseCommon<structStatementState>::structStatementControl*>( &statement );
  int rtn = sqlite3_prepare_v2( 
    m_db, sStatement.c_str(), -1, &psc->stateStatement.pStmt, NULL );
  if ( SQLITE_OK != rtn ) {
    std::string sErr( "ISqlite3::PrepareStatement: " );
    sErr += " error in prepare(";
    sErr += boost::lexical_cast<std::string>( rtn );
    sErr += ")";
    throw std::runtime_error( sErr );
  }
  assert( 0 != psc->stateStatement.pStmt );
}

void ISqlite3::ExecuteStatement( structStatementState& statement ) {
  IDatabaseCommon<structStatementState>::structStatementControl* psc =
    reinterpret_cast<IDatabaseCommon<structStatementState>::structStatementControl*>( &statement );
  if ( 0 != psc->stateStatement.pStmt ) {
    int rtn = sqlite3_step( psc->stateStatement.pStmt );
    if ( SQLITE_DONE != rtn ) {
      std::string sErr( "ISqlite3::ExecuteStatement: " );
      sErr += " error in execute(";
      sErr += boost::lexical_cast<std::string>( rtn );
      sErr += ")";
      throw std::runtime_error( sErr );
    }
  }
}

void ISqlite3::CloseStatement( structStatementState& statement ) {
  IDatabaseCommon<structStatementState>::structStatementControl* psc =
    reinterpret_cast<IDatabaseCommon<structStatementState>::structStatementControl*>( &statement );
  if ( 0 != psc->stateStatement.pStmt ) { // it shouldn't be zero, but test anyway
    int rtn = sqlite3_finalize( psc->stateStatement.pStmt );
    if ( SQLITE_OK != rtn ) {
      std::string sErr( "ISqlite3::CloseStatement: " );
      sErr += " error in close(";
      sErr += boost::lexical_cast<std::string>( rtn );
      sErr += ")";
      throw std::runtime_error( sErr );
    }
    psc->stateStatement.pStmt = 0;
    // todo:  should the list element also be deleted here?
  }
}

} // db
} // ou

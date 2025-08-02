/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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
  : m_db( nullptr )
{
}

ISqlite3::~ISqlite3(void) {
}

void ISqlite3::SessionOpen( const std::string& sDbFileName, enumOpenFlags flags ) {

  //https://sqlite.org/threadsafe.html, https://sqlite.org/c3ref/open.html
  int sqlite3_flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX;
  sqlite3_flags |= ( 0 < ( flags & EOpenFlagsAutoCreate ) ) ? SQLITE_OPEN_CREATE : 0;

  int rtn = sqlite3_open_v2( sDbFileName.c_str(), &m_db, sqlite3_flags, 0 );
  if ( SQLITE_OK != rtn ) {
    m_db = nullptr;
    throw std::runtime_error( "Db open error" );
  }

}

void ISqlite3::SessionClose( void ) {
  if ( nullptr != m_db ) {
    int rtn = sqlite3_close( m_db );
    m_db = nullptr;
    if (  SQLITE_OK != rtn ) {
      if ( SQLITE_BUSY == rtn ) {
        std::cout << "sqlite busy" << std::endl;  // prepared statements need to be closed
      }
      else {
        assert( false );
      }
    }
  }
}

void ISqlite3::PrepareStatement( structStatementState& statement, std::string& sStatement ) {
  sStatement += ";";
  int rtn = sqlite3_prepare_v2(
    m_db, sStatement.c_str(), -1, &statement.pStmt, NULL );
  if ( SQLITE_OK != rtn ) {
    std::string sErr( "ISqlite3::PrepareStatement: " );
    sErr += " error in prepare(";
    sErr += boost::lexical_cast<std::string>( rtn );
    sErr += ")";
    throw std::runtime_error( sErr );
  }
  assert( nullptr != statement.pStmt );
}

bool ISqlite3::ExecuteStatement( structStatementState& statement ) {
  bool bRow = false;  // false, no rows, true, one or more rows
  if ( nullptr != statement.pStmt ) {
    statement.bIsReset = false;
    int rtn = sqlite3_step( statement.pStmt );
    switch ( rtn ) {
      // todo:  create different runtime error objects for each exception
    case SQLITE_DONE: {  // will need reset for subsequent re-use
      // nothing to do, and no rows returned
      }
      break;
    case SQLITE_ROW: {  // returned each time a new row of data is ready
      bRow = true;
      }
      break;
    case SQLITE_BUSY: {
      // no transaction:  retry
      // transaction and no commit: rollback
      // transaction and commit: retry
      std::string sErr( "ISqlite3::ExecuteStatement: " );
      sErr += " busy";
      throw std::runtime_error( sErr );
      }
      break;
    case SQLITE_ERROR: {
      std::string sErr( "ISqlite3::ExecuteStatement: " );
      sErr += " error";
      throw std::runtime_error( sErr );
      }
      break;
    case SQLITE_MISUSE: {
      std::string sErr( "ISqlite3::ExecuteStatement: " );
      sErr += " misuse";
      throw std::runtime_error( sErr );
      }
      break;
    case SQLITE_CONSTRAINT: {
      std::string sErr( "ISqlite3::ExecuteStatement: " );
      sErr += " constraint, record probably exists";
      throw std::runtime_error( sErr );
      }
    default: {
      std::string sErr( "ISqlite3::ExecuteStatement: " );
      sErr += " unknown error in execute(";
      sErr += boost::lexical_cast<std::string>( rtn );
      sErr += ")";
      throw std::runtime_error( sErr );
      }
    }
  }
  return bRow;
}

void ISqlite3::ResetStatement( structStatementState& statement ) {
  if ( nullptr != statement.pStmt ) {
    int rtn = sqlite3_reset( statement.pStmt );
    if ( SQLITE_OK != rtn ) {
      std::string sErr( "ISqlite3::ResetStatement: " );
      sErr += " error in reset(";
      sErr += boost::lexical_cast<std::string>( rtn );
      sErr += ")";
      throw std::runtime_error( sErr );
    }
    statement.bIsReset = true;
  }
}

void ISqlite3::CloseStatement( structStatementState& statement ) {
  if ( nullptr != statement.pStmt ) { // it shouldn't be zero, but test anyway
    int rtn = sqlite3_finalize( statement.pStmt );
    if ( SQLITE_OK != rtn ) {
      std::string sErr( "ISqlite3::CloseStatement: " );
      sErr += " error in close(";
      sErr += boost::lexical_cast<std::string>( rtn );
      sErr += ")";
      throw std::runtime_error( sErr );
    }
    statement.pStmt = nullptr;
    // todo:  should the list element also be deleted here?
  }
}

} // db
} // ou

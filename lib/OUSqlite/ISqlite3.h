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

#pragma once

#include <string>

//#include <OUSQL/SessionBase.h>  // for the enumerations
#include <OUSQL/Constants.h>

#define SQLITE_DEFAULT_FOREIGN_KEYS 1
#define SQLITE_DEFAULT_FILE_FORMAT 4

#include "sqlite3.h"
#include "StatementState.h"
#include "Actions.h"

namespace ou {
namespace db {

// needs to be called as db: public ISqlite3, SessionBase<db, ISqlite3>

class ISqlite3 {
public:

  typedef sqlite::structStatementState structStatementState;

  typedef ou::db::sqlite::Action_Assemble_TableDef Action_Assemble_TableDef;
  typedef ou::db::Action_Compose_Insert Action_Compose_Insert;
  typedef ou::db::Action_Compose_Update Action_Compose_Update;
  typedef ou::db::Action_Compose_Delete Action_Compose_Delete;
  typedef ou::db::sqlite::Action_Extract_Columns Action_Extract_Columns;

  typedef ou::db::sqlite::Action_Bind_Values Action_Bind_Values;

  ISqlite3(void);
  ~ISqlite3(void);

  void SessionOpen( const std::string& sDbFileName, enumOpenFlags = EOpenFlagsZero );
  void SessionClose( void );

  void PrepareStatement( structStatementState& statement, std::string& sStatement );
  bool ExecuteStatement( structStatementState& statement );  // true when row available
  void ResetStatement(   structStatementState& statement );
  void CloseStatement(   structStatementState& statement );

  boost::int64_t GetLastRowId( void ) {
    return sqlite3_last_insert_rowid( m_db );
  }

protected:

private:

  sqlite3* m_db;

};

} // db
} // ou

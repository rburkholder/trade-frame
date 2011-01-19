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

#include <LibSQL/Database.h>

#include "sqlite3.h"
#include "StatementState.h"
#include "Actions.h"

namespace ou {
namespace db {

class ISqlite3: public Database {
public:

  typedef sqlite::structStatementState structStatementState;

  typedef ou::db::sqlite::Action_Assemble_TableDef Action_Assemble_TableDef;
  typedef ou::db::Action_Compose_Insert Action_Compose_Insert;
  typedef ou::db::Action_Compose_Update Action_Compose_Update;
  typedef ou::db::Action_Compose_Delete Action_Compose_Delete;

  typedef ou::db::sqlite::Action_Bind_Values Action_Bind_Values;

  ISqlite3(void);
  ~ISqlite3(void);

  void Open( const std::string& sDbFileName, enumOpenFlags = EOpenFlagsZero );
  void Close( void );

  void PrepareStatement( structStatementState& statement, std::string& sStatement );
  void ExecuteStatement( structStatementState& statement );
  void ResetStatement(   structStatementState& statement );
  void CloseStatement(   structStatementState& statement );

protected:

private:

  sqlite3* m_db;

};

} // db
} // ou

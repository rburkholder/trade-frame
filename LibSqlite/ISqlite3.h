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

#include <LibSQL/IDatabase.h>

#include "Actions.h"
#include "sqlite3.h"

namespace ou {
namespace db {
namespace sqlite {

struct structStatementState {
  sqlite3_stmt* pStmt;
  structStatementState( void ) : pStmt( 0 ) {};
};

} // namespace sqlite

class ISqlite3: public IDatabaseCommon<sqlite::structStatementState> {
public:

  typedef sqlite::structStatementState structStatementState;

  typedef ou::db::sqlite::Action_Compose_CreateTable Action_Compose_CreateTable;

  ISqlite3(void);
  ~ISqlite3(void);

  void Open( const std::string& sDbFileName, enumOpenFlags = EOpenFlagsZero );
  void Close( void );

  void PrepareStatement( structStatement& statement );
  void ExecuteStatement( structStatement& statement );
  void CloseStatement( structStatement& statement );

protected:

private:

  sqlite3* m_db;

};

} // db
} // ou

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

#include <LibSqlite/sqlite3.h>

#include "IDatabase.h"

namespace ou {
namespace db {

struct structStatementState {
  sqlite3_stmt* pStmt;
  structStatementState( void ) : pStmt( 0 ) {};
};

class ISqlite3: public IDatabaseCommon<structStatementState> {
public:

  ISqlite3(void);
  ~ISqlite3(void);

  void Open( const std::string& sDbFileName, enumOpenFlags = EOpenFlagsZero );
  void Close( void );

protected:

private:

  sqlite3* m_db;

};

} // db
} // ou

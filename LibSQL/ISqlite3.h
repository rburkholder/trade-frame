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
#include <list>

#include <LibSqlite/sqlite3.h>

#include "IDatabase.h"

namespace ou {
namespace db {

class ISqlite3: public IDatabase {
public:

  struct structStmt {
    sqlite3_stmt* pStmt;
    structStmt( void ) : pStmt( 0 ) {};
  };

  // maintain list of statements submitted to database
  // caller holds an iterator as a handle
  typedef std::list<structStmt> lStmt_t;
  typedef lStmt_t::iterator lStmt_iter_t;
  lStmt_t m_lStmt;

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

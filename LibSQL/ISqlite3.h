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

#include <boost/cstdint.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <LibSqlite/sqlite3.h>

#include "IDatabase.h"
#include "FieldDef.h"

namespace ou {
namespace db {

struct structStatementState {
  sqlite3_stmt* pStmt;
  structStatementState( void ) : pStmt( 0 ) {};
};

class Action_Sqlite_AddFields: public Action_AddFields {
public:

  const char* FieldType( char key );
  const char* FieldType( bool key );
  const char* FieldType( boost::int64_t key );
  const char* FieldType( boost::int32_t key );
  const char* FieldType( boost::int16_t key );
  const char* FieldType( boost::int8_t key );
  const char* FieldType( std::string& key );
  const char* FieldType( double key );
  const char* FieldType( boost::posix_time::ptime& key ); // don't use julian as ptime has no representation earlier than 1400 AD

  Action_Sqlite_AddFields( void ) {};
  ~Action_Sqlite_AddFields( void ) {};

  template<typename T>
  void registerField( const std::string& sFieldName, T& var ) {
    addField( sFieldName, FieldType( var ) );
  }

  template<typename T>
  void registerField( const std::string& sFieldName, T& var, const std::string& sFieldType ) {
    addField( sFieldName, sFieldType.c_str() );
  }

protected:
private:
};

class ISqlite3: public IDatabaseCommon<structStatementState> {
public:

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

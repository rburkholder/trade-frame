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

// Library: LibSqlite/Actions.cpp

#include <sstream>

#include "Actions.h"

namespace ou {
namespace db {
namespace sqlite {

void Action_Assemble_TableDef::Key( const std::string& sFieldName ) {
  // change this to a boost integral type check to catch all integer types?
  if ( "INT8" == m_vFieldDef.begin()->sFieldType ) {  // proper way to get oid and key to be the same
    m_vFieldDef.begin()->sFieldType = "INTEGER";
  }
  ou::db::Action_Assemble_TableDef::Key( sFieldName );
}

// 
// bind values for query
//

int Action_Bind_Values::Bind( char var ) {
  return sqlite3_bind_int( m_state.pStmt, m_index, var );
}

int Action_Bind_Values::Bind( bool var ) {
  return sqlite3_bind_int( m_state.pStmt, m_index, var ? 1 : 0 );
}

int Action_Bind_Values::Bind( boost::int64_t var ) {
  return sqlite3_bind_int64( m_state.pStmt, m_index, var );
}

int Action_Bind_Values::Bind( boost::int32_t var ) {
  return sqlite3_bind_int( m_state.pStmt, m_index, var );
}

int Action_Bind_Values::Bind( boost::int16_t var ) {
  return sqlite3_bind_int( m_state.pStmt, m_index, var );
}

int Action_Bind_Values::Bind( boost::int8_t var ) {
  return sqlite3_bind_int( m_state.pStmt, m_index, var );
}

int Action_Bind_Values::Bind( std::string& var ) {
  return sqlite3_bind_text( m_state.pStmt, m_index, var.c_str(), var.length(), SQLITE_TRANSIENT );
}

int Action_Bind_Values::Bind( double var ) {
  return sqlite3_bind_double( m_state.pStmt, m_index, var );
}

// http://www.boost.org/doc/libs/1_45_0/doc/html/date_time/date_time_io.html#date_time.io_tutorial
int Action_Bind_Values::Bind( boost::posix_time::ptime& var ) {
  // need to fix up with local time
  std::stringstream ss;
  ss << var;
  return Bind( ss.str() );
}

//
// column extraction
// todo: validate column type matches requested type
//

void Action_Extract_Columns::Column( bool& var ) {
  var = sqlite3_column_int( m_state.pStmt, m_index ) == 0 ? false : true;
}

void Action_Extract_Columns::Column( char& var ) {
  var = sqlite3_column_int( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( boost::int64_t& var ) {
  var = sqlite3_column_int64( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( boost::int32_t& var ) {
  var = sqlite3_column_int( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( boost::int16_t& var ) {
  var = sqlite3_column_int( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( boost::int8_t& var ) {
  var = sqlite3_column_int( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( std::string& var ) {
  var.assign( reinterpret_cast<const char*>( sqlite3_column_text( m_state.pStmt, m_index ) ) );
}

void Action_Extract_Columns::Column( double& var ) {
  var = sqlite3_column_double( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( boost::posix_time::ptime& var ) {
  std::stringstream ss;
  Column( ss.str() );
  ss >> var;
}

} // sqlite
} // db
} // ou

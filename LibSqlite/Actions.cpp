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

// LibSqlite/Actions.cpp

#include <sstream>

#include "Actions.h"

namespace ou {
namespace db {
namespace sqlite {

// 
// determine field types
//

const char* Action_Assemble_TableDef::FieldType( char key ) {
  return "TINYINT";
}

const char* Action_Assemble_TableDef::FieldType( bool key ) {
  return "TINYINT";
}

const char* Action_Assemble_TableDef::FieldType( boost::int64_t key ) {
  return "INT8"; 
}

const char* Action_Assemble_TableDef::FieldType( boost::int32_t key ) {
  return "BIGINT";
}

const char* Action_Assemble_TableDef::FieldType( boost::int16_t key ) {
  return "SMALLINT";
}

const char* Action_Assemble_TableDef::FieldType( boost::int8_t key ) {
  return "TINYINT";
}

const char* Action_Assemble_TableDef::FieldType( std::string& key ) {
  return "TEXT";
}

const char* Action_Assemble_TableDef::FieldType( double key ) {
  return "DOUBLE";
}

// don't use julian as ptime has no representation earlier than 1400 AD
const char* Action_Assemble_TableDef::FieldType( boost::posix_time::ptime& key ) { 
  return "TEXT";
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
//

void Action_Extract_Columns::Column( char& var ) {
  var = sqlite3_column_int( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( bool& var ) {
  var = sqlite3_column_int( m_state.pStmt, m_index ) == 0 ? false : true;
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

//
// unused, untested stuff, needs fix on enum stuff
//
template<>
const char* FieldType2<char>( void ) { return "TINYINT"; };

template<> // don't use julian as ptime has no representation earlier than 1400 AD
const char* FieldType2<boost::posix_time::ptime>( void ) { return "TEXT"; }

template<>
const char* FieldType2<bool>( void ) { return "TINYINT"; }

template<>
const char* FieldType2<boost::int64_t>( void ) { return "INT8"; }

template<>
const char* FieldType2<boost::int32_t>( void ) { return "BIGINT"; }

template<>
const char* FieldType2<boost::int16_t>( void ) { return "SMALLINT"; }

template<>
const char* FieldType2<boost::int8_t>( void ) { return "TINYINT"; }

template<>
const char* FieldType2<std::string>( void ) { return "TEXT"; }

template<>
const char* FieldType2<double>( void ) { return "DOUBLE"; }




} // sqlite
} // db
} // ou

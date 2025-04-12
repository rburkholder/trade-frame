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

// Library: LibSqlite/Actions.cpp

#include <sstream>

#include "Actions.h"

namespace ou {
namespace db {
namespace sqlite {

namespace dispatch {

template<> const char* FieldType<char>( void ) { return "TINYINT"; };
template<> const char* FieldType<bool>( void ) { return "TINYINT"; };
template<> const char* FieldType<boost::int64_t>( void ) { return "INT8"; };
template<> const char* FieldType<boost::uint64_t>( void ) { return "INT8"; };
template<> const char* FieldType<boost::int32_t>( void ) { return "BIGINT"; };
template<> const char* FieldType<boost::uint32_t>( void ) { return "INT8"; };
template<> const char* FieldType<boost::int16_t>( void ) { return "SMALLINT"; };
template<> const char* FieldType<boost::uint16_t>( void ) { return "BIGINT"; };
template<> const char* FieldType<boost::int8_t>( void ) { return "TINYINT"; };
template<> const char* FieldType<boost::uint8_t>( void ) { return "SMALLINT"; };
template<> const char* FieldType<std::string>( void ) { return "TEXT"; };
template<> const char* FieldType<double>( void ) { return "DOUBLE"; };
// don't use julian as ptime has no representation earlier than 1400 AD
template<> const char* FieldType<boost::posix_time::ptime>( void ) { return "TEXT"; };
template<> const char* FieldType<boost::gregorian::date>() { return "TEXT"; }

} // namespace dispatch

//

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

int Action_Bind_Values::Bind( boost::uint64_t var ) {
  return sqlite3_bind_int64( m_state.pStmt, m_index, var );
}

int Action_Bind_Values::Bind( boost::int32_t var ) {
  return sqlite3_bind_int( m_state.pStmt, m_index, var );
}

int Action_Bind_Values::Bind( boost::uint32_t var ) {
  return sqlite3_bind_int64( m_state.pStmt, m_index, var );
}

int Action_Bind_Values::Bind( boost::int16_t var ) {
  return sqlite3_bind_int( m_state.pStmt, m_index, var );
}

int Action_Bind_Values::Bind( boost::uint16_t var ) {
  return sqlite3_bind_int( m_state.pStmt, m_index, var );
}

int Action_Bind_Values::Bind( boost::int8_t var ) {
  return sqlite3_bind_int( m_state.pStmt, m_index, var );
}

int Action_Bind_Values::Bind( boost::uint8_t var ) {
  return sqlite3_bind_int( m_state.pStmt, m_index, var );
}

int Action_Bind_Values::Bind( const std::string& var ) {
  return sqlite3_bind_text( m_state.pStmt, m_index, var.c_str(), var.length(), SQLITE_TRANSIENT );
}

int Action_Bind_Values::Bind( double var ) {
  return sqlite3_bind_double( m_state.pStmt, m_index, var );
}

// http://www.boost.org/doc/libs/1_45_0/doc/html/date_time/date_time_io.html#date_time.io_tutorial
int Action_Bind_Values::Bind( const boost::posix_time::ptime& var ) {
  // need to fix up with local time
  std::stringstream ss;
  ss << var;
  return Bind( ss.str() );
}

int Action_Bind_Values::Bind( const boost::gregorian::date& var ) {

  const auto ymd = var.year_month_day();
  const auto month = ymd.month.as_number();
  const auto day = ymd.day.as_number();

  std::string s(
    boost::lexical_cast<std::string>( ymd.year ) + '-'
    + ( ( 9 < month ) ? "" : "0" ) + boost::lexical_cast<std::string>( month ) + '-'
    + ( ( 9 < day ) ? "" : "0" ) + boost::lexical_cast<std::string>( day )
  );

  return Bind( s );
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

void Action_Extract_Columns::Column( boost::uint64_t& var ) {
  var = sqlite3_column_int64( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( boost::int32_t& var ) {
  var = sqlite3_column_int( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( boost::uint32_t& var ) {
  var = sqlite3_column_int64( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( boost::int16_t& var ) {
  var = sqlite3_column_int( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( boost::uint16_t& var ) {
  var = sqlite3_column_int( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( boost::int8_t& var ) {
  var = sqlite3_column_int( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( boost::uint8_t& var ) {
  var = sqlite3_column_int( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( std::string& var ) {
  var.assign( reinterpret_cast<const char*>( sqlite3_column_text( m_state.pStmt, m_index ) ) );
}

void Action_Extract_Columns::Column( double& var ) {
  var = sqlite3_column_double( m_state.pStmt, m_index );
}

void Action_Extract_Columns::Column( boost::posix_time::ptime& var ) {
  std::string s;
  Column( s );
  std::stringstream ss( s );
  ss >> var;
}

void Action_Extract_Columns::Column( boost::gregorian::date& var ) {

  std::string s;
  Column( s );

  const uint16_t  year( boost::lexical_cast<uint16_t>( s.substr( 0, 4 ) ) );
  const uint16_t month( boost::lexical_cast<uint16_t>( s.substr( 5, 2 ) ) );
  const uint16_t   day( boost::lexical_cast<uint16_t>( s.substr( 8, 2 ) ) );

  var = boost::gregorian::date( year, month, day );
}

} // sqlite
} // db
} // ou

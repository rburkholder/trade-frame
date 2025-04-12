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

// LibSqlite/Actions.h

#pragma once

#include <stdexcept>

#include <boost/type_traits/is_enum.hpp>
#include <boost/type_traits/is_signed.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/lexical_cast.hpp>

#include <OUCommon/Decimal.h>

#include <OUSQL/Actions.h>

#include "StatementState.h"

namespace ou {
namespace db {
namespace sqlite {

typedef dec::decimal6 money_t;

namespace typeselect {
  template<int size, bool signed_> struct chooser{};  // default is empty
  template<> struct chooser<1,false> { typedef boost:: uint8_t type; };
  template<> struct chooser<1,true>  { typedef boost::  int8_t type; };
  template<> struct chooser<2,false> { typedef boost::uint16_t type; };
  template<> struct chooser<2,true>  { typedef boost:: int16_t type; };
  template<> struct chooser<4,false> { typedef boost::uint32_t type; };
  template<> struct chooser<4,true>  { typedef boost:: int32_t type; };
  template<> struct chooser<8,false> { typedef boost::uint64_t type; };
  template<> struct chooser<8,true>  { typedef boost:: int64_t type; };
}

namespace dispatch {

template<typename T>
const char* FieldType() { // is called with enumerations, so need to figure out appropriate type conversion
  std::string s;
  s += "FieldType2 bad cast: ";
  s += typeid( T ).name();
  throw std::runtime_error( s );
}

template<> const char* FieldType<char>();
template<> const char* FieldType<bool>();
template<> const char* FieldType<boost::int64_t>();
template<> const char* FieldType<boost::uint64_t>();
template<> const char* FieldType<boost::int32_t>();
template<> const char* FieldType<boost::uint32_t>();
template<> const char* FieldType<boost::int16_t>();
template<> const char* FieldType<boost::uint16_t>();
template<> const char* FieldType<boost::int8_t>();
template<> const char* FieldType<boost::uint8_t>();
template<> const char* FieldType<std::string>();
template<> const char* FieldType<double>();
// don't use julian as ptime has no representation earlier than 1400 AD
template<> const char* FieldType<boost::posix_time::ptime>();
template<> const char* FieldType<boost::gregorian::date>();
template<> const char* FieldType<money_t>();

} // namespace dispatch

// ====

class Action_Assemble_TableDef: public ou::db::Action_Assemble_TableDef {
public:

  Action_Assemble_TableDef( const std::string& sTableName ): ou::db::Action_Assemble_TableDef( sTableName ) {};
  ~Action_Assemble_TableDef() {};

  template<typename T, bool b> // is not enum
  const char* FieldType( const boost::integral_constant<bool, b>& ) {
    return dispatch::FieldType<T>();
  }

  template<typename T>
  const char* FieldType( const boost::true_type& ) { // is enum
    return dispatch::FieldType<typename typeselect::chooser<sizeof(T),boost::is_signed<T>::value>::type>();
  }

  template<typename T> // sample code: http://www.boost.org/doc/libs/1_45_0/libs/type_traits/doc/html/boost_typetraits/examples/copy.html
  void Field( const std::string& sFieldName, T& var ) {
    addField( sFieldName, FieldType<T>( boost::is_enum<T>() ) );
  }

  template<typename T>
  void Field( const std::string& sFieldName, T& var, const std::string& sFieldType ) {
    addField( sFieldName, sFieldType.c_str() );
  }

  void Key( const std::string& sFieldName );

protected:
private:
};

class Action_Bind_Values {
public:

  Action_Bind_Values( structStatementState& state ): m_state( state ), m_index( 0 ) {};
  ~Action_Bind_Values() {};

  int Bind( char var );
  int Bind( bool var );
  int Bind( boost::int64_t var );
  int Bind( boost::uint64_t var );
  int Bind( boost::int32_t var );
  int Bind( boost::uint32_t var );
  int Bind( boost::int16_t var );
  int Bind( boost::uint16_t var );
  int Bind( boost::int8_t var );
  int Bind( boost::uint8_t var );
  int Bind( const std::string& var );
  int Bind( double var );
  int Bind( const boost::posix_time::ptime& var );
  int Bind( const boost::gregorian::date& var );
  int Bind( money_t var );

  template<typename T>
  void Field( const std::string& sFieldName, T& var, const std::string& sFieldType = "" ) {
    ++m_index;
    int rtn = Bind( var );
    if ( SQLITE_OK != rtn ) {
      std::string sErr( "Action_Bind_Values::Field::Bind: (" );
      sErr += boost::lexical_cast<std::string>( rtn );
      sErr += ")";
      throw std::runtime_error( sErr );
    }
  }

  void Key( const std::string& sFieldName ) { assert( false ); };  // kludge to provide for SessionImpl::QueryState::ProcessInQueryState::Bind
  void Constraint( const std::string& sFieldName, const std::string& sTableName, const std::string& sField2Name ) {
    assert( false ); };  // kludge to provide for SessionImpl::QueryState::ProcessInQueryState::Bind

protected:
private:
  structStatementState& m_state;
  int m_index;  // index into sql statement, starts at 1
};

class Action_Extract_Columns {
public:

  Action_Extract_Columns( structStatementState& state ): m_state( state ), m_index( 0 ) {};
  ~Action_Extract_Columns() {};

  void Column( bool& var );
  void Column( char& var );
  void Column( boost::int64_t& var );
  void Column( boost::uint64_t& var );
  void Column( boost::int32_t& var );
  void Column( boost::uint32_t& var );
  void Column( boost::int16_t& var );
  void Column( boost::uint16_t& var );
  void Column( boost::int8_t& var );
  void Column( boost::uint8_t& var );
  void Column( std::string& var );
  void Column( double& var );
  void Column( boost::posix_time::ptime& var );
  void Column( boost::gregorian::date& var );
  void Column( money_t& var );

  template<typename T>
  void Field( const std::string& sFieldName, T& var, const std::string& sFieldType = "" ) {
    Field( var, boost::is_enum<T>() );
    ++m_index;
  }

  template<typename T, bool b> // is not enum
  void Field( T& var, const boost::integral_constant<bool, b>& ) {
  //void Field( T& var, const boost::false_type& ) {  // 2012/02/05
    Column( var );
  }

  template<typename T>  // is enum
  void Field( T& var, const boost::true_type& ) {
    Column( reinterpret_cast<typename typeselect::chooser<sizeof(T),boost::is_signed<T>::value>::type&>( var ) );
  }

protected:
private:
  structStatementState& m_state;
  int m_index;  // index into sql statement, starts at 0
};

} // sqlite
} // db
} // ou

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
#include <stdexcept>
#include <typeinfo>

#include <boost/cstdint.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace ou {
namespace db {

// fields

template<typename Action, typename T> // A=Action, T=Type
void Field( Action& action, const std::string& sFieldName, T& var ) {
  action.Field( sFieldName, var );
};

template<typename Action, typename T> // A=Action, T=Type
void Field( Action& action, const std::string& sFieldName, T& var, const std::string& sFieldType ) {
  action.Field( sFieldName, var, sFieldType );
};

template<typename Action> // A=Action
void Key( Action& action, const std::string& sFieldName ) {
  action.Key( sFieldName );
}

// where

template<typename Action, typename T> // A=Action, T=Type
void Where( Action& action, const std::string& sName, T& var ) {  // expand functionality at a later date, defaults to '=' currently
  action.Where( sName, var );
};

// orderby

template<typename Action> // A=Action
void OrderBy( Action& action, const std::string& sName ) {
  action.OrderBy( sName );
};

// constraint

template<typename Action> // A=Action
void Constraint( Action& action, const std::string& sLocalKey, const std::string& sRemoteTable, const std::string& sRemoteKey ) {
  action.Constraint( sLocalKey, sRemoteTable, sRemoteKey );
};

// 
// dealing with key resolution: find db field type from c++ plain old data type
//

// following functions are not used.  Need to fix the enumeration problem first.
template<typename T>
const char* FieldType2( void ) { // is called with enumerations, so need to figure out appropriate type conversion
  std::string s;
  s += "FieldType2 bad cast: ";
  s += typeid( T ).name();
  throw std::runtime_error( s ); 
};
template<> const char* FieldType2<char>( void );
template<> const char* FieldType2<bool>( void );
template<> const char* FieldType2<boost::int64_t>( void );
template<> const char* FieldType2<boost::int32_t>( void );
template<> const char* FieldType2<boost::int16_t>( void );
template<> const char* FieldType2<boost::int8_t>( void );
template<> const char* FieldType2<std::string>( void );
template<> const char* FieldType2<double>( void );
// don't use julian as ptime has no representation earlier than 1400 AD
template<> const char* FieldType2<boost::posix_time::ptime>( void );





} // db
} // ou

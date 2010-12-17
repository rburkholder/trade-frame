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

namespace ou {
namespace db {

// keys

template<typename Action, typename T> // A=Action, T=Type
void Key( Action& action, const std::string& sName, T& var, const std::string& sDbType ) {
  action.registerKey( sName, var, sDbType );
};

template<typename Action, typename T> // A=Action, T=Type
void Key( Action& action, const std::string& sName, T& var ) {
  action.registerKey( sName, var );
};

// fields

template<typename Action, typename T> // A=Action, T=Type
void Field( Action& action, const std::string& sName, T& var, const std::string& sDbType ) {
  action.registerField( sName, var, sDbType );
};

template<typename Action, typename T> // A=Action, T=Type
void Field( Action& action, const std::string& sName, T& var ) {
  action.registerField( sName, var );
};

// where

template<typename Action, typename T> // A=Action, T=Type
void Where( Action& action, const std::string& sName, T& var ) {  // expand functionality at a later date, defaults to '=' currently
  action.registerWhere( sName, var );
};

// orderby

template<typename Action> // A=Action
void OrderBy( Action& action, const std::string& sName ) {
  action.registerOrderBy( sName );
};

// constraint

template<typename Action> // A=Action
void Constraint( Action& action, const std::string& sLocalKey, const std::string& sRemoteTable, const std::string& sRemoteKey ) {
  action.registerConstraint( sLocalKey, sRemoteTable, sRemoteKey );
};

// 
// dealing with key resolution: find db field type from c++ plain old data type
//

const char* FieldType( boost::posix_time::ptime& key ); // don't use julian as ptime has no representation earlier than 1400 AD
const char* FieldType( char key );
const char* FieldType( bool key );
const char* FieldType( boost::int64_t key );
const char* FieldType( boost::int32_t key );
const char* FieldType( boost::int16_t key );
const char* FieldType( boost::int8_t key );
const char* FieldType( std::string& key );
const char* FieldType( double key );

const char* KeyType( boost::int64_t key );
const char* KeyType( std::string& key );

} // db
} // ou

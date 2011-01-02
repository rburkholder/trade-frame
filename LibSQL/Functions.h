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

// fields

template<typename Action, typename T> // A=Action, T=Type
void Field( Action& action, const std::string& sFieldName, T& var ) {
  action.registerField( sFieldName, FieldType( var ) );
};

template<typename Action, typename T> // A=Action, T=Type
void Field( Action& action, const std::string& sFieldName, T& var, const std::string& sFieldType ) {
  action.registerField( sFieldName, sFieldType.c_str() );
};

template<typename Action> // A=Action
void IsKey( Action& action, const std::string& sFieldName ) {
  action.setIsKey( sFieldName );
}

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

} // db
} // ou

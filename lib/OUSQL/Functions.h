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

#pragma once

#include <string>

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




} // db
} // ou

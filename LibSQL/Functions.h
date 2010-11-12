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

namespace ou {
namespace db {

template<class Action, typename Var>
void Key( Action& action, const std::string& sName, Var& var ) {
  action.Key( sName, var );
}

class Action_CreateTable;
void Key( Action_CreateTable& action, const std::string& sName ) {
}

} // db
} // ou

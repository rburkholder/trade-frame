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

#include "StdAfx.h"

#include "SqlDelete.h"

namespace ou {
namespace db {

SqlDelete::SqlDelete( void ) {
}

SqlDelete::~SqlDelete( void ) {
}

void SqlDelete::ComposeStatement( const std::string& sTableName, std::string& sStatement ) {

  sStatement = "DELETE FROM " + sTableName;

  // *** todo: need the 'where' clause yet

  //sStatement += + "WHERE "; 

  sStatement += ";";

}

} // db
} // ou

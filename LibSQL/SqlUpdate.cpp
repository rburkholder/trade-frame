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

#include <boost/lexical_cast.hpp>

#include "SqlUpdate.h"

namespace ou {
namespace db {

Action_FieldsForUpdate::Action_FieldsForUpdate( void ) {
}

Action_FieldsForUpdate::~Action_FieldsForUpdate( void ) {
}

void Action_FieldsForUpdate::ComposeStatement( const std::string& sTableName, std::string& sStatement ) {

  sStatement = "UPDATE " + sTableName + " SET "; 

  int ix = 1;
  for ( vFields_iter_t iter = m_vFields.begin(); m_vFields.end() != iter; ++iter ) {
    if ( 1 < ix ) {
      sStatement += ", ";
    }
    sStatement += iter->sFieldName;
    sStatement += " = $" + boost::lexical_cast<std::string>( ix );
    ++ix;
  }

  // *** todo: need the 'where' clause yet

  sStatement += ";";

}

} // db
} // ou

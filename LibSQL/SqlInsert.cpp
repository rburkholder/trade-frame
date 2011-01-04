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

#include "SqlInsert.h"
 
namespace ou {
namespace db {

Action_FieldsForInsert::Action_FieldsForInsert( void ) {
}

Action_FieldsForInsert::~Action_FieldsForInsert( void ) {
}

void Action_FieldsForInsert::ComposeStatement( const std::string& sTableName, std::string& sStatement ) {

  std::string sFields;
  std::string sHolders;

  int ix = 1;
  for ( vFields_iter_t iter = m_vFields.begin(); m_vFields.end() != iter; ++iter ) {
    if ( 1 < ix ) {
      sFields += ", ";
      sHolders += ", ";
    }
    sFields += iter->sFieldName;
    sHolders += "$" + boost::lexical_cast<std::string>( ix );
    ++ix;
  }

  sStatement = "INSERT INTO " + sTableName + " (" + sFields + ") VALUES (" + sHolders + ");";

}

} // db
} // ou

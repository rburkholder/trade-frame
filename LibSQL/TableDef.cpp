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

#include "TableDef.h"

namespace ou {
namespace db {

void Constraint( Action_CreateTable& action, const std::string& sLocalVar, const std::string& sRemoteTable, const std::string& sRemoteField ) {
  action.Constraint( sLocalVar, sRemoteTable, sRemoteField );
}

// ----

void Action_CreateTable::Key( const std::string& sKey, const char* szDbKeyType  ) {
  structFieldDef fd( sKey, szDbKeyType );
  m_vKeys.push_back( fd );
}

void Action_CreateTable::Field( const std::string& sField, const char* szDbFieldType ) {
  structFieldDef fd( sField, szDbFieldType );
  m_vFields.push_back( fd );
}

void Action_CreateTable::Constraint( const std::string& sLocalField, const std::string& sRemoteTable, const std::string& sRemoteField ) {
  structConstraint constraint( sLocalField, sRemoteTable, sRemoteField );
  m_vConstraints.push_back( constraint );
}

void Action_CreateTable::ComposeStatement( const std::string& sTableName, std::string& sStatement ) {

  sStatement = "CREATE TABLE " + sTableName + " (";
  int ix = 0;

  // keys
  for ( vFields_iter_t iter = m_vKeys.begin(); m_vKeys.end() != iter; ++iter ) {
    if ( 0 != ix ) {
      sStatement += ", ";
    }
    ++ix;
    sStatement += iter->sFieldName + " " + iter->sFieldType;
  }

  // fields
  for ( vFields_iter_t iter = m_vFields.begin(); m_vFields.end() != iter; ++iter ) {
    if ( 0 != ix ) {
      sStatement += ", ";
    }
    ++ix;
    sStatement += iter->sFieldName + " " + iter->sFieldType;
    if ( "BLOB" != iter->sFieldType ) {
      sStatement += " NOT NULL";
    }
  }

  // constraints
  for ( vConstraints_iter_t iter = m_vConstraints.begin(); m_vConstraints.end() != iter; ++iter ) {
    if ( 0 != ix ) {
      sStatement += ", ";
    }
    ++ix;
    sStatement += "CONSTRAINT fk_"
      + sTableName + "_" + iter->sLocalField
      + " FOREIGN KEY(" + iter->sLocalField + ") REFERENCES "
      + iter->sRemoteTable + "(" + iter->sRemoteField + ") ON DELETE RESTRICT ON UPDATE CASCADE";
  }

  // finish statement
  sStatement += ");";

}


} // db
} // ou


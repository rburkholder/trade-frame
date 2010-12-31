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

#include <algorithm>
#include <stdexcept>

#include <boost/spirit/include/phoenix_core.hpp>

#include "TableDef.h"

namespace ou {
namespace db {

using namespace boost::phoenix;
using namespace boost::phoenix::arg_names;


Action_CreateTable::Action_CreateTable( void ) 
  : m_cntKeys( 0 )
{
}

Action_CreateTable::~Action_CreateTable( void ) {
}

void Constraint( Action_CreateTable& action, const std::string& sLocalVar, const std::string& sRemoteTable, const std::string& sRemoteField ) {
  action.Constraint( sLocalVar, sRemoteTable, sRemoteField );
}

// ----

void Action_CreateTable::Field( const std::string& sField, const char* szDbFieldType ) {
  structFieldDef fd( sField, szDbFieldType );
  m_vFields.push_back( fd );
}

void Action_CreateTable::IsKey( const std::string& sFieldName ) {
  //vFields_iter_t iter = std::find_if( m_vFields.begin(), m_vFields.end(), sFieldName,  );
  vFields_iter_t iter = m_vFields.begin();
  while (  iter != m_vFields.end() ) {
    if ( iter->sFieldName == sFieldName ) {
      iter->bIsKeyPart = true;
      ++m_cntKeys;
      break;
    }
    ++iter;
  }
  if ( m_vFields.end() == iter ) {
    throw std::runtime_error( "IsKey, can't find field " + sFieldName );
  }
}

void Action_CreateTable::Constraint( const std::string& sLocalField, const std::string& sRemoteTable, const std::string& sRemoteField ) {
  structConstraint constraint( sLocalField, sRemoteTable, sRemoteField );
  m_vConstraints.push_back( constraint );
}

void Action_CreateTable::ComposeStatement( const std::string& sTableName, std::string& sStatement ) {

  sStatement = "CREATE TABLE " + sTableName + " (";
  int ix = 0;

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
    if ( iter->bIsKeyPart && ( 1 == m_cntKeys ) ) {
      sStatement += " PRIMARY KEY";
    }
  }

  // constraints: primary key
  if ( 1 < m_cntKeys ) {
    if ( 0 != ix ) {
      sStatement += ", ";
    }
    ++ix;
    sStatement += " CONSTRAINT PK_" + sTableName + " PRIMARY KEY (";
    int iy = 0;
    for ( vFields_iter_t iter = m_vFields.begin(); m_vFields.end() != iter; ++iter ) {
      if ( iter->bIsKeyPart ) {
        if ( 0 < iy ) {
          sStatement += ", ";
        }
        ++iy;
        sStatement += iter->sFieldName;
      }
    }
    sStatement += ")";
  }

  // constraints: specified
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


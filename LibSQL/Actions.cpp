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

// LibSql/Actions.cpp

#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include "Actions.h"

namespace ou {
namespace db {

// Action_Compose

Action_Compose::Action_Compose( void ) 
{
}

Action_Compose::~Action_Compose( void ) {
}

void Action_Compose::addField( const std::string& sField, const char* szDbFieldType ) {
  structFieldDef fd( sField, szDbFieldType );
  m_vFields.push_back( fd );
}

// Action_Compose_CreateTable

void Constraint( Action_Compose_CreateTable& action, const std::string& sLocalVar, const std::string& sRemoteTable, const std::string& sRemoteField ) {
  action.registerConstraint( sLocalVar, sRemoteTable, sRemoteField );
}

Action_Compose_CreateTable::Action_Compose_CreateTable( void ) 
  : m_cntKeys( 0 )
{
}

Action_Compose_CreateTable::~Action_Compose_CreateTable( void ) {
}

// setKey
void Action_Compose_CreateTable::setKey( const std::string& sFieldName ) {
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

// registerConstraint
void Action_Compose_CreateTable::registerConstraint( const std::string& sLocalField, const std::string& sRemoteTable, const std::string& sRemoteField ) {
  structConstraint constraint( sLocalField, sRemoteTable, sRemoteField );
  m_vConstraints.push_back( constraint );
}

// ComposeStatement
void Action_Compose_CreateTable::ComposeStatement( const std::string& sTableName, std::string& sStatement ) {

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

// Action_Compose_Insert

Action_Compose_Insert::Action_Compose_Insert( void ) {
}

Action_Compose_Insert::~Action_Compose_Insert( void ) {
}

void Action_Compose_Insert::ComposeStatement( const std::string& sTableName, std::string& sStatement ) {

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

// Action_Compose_Update

Action_Compose_Update::Action_Compose_Update( void ) {
}

Action_Compose_Update::~Action_Compose_Update( void ) {
}

void Action_Compose_Update::ComposeStatement( const std::string& sTableName, std::string& sStatement ) {

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

// Action_Compose_Delete

Action_Compose_Delete::Action_Compose_Delete( void ) {
}

Action_Compose_Delete::~Action_Compose_Delete( void ) {
}

void Action_Compose_Delete::ComposeStatement( const std::string& sTableName, std::string& sStatement ) {

  sStatement = "DELETE FROM " + sTableName;

  // *** todo: need the 'where' clause yet

  //sStatement += + "WHERE "; 

  sStatement += ";";

}
  

} // db
} // ou

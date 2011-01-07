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

#include "StdAfx.h"

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

void Action_Compose::addField( const std::string& sFieldName ) {
  structField f( sFieldName );
  m_vField.push_back( f );
}

// Action_Assemble_TableDef

// =====
void Constraint( 
  Action_Assemble_TableDef& action, 
  const std::string& sLocalVar, 
  const std::string& sRemoteTable, 
  const std::string& sRemoteField ) {
  action.Constraint( sLocalVar, sRemoteTable, sRemoteField );
}
// =====

Action_Assemble_TableDef::Action_Assemble_TableDef( const std::string& sTableName ) 
  : Action_Compose(), m_cntKeys( 0 ), m_sTableName( sTableName )
{
}

Action_Assemble_TableDef::~Action_Assemble_TableDef( void ) {
}

void Action_Assemble_TableDef::addField( const std::string& sField, const char* szDbFieldType ) {
  structFieldDef fd( sField, szDbFieldType );
  m_vFieldDef.push_back( fd );
}

// setKey
void Action_Assemble_TableDef::Key( const std::string& sFieldName ) {
  //vFields_iter_t iter = std::find_if( m_vFields.begin(), m_vFields.end(), sFieldName,  );
  vFieldDef_iter_t iter = m_vFieldDef.begin();
  while (  iter != m_vFieldDef.end() ) {
    if ( iter->sFieldName == sFieldName ) {
      iter->bIsKeyPart = true;
      ++m_cntKeys;
      break;
    }
    ++iter;
  }
  if ( m_vFieldDef.end() == iter ) {
    throw std::runtime_error( "IsKey, can't find field " + sFieldName );
  }
}

// registerConstraint
void Action_Assemble_TableDef::Constraint( 
  const std::string& sLocalField, 
  const std::string& sRemoteTable, 
  const std::string& sRemoteField ) {
  structConstraint constraint( sLocalField, sRemoteTable, sRemoteField );
  m_vConstraints.push_back( constraint );
}

// ComposeStatement
void Action_Assemble_TableDef::ComposeCreateStatement( std::string& sStatement ) {

  sStatement = "CREATE TABLE " + m_sTableName + " (";
  int ix = 0;

  // fields
  for ( vFieldDef_iter_t iter = m_vFieldDef.begin(); m_vFieldDef.end() != iter; ++iter ) {
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
    sStatement += " CONSTRAINT PK_" + m_sTableName + " PRIMARY KEY (";
    int iy = 0;
    for ( vFieldDef_iter_t iter = m_vFieldDef.begin(); m_vFieldDef.end() != iter; ++iter ) {
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
      + m_sTableName + "_" + iter->sLocalField
      + " FOREIGN KEY(" + iter->sLocalField + ") REFERENCES "
      + iter->sRemoteTable + "(" + iter->sRemoteField + ") ON DELETE RESTRICT ON UPDATE CASCADE";
  }

  // finish statement
  sStatement += ");";

}

// Action_Compose_Insert

Action_Compose_Insert::Action_Compose_Insert( const std::string& sTableName )
  : Action_Compose(), m_sTableName( sTableName )
{
}

Action_Compose_Insert::~Action_Compose_Insert( void ) {
}

void Action_Compose_Insert::ComposeStatement( std::string& sStatement ) {

  std::string sFields;
  std::string sHolders;

  int ix = 1;
  for ( vField_iter_t iter = m_vField.begin(); m_vField.end() != iter; ++iter ) {
    if ( 1 < ix ) {
      sFields += ", ";
      sHolders += ", ";
    }
    sFields += iter->sFieldName;
    sHolders += "$" + boost::lexical_cast<std::string>( ix );
    ++ix;
  }

  sStatement = "INSERT INTO " + m_sTableName + " (" + sFields + ") VALUES (" + sHolders + ");";

}

// Action_Compose_Update

Action_Compose_Update::Action_Compose_Update( const std::string& sTableName ) 
  : Action_Compose(), m_sTableName( sTableName )
{
}

Action_Compose_Update::~Action_Compose_Update( void ) {
}

void Action_Compose_Update::ComposeStatement( std::string& sStatement ) {

  sStatement = "UPDATE " + m_sTableName + " SET "; 

  int ix = 1;
  for ( vField_iter_t iter = m_vField.begin(); m_vField.end() != iter; ++iter ) {
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

Action_Compose_Delete::Action_Compose_Delete( const std::string& sTableName ) 
  : Action_Compose(), m_sTableName( sTableName )
{
}

Action_Compose_Delete::~Action_Compose_Delete( void ) {
}

void Action_Compose_Delete::ComposeStatement( std::string& sStatement ) {

  sStatement = "DELETE FROM " + m_sTableName;

  // *** todo: need the 'where' clause yet

  //sStatement += + "WHERE "; 

  sStatement += ";";

}
  

} // db
} // ou

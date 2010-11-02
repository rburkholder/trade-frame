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

#include "stdafx.h"

#include "Statement.h"

#include <stdexcept>

#include <boost/lexical_cast.hpp>

const char* KeyType( boost::int64_t key ) {
  return "INTEGER PRIMARY KEY";
}

const char* KeyType( std::string& key ) {
  return "TEXT";
}

const char* FieldType( boost::posix_time::ptime& key ) { // don't use julian as ptime has no representation earlier than 1400 AD
  return "TEXT";
}

const char* FieldType( int key ) {
  return "INT";
}

const char* FieldType( char key ) {
  return "TINYINT";
}

const char* FieldType( bool key ) {
  return "TINYINT";
}

const char* FieldType( boost::int64_t key ) {
  return "INT8";
}

const char* FieldType( boost::int32_t key ) {
  return "BIGINT";
}

const char* FieldType( boost::int16_t key ) {
  return "SMALLINT";
}

const char* FieldType( boost::int8_t key ) {
  return "TINYINT";
}

const char* FieldType( std::string& key ) {
  return "TEXT";
}

const char* FieldType( double key ) {
  return "DOUBLE";
}



void CStatementCreateTable::ComposeStatement( void ) {

  m_sStatement = "CREATE TABLE " + m_sTableName + "(";
  int ix = 0;

  // keys
  for ( iteratorFieldDef_t iter = m_vKeyDefs.begin(); m_vKeyDefs.end() != iter; ++iter ) {
    if ( 0 != ix ) {
      m_sStatement += ", ";
    }
    ++ix;
    m_sStatement += iter->sFieldName + " " + iter->sFieldType;
  }

  // version
  if ( 0 != ix ) {
    m_sStatement += ", ";
  }
  ++ix;
  m_sStatement += "version SMALLINT DEFAULT 1";

  // fields
  for ( iteratorFieldDef_t iter = m_vFieldDefs.begin(); m_vFieldDefs.end() != iter; ++iter ) {
    if ( 0 != ix ) {
      m_sStatement += ", ";
    }
    ++ix;
    m_sStatement += iter->sFieldName + " " + iter->sFieldType;
    if ( "BLOB" != iter->sFieldType ) {
      m_sStatement += " NOT NULL";
    }
  }

  // constraints
  for ( iteratorConstraint_t iter = m_vConstraints.begin(); m_vConstraints.end() != iter; ++iter ) {
    if ( 0 != ix ) {
      m_sStatement += ", ";
    }
    ++ix;
    m_sStatement += "CONSTRAINT fk_" 
      + m_sTableName + "_" + iter->sLocalKey 
      + " FOREIGN KEY(" + iter->sLocalKey + ") REFERENCES " 
      + iter->sRemoteTable + "(" + iter->sRemoteKey + ") ON DELETE RESTRICT ON UPDATE CASCADE";
  }

  // close statemnt
  m_sStatement += ");";

}

void CStatementInsert::ComposeStatement( void ) {

  int ix = 0;

  std::string sKeys;
  std::string sHolders;

  // keys
  for ( iteratorFieldDef_t iter = m_vKeyDefs.begin(); m_vKeyDefs.end() != iter; ++iter ) {
    if ( 0 != ix ) {
      m_sStatement += ", ";
    }
    ++ix;
    sKeys += iter->sFieldName;
    sHolders += ":" + iter->sFieldName;
  }

  // fields 
  for ( iteratorFieldDef_t iter = m_vFieldDefs.begin(); m_vFieldDefs.end() != iter; ++iter ) {
    if ( 0 != ix ) {
      m_sStatement += ", ";
    }
    ++ix;
    sKeys += iter->sFieldName;
    sHolders += ":" + iter->sFieldName;
  }

  m_sStatement = "INSERT INTO " + m_sTableName + "(" + sKeys + ") VALUES (" + sHolders + ");";

}

void CStatementUpdate::ComposeStatement( void ) {

  int ix = 0;

  m_sStatement = "UPDATE " + m_sTableName + " SET ";

  for ( iteratorFieldDef_t iter = m_vFieldDefs.begin(); m_vFieldDefs.end() != iter; ++iter ) {
    if ( 0 != ix ) {
      m_sStatement += ", ";
    }
    ++ix;
    m_sStatement += iter->sFieldName + "= :" + iter->sFieldName;
  }

  m_sStatement += "WHERE ";

  ix = 0;
  for ( iteratorFieldDef_t iter = m_vKeyDefs.begin(); m_vKeyDefs.end() != iter; ++iter ) {
    if ( 0 != ix ) {
      m_sStatement += " AND ";
    }
    ++ix;
    m_sStatement += iter->sFieldName + "= :" + iter->sFieldName;
  }

  m_sStatement += ";";
}

void CStatementSelect::ComposeStatement( void ) {

  int ix = 0;

  m_sStatement = "SELECT ";

  for ( iteratorFieldDef_t iter = m_vFieldDefs.begin(); m_vFieldDefs.end() != iter; ++iter ) {
    if ( 0 != ix ) {
      m_sStatement += ", ";
    }
    ++ix;
    m_sStatement += iter->sFieldName;
  }

  m_sStatement += "FROM " + m_sTableName + "WHERE ";

  ix = 0;
  for ( iteratorFieldDef_t iter = m_vKeyDefs.begin(); m_vKeyDefs.end() != iter; ++iter ) {
    if ( 0 != ix ) {
      m_sStatement += " AND ";
    }
    ++ix;
    m_sStatement += iter->sFieldName + "= :" + iter->sFieldName;
  }

  m_sStatement += ";"; 

}

void CStatementDelete::ComposeStatement( void ) {

  int ix = 0;

  m_sStatement = "DELETE FROM " + m_sTableName + " WHERE ";

  for ( iteratorFieldDef_t iter = m_vKeyDefs.begin(); m_vKeyDefs.end() != iter; ++iter ) {
    if ( 0 != ix ) {
      m_sStatement += " AND ";
    }
    ++ix;
    m_sStatement += iter->sFieldName + "= :" + iter->sFieldName;
  }

  m_sStatement += ";"; 
}


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

namespace ou {
namespace db {



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

} // db
} // ou

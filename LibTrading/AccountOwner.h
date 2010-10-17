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

#include <LibSqlite/sqlite3.h>

#include "AccountAdvisor.h"

class CAccountOwner
{
public:

  typedef CAccountAdvisor::keyAccountAdvisorId_t keyAccountAdvisorId_t;
  typedef std::string keyAccountOwnerId_t;

  CAccountOwner( 
    const keyAccountOwnerId_t& sAccountOwnerId, 
    const keyAccountAdvisorId_t& sAccountAdvisorId,
    const std::string& sFirstName, const std::string& sLastName );
  CAccountOwner( const keyAccountOwnerId_t& sAccountOwnerId, sqlite3_stmt* pStmt);
  ~CAccountOwner(void);

  static void CreateDbTable( sqlite3* pDb );
  int BindDbKey( sqlite3_stmt* pStmt );
  int BindDbVariables( sqlite3_stmt* pStmt );
  static const std::string& GetSqlSelect( void ) { return m_sSqlSelect; };
  static const std::string& GetSqlInsert( void ) { return m_sSqlInsert; };
  static const std::string& GetSqlUpdate( void ) { return m_sSqlUpdate; };
  static const std::string& GetSqlDelete( void ) { return m_sSqlDelete; };

protected:
private:

  static const std::string m_sSqlCreate;
  static const std::string m_sSqlSelect;
  static const std::string m_sSqlInsert;
  static const std::string m_sSqlUpdate;
  static const std::string m_sSqlDelete;

  keyAccountOwnerId_t m_sAccountOwnerId;
  keyAccountAdvisorId_t m_sAccountAdvisorId;
  std::string m_sFirstName;
  std::string m_sLastName;

};


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

#include "boost/shared_ptr.hpp"

#include <LibSqlite/sqlite3.h>

class CAccountAdvisor
{
  friend class CAccountManager;

public:

  typedef boost::shared_ptr<CAccountAdvisor> pAccountAdvisor_t;
  typedef pAccountAdvisor_t sharedptr_t;

  CAccountAdvisor( const std::string& sAdvisorId, const std::string& sAdvisorName );
  CAccountAdvisor( const std::string& sAdvisorId, sqlite3_stmt* pStmt );
  ~CAccountAdvisor(void);

  const std::string& GetId( void ) const { 
    return m_sAdvisorId; 
  }

  const std::string& GetName( void ) const { 
    return m_sAdvisorName; 
  }

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

  std::string m_sAdvisorId;
  std::string m_sAdvisorName;

  CAccountAdvisor( void );
};



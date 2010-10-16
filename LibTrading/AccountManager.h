/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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
#include <map>

#include <LibSqlite/sqlite3.h>

#include "ManagerBase.h"
#include "AccountAdvisor.h"
#include "AccountOwner.h"
#include "Account.h"

class CAccountManager: public ManagerBase<CAccountManager, std::string, CAccountAdvisor> {
public:

  typedef CAccountAdvisor::pAccountAdvisor_t pAccountAdvisor_t;

  CAccountManager( void );
  CAccountManager( sqlite3* pDb );
  ~CAccountManager(void);

  void CreateDbTables( void );

  pAccountAdvisor_t AddAccountAdvisor( const std::string& sAdvisorId, const std::string& sAccountAdvisorName );
  pAccountAdvisor_t GetAccountAdvisor( const std::string& sAdvisorId );
  pAccountAdvisor_t UpdateAccountAdvisor( const std::string& sAdvisorId );  // uses existing class variables
  void DeleteAccountAdvisor( const std::string& sAdvisorId );

protected:


private:

  sqlite3* m_pDb;

  sqlite3_stmt* pStmtLoadAccountAdvisor;
  sqlite3_stmt* pStmtAddAccountAdvisor;
  sqlite3_stmt* pStmtUpdateAccountAdvisor;
  sqlite3_stmt* pStmtDeleteAccountAdvisor;

  typedef std::pair<std::string, pAccountAdvisor_t> pairAccountAdvisor_t;
  typedef std::map<std::string, pAccountAdvisor_t> mapAccountAdvisor_t;
  typedef mapAccountAdvisor_t::iterator iterAccountAdvisor_t;
  mapAccountAdvisor_t m_mapAccountAdvisor;

};


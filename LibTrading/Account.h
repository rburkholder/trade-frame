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

#include "AccountOwner.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CAccount
{
public:

  typedef std::string keyAccountId_t;
  typedef CAccountOwner::keyAccountOwnerId_t keyAccountOwnerId_t;

  CAccount(
    const keyAccountId_t& sAccountId,
    const keyAccountOwnerId_t& sAccountOwnerId,
    const std::string& sAccountName,
    const std::string& sProviderName,
    const std::string& sBrokerName,
    const std::string& sBrokerAccountId,
    const std::string& sLogin, 
    const std::string& sPassword
    );
  CAccount( const keyAccountId_t& sAccountId, sqlite3_stmt* pStmt );
  ~CAccount(void);

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

  keyAccountId_t m_sAccountId;
  keyAccountOwnerId_t m_sAccountOwnerId;
  std::string m_sAccountName;
  std::string m_sProviderName;
  std::string m_sBrokerName;
  std::string m_sBrokerAccountId;
  std::string m_sLogin;
  std::string m_sPassword;

};

} // namespace tf
} // namespace ou

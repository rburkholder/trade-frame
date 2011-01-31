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

#include <stdexcept>

#include "Account.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

const std::string CAccount::m_sSqlCreate(
  "create table accounts ( \
    accountid TEXT CONSTRAINT pk_accounts PRIMARY KEY, \
    version SMALLINT DEFAULT 1, \
    accountownerid TEXT NOT NULL, \
    accountname TEXT NOT NULL, \
    providername TEXT NOT NULL, \
    brokername TEXT default '', \
    brokeraccountid TEXT default '', \
    login TEXT default '', \
    password TEXT default '', \
    CONSTRAINT fk_accounts_accountownerid \
      FOREIGN KEY(accountownerid) REFERENCES accountowners(accountownderid) \
        ON DELETE RESTRICT ON UPDATE CASCADE \
       \
    );"
  );
const std::string CAccount::m_sSqlSelect( 
  "SELECT accountownderid, accountname, providername, brokername, brokeraccountid, login, password FROM accounts WHERE accountid = :id;" );
const std::string CAccount::m_sSqlInsert( 
  "INSERT INTO accounts (accountid, accountownerid, accountname, providername, brokername, brokeraccountid, login, password ) \
  VALUES (:id, :accountownerid, :accountname, :providername, :brokername, :brokeraccountid, :login, :password );" );
const std::string CAccount::m_sSqlUpdate( 
  "UPDATE accounts SET \
  accountownderid = :accountownerid, \
  accountname = :accountname, \
  providername = :providername, \
  brokername = :brokername, \
  brokeraccountid = :brokeraccountid, \
  login = :login, \
  password = :password \
  WHERE accountid = :id \
  ;" );
const std::string CAccount::m_sSqlDelete( "DELETE FROM accounts WHERE accountid = :id;" );


CAccount::CAccount(
    const keyAccountId_t& sAccountId,
    const keyAccountOwnerId_t& sAccountOwnerId,
    const std::string& sAccountName,
    const std::string& sProviderName,
    const std::string& sBrokerName,
    const std::string& sBrokerAccountId,
    const std::string& sLogin, 
    const std::string& sPassword
    ) 
: m_sAccountId( sAccountId ),
  m_sAccountOwnerId( sAccountOwnerId ),
  m_sAccountName( sAccountName ),
  m_sProviderName( sProviderName ),
  m_sBrokerName( sBrokerName ), m_sBrokerAccountId( sBrokerAccountId ),
  m_sLogin( sLogin ), m_sPassword( sPassword )
{
}

CAccount::CAccount( const keyAccountId_t& sAccountId, sqlite3_stmt* pStmt ) 
: m_sAccountId( sAccountId ),
  m_sAccountOwnerId( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 0 ) ) ),
  m_sAccountName( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 1 ) ) ),
  m_sProviderName( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 2 ) ) ),
  m_sBrokerName( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 3 ) ) ),
  m_sBrokerAccountId( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 4 ) ) ),
  m_sLogin( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 5 ) ) ),
  m_sPassword( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 6 ) ) )
{
}

CAccount::~CAccount(void) {
}

void CAccount::CreateDbTable( sqlite3* pDb ) {

  char* pMsg;
  int rtn;

  rtn = sqlite3_exec( pDb, m_sSqlCreate.c_str(), 0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating table accounts: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }
}

int CAccount::BindDbKey( sqlite3_stmt* pStmt ) {
  int rtn( 0 );
  rtn = sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":id" ), m_sAccountId.c_str(), -1, SQLITE_TRANSIENT );
  return rtn;
}

int CAccount::BindDbVariables( sqlite3_stmt* pStmt ) {
  int rtn( 0 );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":accountownerid" ), m_sAccountOwnerId.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":accountname" ), m_sAccountName.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":providername" ), m_sProviderName.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":brokername" ), m_sProviderName.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":brokeraccountid" ), m_sBrokerAccountId.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":login" ), m_sLogin.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":password" ), m_sPassword.c_str(), -1, SQLITE_TRANSIENT );
  return rtn;  // should be 0 if all goes well
}

} // namespace tf
} // namespace ou

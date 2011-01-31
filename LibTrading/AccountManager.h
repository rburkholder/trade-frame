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

#include <sstream>
#include <stdexcept>
#include <cassert>

#include "ManagerBase.h"
#include "AccountAdvisor.h"
#include "AccountOwner.h"
#include "Account.h"

#include "Portfolio.h"  // should be created through PortfolioManager?
#include "Position.h"   // should be created through PortfolioManager?
#include "Order.h"      // should be created through OrderManager?
#include "Execution.h"  // should be created through OrderManager?

namespace ou { // One Unified
namespace tf { // TradeFrame

template<class Session>
class CAccountManager: public ManagerBase<CAccountManager, std::string, CAccountAdvisor> {
public:

  typedef CAccountAdvisor::pAccountAdvisor_t pAccountAdvisor_t;

  CAccountManager( void );  // no use of database
  CAccountManager( Session* session ); // uses database
  ~CAccountManager(void);

  void CreateDbTables( void );

  pAccountAdvisor_t AddAccountAdvisor( const std::string& sAdvisorId, const std::string& sAccountAdvisorName );
  pAccountAdvisor_t GetAccountAdvisor( const std::string& sAdvisorId );
  pAccountAdvisor_t UpdateAccountAdvisor( const std::string& sAdvisorId );  // uses existing class variables
  void DeleteAccountAdvisor( const std::string& sAdvisorId );

protected:

private:

  Session* m_session;

//  sqlite3_stmt* pStmtLoadAccountAdvisor;
//  sqlite3_stmt* pStmtAddAccountAdvisor;
//  sqlite3_stmt* pStmtUpdateAccountAdvisor;
//  sqlite3_stmt* pStmtDeleteAccountAdvisor;

  typedef std::pair<std::string, pAccountAdvisor_t> pairAccountAdvisor_t;
  typedef std::map<std::string, pAccountAdvisor_t> mapAccountAdvisor_t;
  typedef mapAccountAdvisor_t::iterator iterAccountAdvisor_t;
  mapAccountAdvisor_t m_mapAccountAdvisor;

};

template<class Session>
CAccountManager::CAccountManager( void ) 
: ManagerBase<CAccountManager, std::string, CAccountAdvisor>(), 
  m_session( 0 )
  //, pStmtLoadAccountAdvisor( NULL ), pStmtAddAccountAdvisor( NULL )
{
}

template<class Session>
CAccountManager::CAccountManager( Session* session ) 
: ManagerBase<CAccountManager, std::string, CAccountAdvisor>(),
  m_session( session )
  //, pStmtLoadAccountAdvisor( NULL ), pStmtAddAccountAdvisor( NULL )
{
  assert( 0 != m_session );
}

template<class Session>
CAccountManager::~CAccountManager(void) {
}

template<class Session>
void CAccountManager::CreateDbTables( void ) {

  try {  // use Transaction here, or use automatically in table creation section
/* add tables to session and create
    CAccountAdvisor::CreateDbTable( m_pDb );
    CAccountOwner::CreateDbTable( m_pDb );
    CAccount::CreateDbTable( m_pDb );
    CPortfolio::CreateDbTable( m_pDb );
    CPosition::CreateDbTable( m_pDb );
    COrder::CreateDbTable( m_pDb );
    CExecution::CreateDbTable( m_pDb );
*/
  }
  catch ( std::runtime_error &e ) {
  }

}

template<class Session>
CAccountAdvisor::pAccountAdvisor_t CAccountManager::GetAccountAdvisor( const std::string& sAdvisorId ) {

  pAccountAdvisor_t p;

  // arg1=key, arg2=pStmt
  LoadObject( "CAccountManager::GetAccountAdvisor", CAccountAdvisor::GetSqlSelect(), sAdvisorId, 
    m_pDb, 
    &pStmtLoadAccountAdvisor, 
    m_mapAccountAdvisor, 
    p, 
    boost::phoenix::new_<CAccountAdvisor>( arg1, arg2 ) 
    );

  return p;
}

template<class Session>
CAccountAdvisor::pAccountAdvisor_t CAccountManager::AddAccountAdvisor( const std::string& sAdvisorId, const std::string& sAdvisorName ) {

  pAccountAdvisor_t p;

  bool bExists = false;
  try {
    p = GetAccountAdvisor( sAdvisorId );
    // if we have something we can't insert something
    bool bExists = true;
  }
  catch (...) {
    // had an error, so assume no record exists, and proceed
  }

  if ( bExists ) {
    throw std::runtime_error( "CAccountManager::AddAccountAdvisor: record already exists" );
  }

  // create class, as it will do some of the processing
  p.reset( new CAccountAdvisor( sAdvisorId, sAdvisorName ) );

  SqlOpOnObject( "CAccountManager::AddAccountAdvisor", CAccountAdvisor::GetSqlInsert(), 
    m_pDb,
    &pStmtAddAccountAdvisor,
    sAdvisorId, 
    p );

  // if everything is fine, can add record to map
  m_mapAccountAdvisor.insert( pairAccountAdvisor_t( sAdvisorId, p ) );

  return p;
}

template<class Session>
CAccountAdvisor::pAccountAdvisor_t CAccountManager::UpdateAccountAdvisor( const std::string& sAdvisorId ) {

  pAccountAdvisor_t p;

  iterAccountAdvisor_t iter = m_mapAccountAdvisor.find( sAdvisorId );
  if ( m_mapAccountAdvisor.end() == iter ) {
    throw std::runtime_error( "CAccountManager::UpdateAccountAdvisor: could not find advisor in local storage" );
  }
  p = iter->second;

  SqlOpOnObject( "CAccountManager::UpdateAccountAdvisor", CAccountAdvisor::GetSqlUpdate(), 
    m_pDb,
    &pStmtUpdateAccountAdvisor,
    sAdvisorId, 
    p );

  return p;
}

template<class Session>
void CAccountManager::DeleteAccountAdvisor( const std::string& sAdvisorId ) {

  iterAccountAdvisor_t iter = m_mapAccountAdvisor.find( sAdvisorId );
  if ( m_mapAccountAdvisor.end() == iter ) {
    throw std::runtime_error( "CAccountManager::DeleteAccountAdvisor: could not find advisor in local storage" );
  }

  DeleteObject( "CAccountManager::DeleteAccountAdvisor", CAccountAdvisor::GetSqlDelete(), 
    m_pDb,
    &pStmtDeleteAccountAdvisor,
    sAdvisorId, 
    iter->second );

  m_mapAccountAdvisor.erase( iter );

}

} // namespace tf
} // namespace ou

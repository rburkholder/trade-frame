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

#include "StdAfx.h"

#include <sstream>
#include <string>
#include <stdexcept>
#include <cassert>

#include "Portfolio.h"
#include "Position.h"
#include "Order.h"
#include "Execution.h"

#include "AccountManager.h"

CAccountManager::CAccountManager( void ) 
: ManagerBase<CAccountManager, std::string, CAccountAdvisor>(), 
  m_pDb( NULL )
{
}

CAccountManager::CAccountManager( sqlite3* pDb ) 
: ManagerBase<CAccountManager, std::string, CAccountAdvisor>(),
  m_pDb( pDb )
{
  assert( NULL != pDb );
}

CAccountManager::~CAccountManager(void) {
}

void CAccountManager::CreateDbTables( void ) {

  try {
    /*
    CAccountAdvisor aa;
    aa.CreateDbTable( m_pDb );

    CAccountOwner ao;
    ao.CreateDbTable( m_pDb );

    CAccount a;
    a.CreateDbTable( m_pDb );
    */

    CAccountAdvisor::CreateDbTable( m_pDb );
    CAccountOwner::CreateDbTable( m_pDb );
    CAccount::CreateDbTable( m_pDb );
    CPortfolio::CreateDbTable( m_pDb );
    CPosition::CreateDbTable( m_pDb );
    COrder::CreateDbTable( m_pDb );
    CExecution::CreateDbTable( m_pDb );

  }
  catch ( std::runtime_error &e ) {
  }

}

CAccountAdvisor::pAccountAdvisor_t CAccountManager::GetAccountAdvisor( const std::string& sAccountAdvisorId ) {

  pAccountAdvisor_t p;

  // prepare this stuff in constructor

  if ( NULL != m_pDb ) {
    int rtn, rtnStep;
    sqlite3_stmt* pStmt;
    rtn = sqlite3_prepare_v2( m_pDb, 
      "SELECT name from accountadvisors where accountadvisorid = :id;",
      -1, &pStmt, NULL );
    rtnStep = sqlite3_step( pStmt );
    if ( true ) {
      p.reset( new CAccountAdvisor( sAccountAdvisorId, reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 0 ) ) ) );
    }
    else {
      
    }
    rtn = sqlite3_finalize( pStmt );
  }

  if ( NULL == p.get() ) {
    throw std::runtime_error( "GetAccountAdvisor:  no record available" );
  }

  // update the map here

  return p;
}

CAccountAdvisor::pAccountAdvisor_t CAccountManager::AddAccountAdvisor( const std::string& sAdvisorId, const std::string& sAdvisorName ) {

  pAccountAdvisor_t p;

  // prepare this stuff in constructor

  if ( NULL != m_pDb ) {
    int rtn, rtnStep;
    sqlite3_stmt* pStmt;
    rtn = sqlite3_prepare_v2( m_pDb, 
      "INSERT INTO accountadvisors (accountadvisorid, name ) VALUES ( :id, :name );",
      -1, &pStmt, NULL );
    rtn = sqlite3_bind_text( pStmt, sqlite3_bind_parameter_index( pStmt, ":id" ), sAdvisorId.c_str(), -1, SQLITE_TRANSIENT );
    rtn = sqlite3_bind_text( pStmt, sqlite3_bind_parameter_index( pStmt, ":name" ), sAdvisorName.c_str(), -1, SQLITE_TRANSIENT );
    rtnStep = sqlite3_step( pStmt );
    rtn = sqlite3_finalize( pStmt );
    if ( SQLITE_DONE != rtnStep ) {
      std::stringstream ss;
      ss << "AccountAdvisor insert error:  " << rtnStep;
      throw std::runtime_error( ss.str() );
    }
  }

  // update the map here

  p.reset( new CAccountAdvisor( sAdvisorId, sAdvisorName ) );
  return p;
}


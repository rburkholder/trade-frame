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

#include <boost/spirit/home/phoenix/object/new.hpp>
#include <boost/spirit/home/phoenix/core/argument.hpp>
#include <boost/spirit/home/phoenix/bind/bind_function.hpp>
#include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>

#include "Portfolio.h"
#include "Position.h"
#include "Order.h"
#include "Execution.h"

#include "TradingDb.h"
#include "AccountManager.h"

using namespace boost::phoenix::arg_names;

CAccountManager::CAccountManager( void ) 
: ManagerBase<CAccountManager, std::string, CAccountAdvisor>(), 
  m_pDb( NULL ),
  pStmtLoadAccountAdvisor( NULL ), pStmtAddAccountAdvisor( NULL )
{
}

CAccountManager::CAccountManager( sqlite3* pDb ) 
: ManagerBase<CAccountManager, std::string, CAccountAdvisor>(),
  m_pDb( pDb ),
  pStmtLoadAccountAdvisor( NULL ), pStmtAddAccountAdvisor( NULL )
{
  assert( NULL != pDb );
}

CAccountManager::~CAccountManager(void) {
  if ( NULL != pStmtLoadAccountAdvisor ) {
    int rtn = sqlite3_finalize( pStmtLoadAccountAdvisor );
    pStmtLoadAccountAdvisor = NULL;
  }
  if ( NULL != pStmtAddAccountAdvisor ) {
    int rtn = sqlite3_finalize( pStmtAddAccountAdvisor );
    pStmtAddAccountAdvisor = NULL;
  }
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

/*
struct bind_key {
  int operator()( const std::string& key, sqlite3_stmt* pStmt ) {
    return sqlite3_bind_text( pStmt, sqlite3_bind_parameter_index( pStmt, ":id" ), key.c_str(), -1, SQLITE_TRANSIENT );
  }
};
*/

CAccountAdvisor::pAccountAdvisor_t CAccountManager::GetAccountAdvisor( const std::string& sAdvisorId ) {

  pAccountAdvisor_t p;

  // arg1=key, arg2=pStmt
  LoadObject( "CAccountManager::GetAccountAdvisor", CAccountAdvisor::GetSqlSelect(), sAdvisorId, 
    &pStmtLoadAccountAdvisor, 
    m_mapAccountAdvisor, 
    p, 
    boost::phoenix::bind( &sqlite3_bind_text, arg2,   // fBindStatment( key, *pStmt )
      boost::phoenix::bind( &sqlite3_bind_parameter_index, arg2, ":id" ), 
      boost::phoenix::bind( &std::string::c_str, arg1 ), -1, SQLITE_TRANSIENT ),
    boost::phoenix::new_<CAccountAdvisor>( arg1, arg2 ) 
    );

  return p;
}

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

  InsertObject( "CAccountManager::AddAccountAdvisor", CAccountAdvisor::GetSqlInsert(), 
    &pStmtAddAccountAdvisor,
    sAdvisorId, m_mapAccountAdvisor, p );

  return p;
}


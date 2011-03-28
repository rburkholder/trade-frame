/************************************************************************
 * Copyright(c) 2009-2011, One Unified. All rights reserved.            *
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
#include <cassert>

#include "AccountManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

CAccountManager::CAccountManager( void ) 
: ManagerBase<CAccountManager>()//, 
  //m_session( 0 )
{
}

CAccountManager::~CAccountManager(void) {
}

CAccountAdvisor::pAccountAdvisor_t CAccountManager::ConstructAccountAdvisor( const idAccountAdvisor_t& idAdvisor, const std::string& sAdvisorName, const std::string& sCompanyName ) {

  pAccountAdvisor_t p( new CAccountAdvisor( idAdvisor, sAdvisorName, sCompanyName ) );

  iterAccountAdvisor_t iter = m_mapAccountAdvisor.find( idAdvisor );
  if ( m_mapAccountAdvisor.end() != iter ) {
    throw std::runtime_error( "AccountAdvisor already exists" );
  }
  else {
    m_mapAccountAdvisor.insert( pairAccountAdvisor_t( idAdvisor, p ) );
    ou::db::QueryFields<CAccountAdvisor::TableRowDef>::pQueryFields_t pQuery 
      = m_pDbSession->Insert<CAccountAdvisor::TableRowDef>( const_cast<CAccountAdvisor::TableRowDef&>( p->GetRow() ) );
  }

  return p;
}

namespace AccountManagerQueries {
  struct AccountAdvisorKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "accountadvisorid", idAccountAdvisor );
    }
    const ou::tf::keytypes::idAccountAdvisor_t& idAccountAdvisor;
    AccountAdvisorKey( const ou::tf::keytypes::idAccountAdvisor_t& idAccountAdvisor_ ): idAccountAdvisor( idAccountAdvisor_ ) {};
  };
}

CAccountAdvisor::pAccountAdvisor_t CAccountManager::GetAccountAdvisor( const idAccountAdvisor_t& idAdvisor ) {

  pAccountAdvisor_t pAccountAdvisor;

  iterAccountAdvisor_t iter = m_mapAccountAdvisor.find( idAdvisor );
  if ( m_mapAccountAdvisor.end() != iter ) {
    pAccountAdvisor = iter->second;
  }
  else {
    AccountManagerQueries::AccountAdvisorKey key( idAdvisor );
    ou::db::QueryFields<AccountManagerQueries::AccountAdvisorKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
      = m_pDbSession->SQL<AccountManagerQueries::AccountAdvisorKey>( "select * from accountadvisors", key ).Where( "accountadvisorid = ?" ).NoExecute();
    m_pDbSession->Bind<AccountManagerQueries::AccountAdvisorKey>( pExistsQuery );
    if ( m_pDbSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
      CAccountAdvisor::TableRowDef rowAccountAdvisor;
      m_pDbSession->Columns<AccountManagerQueries::AccountAdvisorKey, CAccountAdvisor::TableRowDef>( pExistsQuery, rowAccountAdvisor );
      pAccountAdvisor.reset( new CAccountAdvisor( rowAccountAdvisor ) );
      m_mapAccountAdvisor.insert( pairAccountAdvisor_t( idAdvisor, pAccountAdvisor ) );
    }
    else {
      throw std::runtime_error( "AccountAdvisor does not exist" );
    }
  }

  return pAccountAdvisor;
}

void CAccountManager::DeleteAccountAdvisor( const idAccountAdvisor_t& idAdvisor ) {

  pAccountAdvisor_t pAccountAdvisor( GetAccountAdvisor( idAdvisor ) );  // has exception if does not exist

  iterAccountAdvisor_t iter = m_mapAccountAdvisor.find( idAdvisor );
  if ( m_mapAccountAdvisor.end() == iter ) {
    throw std::runtime_error( "CAccountManager::DeleteAccountAdvisor: could not find advisor in local storage" );
  }

  AccountManagerQueries::AccountAdvisorKey key( idAdvisor );
  ou::db::QueryFields<AccountManagerQueries::AccountAdvisorKey>::pQueryFields_t pQueryDelete
    = m_pDbSession->Delete<AccountManagerQueries::AccountAdvisorKey>( key ).Where( "accountadvisorid = ?" );
  m_mapAccountAdvisor.erase( iter );

}

void CAccountManager::RegisterTablesForCreation( void ) {
  m_pDbSession->RegisterTable<CAccountAdvisor::TableCreateDef>( tablenames::sAccountAdvisor );
  m_pDbSession->RegisterTable<CAccountOwner::TableCreateDef>( tablenames::sAccountOwner );
  m_pDbSession->RegisterTable<CAccount::TableCreateDef>( tablenames::sAccount );
}

void CAccountManager::RegisterRowDefinitions( void ) {
  m_pDbSession->MapRowDefToTableName<CAccountAdvisor::TableRowDef>( tablenames::sAccountAdvisor );
  m_pDbSession->MapRowDefToTableName<CAccountOwner::TableRowDef>( tablenames::sAccountOwner );
  m_pDbSession->MapRowDefToTableName<CAccount::TableRowDef>( tablenames::sAccount );
}

void CAccountManager::PopulateTables( void ) {
}


} // namespace tf
} // namespace ou

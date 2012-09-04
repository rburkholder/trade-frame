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

// todo:  only use database stuff if session is non zero

namespace ou { // One Unified
namespace tf { // TradeFrame

CAccountManager::CAccountManager( void ) 
: ManagerBase<CAccountManager>()//, 
  //m_session( 0 )
{
}

CAccountManager::~CAccountManager(void) {
}

//
// Account Advisor
//

CAccountManager::pAccountAdvisor_t CAccountManager::ConstructAccountAdvisor( const idAccountAdvisor_t& idAdvisor, const std::string& sAdvisorName, const std::string& sCompanyName ) {

  pAccountAdvisor_t p( new CAccountAdvisor( idAdvisor, sAdvisorName, sCompanyName ) );

  iterAccountAdvisor_t iter = m_mapAccountAdvisor.find( idAdvisor );
  if ( m_mapAccountAdvisor.end() != iter ) {
    throw std::runtime_error( "AccountAdvisor already exists" );
  }
  else {
    m_mapAccountAdvisor.insert( pairAccountAdvisor_t( idAdvisor, p ) );
    ou::db::QueryFields<CAccountAdvisor::TableRowDef>::pQueryFields_t pQuery 
      = m_pSession->Insert<CAccountAdvisor::TableRowDef>( const_cast<CAccountAdvisor::TableRowDef&>( p->GetRow() ) );
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

CAccountManager::pAccountAdvisor_t CAccountManager::GetAccountAdvisor( const idAccountAdvisor_t& idAdvisor ) {

  pAccountAdvisor_t pAccountAdvisor;

  iterAccountAdvisor_t iter = m_mapAccountAdvisor.find( idAdvisor );
  if ( m_mapAccountAdvisor.end() != iter ) {
    pAccountAdvisor = iter->second;
  }
  else {
    AccountManagerQueries::AccountAdvisorKey key( idAdvisor );
    ou::db::QueryFields<AccountManagerQueries::AccountAdvisorKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
      = m_pSession->SQL<AccountManagerQueries::AccountAdvisorKey>( "select * from accountadvisors", key ).Where( "accountadvisorid = ?" ).NoExecute();
    m_pSession->Bind<AccountManagerQueries::AccountAdvisorKey>( pExistsQuery );
    if ( m_pSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
      CAccountAdvisor::TableRowDef rowAccountAdvisor;
      m_pSession->Columns<AccountManagerQueries::AccountAdvisorKey, CAccountAdvisor::TableRowDef>( pExistsQuery, rowAccountAdvisor );
      pAccountAdvisor.reset( new CAccountAdvisor( rowAccountAdvisor ) );
      m_mapAccountAdvisor.insert( pairAccountAdvisor_t( idAdvisor, pAccountAdvisor ) );
    }
    else {
      throw std::runtime_error( "AccountAdvisor does not exist" );
    }
  }

  return pAccountAdvisor;
}

void CAccountManager::DeleteAccountAdvisor( const idAccountAdvisor_t& idAccountAdvisor ) {

  pAccountAdvisor_t pAccountAdvisor( GetAccountAdvisor( idAccountAdvisor ) );  // has exception if does not exist

  DeleteRecord<idAccountAdvisor_t, mapAccountAdvisor_t, AccountManagerQueries::AccountAdvisorKey>( 
    idAccountAdvisor, m_mapAccountAdvisor, "accountadvisorid = ?" );
}

//
// Account Owner
//

CAccountManager::pAccountOwner_t CAccountManager::ConstructAccountOwner( 
  const idAccountOwner_t& idAccountOwner, const idAccountAdvisor_t& idAccountAdvisor,
    const std::string& sFirstName, const std::string& sLastName ) {

  // todo: assert that idAccountAdvisor already exists

  pAccountOwner_t p( new CAccountOwner( idAccountOwner, idAccountAdvisor, sFirstName, sLastName ) );

  iterAccountOwner_t iter = m_mapAccountOwner.find( idAccountOwner );
  if ( m_mapAccountOwner.end() != iter ) {
    throw std::runtime_error( "AccountOwner already exists" );
  }
  else {
    m_mapAccountOwner.insert( pairAccountOwner_t( idAccountOwner, p ) );
    ou::db::QueryFields<CAccountOwner::TableRowDef>::pQueryFields_t pQuery 
      = m_pSession->Insert<CAccountOwner::TableRowDef>( const_cast<CAccountOwner::TableRowDef&>( p->GetRow() ) );
  }

  return p;
}

namespace AccountManagerQueries {
  struct AccountOwnerKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "accountownerid", idAccountOwner );
    }
    const ou::tf::keytypes::idAccountOwner_t& idAccountOwner;
    AccountOwnerKey( const ou::tf::keytypes::idAccountOwner_t& idAccountOwner_ ): idAccountOwner( idAccountOwner_ ) {};
  };
}

CAccountManager::pAccountOwner_t CAccountManager::GetAccountOwner( const idAccountOwner_t& idAccountOwner ) {

  pAccountOwner_t pAccountOwner;

  iterAccountOwner_t iter = m_mapAccountOwner.find( idAccountOwner );
  if ( m_mapAccountOwner.end() != iter ) {
    pAccountOwner = iter->second;
  }
  else {
    AccountManagerQueries::AccountOwnerKey key( idAccountOwner );
    ou::db::QueryFields<AccountManagerQueries::AccountOwnerKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
      = m_pSession->SQL<AccountManagerQueries::AccountOwnerKey>( "select * from accountowners", key ).Where( "accountownerid = ?" ).NoExecute();
    m_pSession->Bind<AccountManagerQueries::AccountOwnerKey>( pExistsQuery );
    if ( m_pSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
      CAccountOwner::TableRowDef rowAccountOwner;
      m_pSession->Columns<AccountManagerQueries::AccountOwnerKey, CAccountOwner::TableRowDef>( pExistsQuery, rowAccountOwner );
      pAccountOwner.reset( new CAccountOwner( rowAccountOwner ) );
      m_mapAccountOwner.insert( pairAccountOwner_t( idAccountOwner, pAccountOwner ) );
    }
    else {
      throw std::runtime_error( "AccountOwner does not exist" );
    }
  }

  return pAccountOwner;
}

void CAccountManager::DeleteAccountOwner( const idAccountOwner_t& idAccountOwner ) {

  pAccountOwner_t pAccountOwner( GetAccountOwner( idAccountOwner ) );  // has exception if does not exist

  DeleteRecord<idAccountOwner_t, mapAccountOwner_t, AccountManagerQueries::AccountOwnerKey>( 
    idAccountOwner, m_mapAccountOwner, "accountownerid = ?" );

}

//
// Account 
//

CAccountManager::pAccount_t CAccountManager::ConstructAccount( 
  const idAccount_t& idAccount, const idAccountOwner_t& idAccountOwner,
    std::string sAccountName, keytypes::eidProvider_t idProvider, 
    std::string sBrokerName, std::string sBrokerAccountId, std::string sLogin, std::string sPassword ) {

  // todo: assert that idAccountOwner already exists

  pAccount_t p( new CAccount( idAccount, idAccountOwner, sAccountName, idProvider, sBrokerName, sBrokerAccountId, sLogin, sPassword ) );

  iterAccount_t iter = m_mapAccount.find( idAccountOwner );
  if ( m_mapAccount.end() != iter ) {
    throw std::runtime_error( "Account already exists" );
  }
  else {
    m_mapAccount.insert( pairAccount_t( idAccount, p ) );
    ou::db::QueryFields<CAccount::TableRowDef>::pQueryFields_t pQuery 
      = m_pSession->Insert<CAccount::TableRowDef>( const_cast<CAccount::TableRowDef&>( p->GetRow() ) );
  }

  return p;
}

namespace AccountManagerQueries {
  struct AccountKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "accountid", idAccount );
    }
    const ou::tf::keytypes::idAccount_t& idAccount;
    AccountKey( const ou::tf::keytypes::idAccount_t& idAccount_ ): idAccount( idAccount_ ) {};
  };
}

CAccountManager::pAccount_t CAccountManager::GetAccount( const idAccount_t& idAccount ) {

  pAccount_t pAccount;

  iterAccount_t iter = m_mapAccount.find( idAccount );
  if ( m_mapAccount.end() != iter ) {
    pAccount = iter->second;
  }
  else {
    AccountManagerQueries::AccountKey key( idAccount );
    ou::db::QueryFields<AccountManagerQueries::AccountKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
      = m_pSession->SQL<AccountManagerQueries::AccountKey>( "select * from accounts", key ).Where( "accountid = ?" ).NoExecute();
    m_pSession->Bind<AccountManagerQueries::AccountKey>( pExistsQuery );
    if ( m_pSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
      CAccount::TableRowDef rowAccount;
      m_pSession->Columns<AccountManagerQueries::AccountKey, CAccount::TableRowDef>( pExistsQuery, rowAccount );
      pAccount.reset( new CAccount( rowAccount ) );
      m_mapAccount.insert( pairAccount_t( idAccount, pAccount ) );
    }
    else {
      throw std::runtime_error( "Account does not exist" );
    }
  }

  return pAccount;
}

void CAccountManager::DeleteAccount( const idAccount_t& idAccount ) {

  pAccount_t pAccount( GetAccount( idAccount ) );  // has exception if does not exist

  DeleteRecord<idAccount_t, mapAccount_t, AccountManagerQueries::AccountKey>( 
    idAccount, m_mapAccount, "accountid = ?" );

}

//
// Table Management
//

void CAccountManager::HandleRegisterTables( ou::db::Session& session ) {
  session.RegisterTable<CAccountAdvisor::TableCreateDef>( tablenames::sAccountAdvisor );
  session.RegisterTable<CAccountOwner::TableCreateDef>( tablenames::sAccountOwner );
  session.RegisterTable<CAccount::TableCreateDef>( tablenames::sAccount );
}

void CAccountManager::HandleRegisterRows( ou::db::Session& session ) {
  session.MapRowDefToTableName<CAccountAdvisor::TableRowDef>( tablenames::sAccountAdvisor );
  session.MapRowDefToTableName<CAccountOwner::TableRowDef>( tablenames::sAccountOwner );
  session.MapRowDefToTableName<CAccount::TableRowDef>( tablenames::sAccount );
}

void CAccountManager::HandlePopulateTables( ou::db::Session& session ) {
}

// this stuff could probably be rolled into Session with a template
void CAccountManager::AttachToSession( ou::db::Session* pSession ) {
  ManagerBase::AttachToSession( pSession );
  pSession->OnRegisterTables.Add( MakeDelegate( this, &CAccountManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Add( MakeDelegate( this, &CAccountManager::HandleRegisterRows ) );
  pSession->OnPopulate.Add( MakeDelegate( this, &CAccountManager::HandlePopulateTables ) );

}

void CAccountManager::DetachFromSession( ou::db::Session* pSession ) {
  pSession->OnRegisterTables.Remove( MakeDelegate( this, &CAccountManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Remove( MakeDelegate( this, &CAccountManager::HandleRegisterRows ) );
  pSession->OnPopulate.Remove( MakeDelegate( this, &CAccountManager::HandlePopulateTables ) );
  ManagerBase::DetachFromSession( pSession );
}

} // namespace tf
} // namespace ou

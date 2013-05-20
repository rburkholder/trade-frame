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

AccountManager::AccountManager( void ) 
: ou::db::ManagerBase<AccountManager>()//, 
  //m_session( 0 )
{
}

AccountManager::~AccountManager(void) {
}

//
// Account Advisor
//

AccountManager::pAccountAdvisor_t AccountManager::ConstructAccountAdvisor( const idAccountAdvisor_t& idAdvisor, const std::string& sAdvisorName, const std::string& sCompanyName ) {

  pAccountAdvisor_t p( new AccountAdvisor( idAdvisor, sAdvisorName, sCompanyName ) );

  iterAccountAdvisor_t iter = m_mapAccountAdvisor.find( idAdvisor );
  if ( m_mapAccountAdvisor.end() != iter ) {
    throw std::runtime_error( "AccountAdvisor already exists" );
  }
  else {
    m_mapAccountAdvisor.insert( pairAccountAdvisor_t( idAdvisor, p ) );
    ou::db::QueryFields<AccountAdvisor::TableRowDef>::pQueryFields_t pQuery 
      = m_pSession->Insert<AccountAdvisor::TableRowDef>( const_cast<AccountAdvisor::TableRowDef&>( p->GetRow() ) );
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

AccountManager::pAccountAdvisor_t AccountManager::GetAccountAdvisor( const idAccountAdvisor_t& idAdvisor ) {

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
      AccountAdvisor::TableRowDef rowAccountAdvisor;
      m_pSession->Columns<AccountManagerQueries::AccountAdvisorKey, AccountAdvisor::TableRowDef>( pExistsQuery, rowAccountAdvisor );
      pAccountAdvisor.reset( new AccountAdvisor( rowAccountAdvisor ) );
      m_mapAccountAdvisor.insert( pairAccountAdvisor_t( idAdvisor, pAccountAdvisor ) );
    }
    else {
      throw std::runtime_error( "AccountAdvisor does not exist" );
    }
  }

  return pAccountAdvisor;
}

void AccountManager::DeleteAccountAdvisor( const idAccountAdvisor_t& idAccountAdvisor ) {

  pAccountAdvisor_t pAccountAdvisor( GetAccountAdvisor( idAccountAdvisor ) );  // has exception if does not exist

  DeleteRecord<idAccountAdvisor_t, mapAccountAdvisor_t, AccountManagerQueries::AccountAdvisorKey>( 
    idAccountAdvisor, m_mapAccountAdvisor, "accountadvisorid = ?" );
}

//
// Account Owner
//

AccountManager::pAccountOwner_t AccountManager::ConstructAccountOwner( 
  const idAccountOwner_t& idAccountOwner, const idAccountAdvisor_t& idAccountAdvisor,
    const std::string& sFirstName, const std::string& sLastName ) {

  // todo: assert that idAccountAdvisor already exists

  pAccountOwner_t p( new AccountOwner( idAccountOwner, idAccountAdvisor, sFirstName, sLastName ) );

  iterAccountOwner_t iter = m_mapAccountOwner.find( idAccountOwner );
  if ( m_mapAccountOwner.end() != iter ) {
    throw std::runtime_error( "AccountOwner already exists" );
  }
  else {
    m_mapAccountOwner.insert( pairAccountOwner_t( idAccountOwner, p ) );
    ou::db::QueryFields<AccountOwner::TableRowDef>::pQueryFields_t pQuery 
      = m_pSession->Insert<AccountOwner::TableRowDef>( const_cast<AccountOwner::TableRowDef&>( p->GetRow() ) );
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

AccountManager::pAccountOwner_t AccountManager::GetAccountOwner( const idAccountOwner_t& idAccountOwner ) {

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
      AccountOwner::TableRowDef rowAccountOwner;
      m_pSession->Columns<AccountManagerQueries::AccountOwnerKey, AccountOwner::TableRowDef>( pExistsQuery, rowAccountOwner );
      pAccountOwner.reset( new AccountOwner( rowAccountOwner ) );
      m_mapAccountOwner.insert( pairAccountOwner_t( idAccountOwner, pAccountOwner ) );
    }
    else {
      throw std::runtime_error( "AccountOwner does not exist" );
    }
  }

  return pAccountOwner;
}

void AccountManager::DeleteAccountOwner( const idAccountOwner_t& idAccountOwner ) {

  pAccountOwner_t pAccountOwner( GetAccountOwner( idAccountOwner ) );  // has exception if does not exist

  DeleteRecord<idAccountOwner_t, mapAccountOwner_t, AccountManagerQueries::AccountOwnerKey>( 
    idAccountOwner, m_mapAccountOwner, "accountownerid = ?" );

}

//
// Account 
//

AccountManager::pAccount_t AccountManager::ConstructAccount( 
  const idAccount_t& idAccount, const idAccountOwner_t& idAccountOwner,
    std::string sAccountName, keytypes::eidProvider_t idProvider, 
    std::string sBrokerName, std::string sBrokerAccountId, std::string sLogin, std::string sPassword ) {

  // todo: assert that idAccountOwner already exists

  pAccount_t p( new Account( idAccount, idAccountOwner, sAccountName, idProvider, sBrokerName, sBrokerAccountId, sLogin, sPassword ) );

  iterAccount_t iter = m_mapAccount.find( idAccount );
  if ( m_mapAccount.end() != iter ) {
    throw std::runtime_error( "Account already exists" );
  }
  else {
    m_mapAccount.insert( pairAccount_t( idAccount, p ) );
    ou::db::QueryFields<Account::TableRowDef>::pQueryFields_t pQuery 
      = m_pSession->Insert<Account::TableRowDef>( const_cast<Account::TableRowDef&>( p->GetRow() ) );
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

AccountManager::pAccount_t AccountManager::GetAccount( const idAccount_t& idAccount ) {

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
      Account::TableRowDef rowAccount;
      m_pSession->Columns<AccountManagerQueries::AccountKey, Account::TableRowDef>( pExistsQuery, rowAccount );
      pAccount.reset( new Account( rowAccount ) );
      m_mapAccount.insert( pairAccount_t( idAccount, pAccount ) );
    }
    else {
      throw std::runtime_error( "Account does not exist" );
    }
  }

  return pAccount;
}

void AccountManager::DeleteAccount( const idAccount_t& idAccount ) {

  pAccount_t pAccount( GetAccount( idAccount ) );  // has exception if does not exist

  DeleteRecord<idAccount_t, mapAccount_t, AccountManagerQueries::AccountKey>( 
    idAccount, m_mapAccount, "accountid = ?" );

}

//
// Table Management
//

void AccountManager::HandleRegisterTables( ou::db::Session& session ) {
  session.RegisterTable<AccountAdvisor::TableCreateDef>( tablenames::sAccountAdvisor );
  session.RegisterTable<AccountOwner::TableCreateDef>( tablenames::sAccountOwner );
  session.RegisterTable<Account::TableCreateDef>( tablenames::sAccount );
}

void AccountManager::HandleRegisterRows( ou::db::Session& session ) {
  session.MapRowDefToTableName<AccountAdvisor::TableRowDef>( tablenames::sAccountAdvisor );
  session.MapRowDefToTableName<AccountOwner::TableRowDef>( tablenames::sAccountOwner );
  session.MapRowDefToTableName<Account::TableRowDef>( tablenames::sAccount );
}

void AccountManager::HandlePopulateTables( ou::db::Session& session ) {
}

// this stuff could probably be rolled into Session with a template
void AccountManager::AttachToSession( ou::db::Session* pSession ) {
  ManagerBase::AttachToSession( pSession );
  pSession->OnRegisterTables.Add( MakeDelegate( this, &AccountManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Add( MakeDelegate( this, &AccountManager::HandleRegisterRows ) );
  pSession->OnPopulate.Add( MakeDelegate( this, &AccountManager::HandlePopulateTables ) );
}

void AccountManager::DetachFromSession( ou::db::Session* pSession ) {
  pSession->OnRegisterTables.Remove( MakeDelegate( this, &AccountManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Remove( MakeDelegate( this, &AccountManager::HandleRegisterRows ) );
  pSession->OnPopulate.Remove( MakeDelegate( this, &AccountManager::HandlePopulateTables ) );
  ManagerBase::DetachFromSession( pSession );
}

} // namespace tf
} // namespace ou

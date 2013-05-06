/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 20130407

#include "StdAfx.h"

#include "CashManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

CashManager::CashManager(void): ManagerBase<CashManager>() {
}

CashManager::~CashManager(void) {
}

CashManager::pCashAccount_t CashManager::ConstructCashAccount( const idAccount_t& idAccount, const idCurrency_t& idCurrency ) {

  // todo: assert that idAccount already exists, idCashAccount does not

  pCashAccount_t p( new CashAccount( idAccount, idCurrency ) );

  key_t key( idAccount, idCurrency );
  iterCashAccount_t iter = mapCashAccount.find( key );
  if ( mapCashAccount.end() != iter ) {
    throw std::runtime_error( "Cash Account already exists" );
  }
  else {
    mapCashAccount.insert( pairCashAccount_t( key, p ) );
    ou::db::QueryFields<CashAccount::TableRowDef>::pQueryFields_t pQuery
      = m_pSession->Insert<CashAccount::TableRowDef>( const_cast<CashAccount::TableRowDef&>( p->GetRow() ) );
  }

  return p;
}

namespace CashAccountManagerQueries {
  typedef ou::tf::keytypes::idAccount_t idAccount_t;
  typedef ou::tf::keytypes::idCurrency_t idCurrency_t;
  struct CashAccountKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "accountid", idAccount );
      ou::db::Field( a, "currencyid", idCurrency );
    }
    const ou::tf::keytypes::idAccount_t& idAccount;
    const ou::tf::keytypes::idCurrency_t& idCurrency;
    CashAccountKey( 
      const idAccount_t& idAccount_, 
      const idCurrency_t& idCurrency_ )
      : idAccount( idAccount_ ), idCurrency( idCurrency_ ) {};
    typedef ou::MultiKeyCompare<idAccount_t, idCurrency_t> key_t;
    CashAccountKey( const key_t& key ) 
      : idAccount( key.GetKey1() ), idCurrency( key.GetKey2() ) {};
  };
}

CashManager::pCashAccount_t CashManager::GetCashAccount( const idAccount_t& idAccount, const idCurrency_t& idCurrency ) {

  pCashAccount_t pCashAccount;

  key_t keyMap( idAccount, idCurrency );
  iterCashAccount_t iter = mapCashAccount.find( keyMap );
  if ( mapCashAccount.end() != iter ) {
    pCashAccount = iter->second;
  }
  else {
    CashAccountManagerQueries::CashAccountKey key( idAccount, idCurrency );
    ou::db::QueryFields<CashAccountManagerQueries::CashAccountKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
      = m_pSession->SQL<CashAccountManagerQueries::CashAccountKey>( "select * from cashaccounts", key )
      .Where( "accountid = ? and currencyid = ?" ).NoExecute();
    m_pSession->Bind<CashAccountManagerQueries::CashAccountKey>( pExistsQuery );
    if ( m_pSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
      CashAccount::TableRowDef rowCashAccount;
      m_pSession->Columns<CashAccountManagerQueries::CashAccountKey, CashAccount::TableRowDef>( pExistsQuery, rowCashAccount );
      pCashAccount.reset( new CashAccount( rowCashAccount ) );
      mapCashAccount.insert( pairCashAccount_t( keyMap, pCashAccount ) );
    }
    else {
      throw std::runtime_error( "Cash Account does not exist" );
    }
  }

  return pCashAccount;
}

void CashManager::DeleteAccount( const idAccount_t& idAccount, const idCurrency_t& idCurrency ) {

  pCashAccount_t pCashAccount( GetCashAccount( idAccount, idCurrency ) );  // has exception if does not exist

  key_t keyMap( idAccount, idCurrency );
  DeleteRecord<key_t, mapCashAccount_t, CashAccountManagerQueries::CashAccountKey>( 
    keyMap, mapCashAccount, "accountid = ? and currencyid = ?" );

}

CashManager::pCashTransaction_t CashManager::AddCashTransaction( 
    const idAccount_t& idAccount, const idCurrency_t& idCurrency, money_t mnyCredit, money_t mnyDebit, const std::string& sCode, const std::string& sDescription ) {
}

//
// Table Management
//

void CashManager::HandleRegisterTables( ou::db::Session& session ) {
  session.RegisterTable<CashAccount::TableCreateDef>( tablenames::sCashAccount );
}

void CashManager::HandleRegisterRows( ou::db::Session& session ) {
  session.MapRowDefToTableName<CashAccount::TableRowDef>( tablenames::sCashAccount );
}

void CashManager::HandlePopulateTables( ou::db::Session& session ) {
  // populate with default currency: USD?  This should therefore be overridden somewhere as default currency 
  //  is different in various jurisdictions
  // should come from a TLV table?
}

// this stuff could probably be rolled into Session with a template
void CashManager::AttachToSession( ou::db::Session* pSession ) {
  ManagerBase::AttachToSession( pSession );
  pSession->OnRegisterTables.Add( MakeDelegate( this, &CashManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Add( MakeDelegate( this, &CashManager::HandleRegisterRows ) );
  pSession->OnPopulate.Add( MakeDelegate( this, &CashManager::HandlePopulateTables ) );
}

void CashManager::DetachFromSession( ou::db::Session* pSession ) {
  pSession->OnRegisterTables.Remove( MakeDelegate( this, &CashManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Remove( MakeDelegate( this, &CashManager::HandleRegisterRows ) );
  pSession->OnPopulate.Remove( MakeDelegate( this, &CashManager::HandlePopulateTables ) );
  ManagerBase::DetachFromSession( pSession );
}



} // namespace tf
} // namespace ou

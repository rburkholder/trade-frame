/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

// started 2013-05-21

#include <OUCommon/TimeSource.h>

#include "AccountsManager.h"

namespace ou { // One Unified
namespace dea { // double entry accounting

AccountsManager::AccountsManager(void): ou::db::ManagerBase<AccountsManager>() {
}

AccountsManager::~AccountsManager(void) {
}

// Chart Of Accounts

namespace AccountsManagerQueries {
  struct AccountKey {
    typedef AccountsManager::idAccount_t idAccount_t;
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "accountid", idAccount );
    }
    const idAccount_t& idAccount;
    AccountKey( const idAccount_t& idAccount_ ): idAccount( idAccount_ ) {};
  };
}  // namespace

bool AccountsManager::LocateAccount( const idAccount_t& idAccount, pAccount_t& pAccount ) {
  bool bFound = false;
  if ( 0 != m_pSession ) {
    AccountsManagerQueries::AccountKey keyAccount( idAccount );
    ou::db::QueryFields<AccountsManagerQueries::AccountKey>::pQueryFields_t pAccountQuery
      = m_pSession->SQL<AccountsManagerQueries::AccountKey>( "select * from chartofaccounts", keyAccount ).Where( "accountid=?" ).NoExecute();
    m_pSession->Bind<AccountsManagerQueries::AccountKey>( pAccountQuery );
    if ( m_pSession->Execute( pAccountQuery ) ) {
      Account::TableRowDef rowAccount;
      m_pSession->Columns<AccountsManagerQueries::AccountKey, Account::TableRowDef>( pAccountQuery, rowAccount );
      pAccount.reset( new Account( rowAccount ) );
      bFound = true;
    }
  }
  return bFound;
}

AccountsManager::pAccount_t AccountsManager::ConstructAccount(
  const idAccount_t& idAccount, const idCurrency_t& idCurrency,
  const std::string& sLocation, const std::string& sDepartment,
  const std::string& sCategory, const std::string& sSubCategory, const std::string& sDescription ) {
  pAccount_t pAccount;
  if ( 0 != m_pSession ) {
    if ( LocateAccount( idAccount, pAccount ) ) throw std::runtime_error( "account exists" );
    else {
      pAccount.reset( new Account( Account::TableRowDef(
        idAccount, idCurrency, sLocation, sDepartment,
        sCategory, sSubCategory, sDescription ) ) );
      ou::db::QueryFields<Account::TableRowDef>::pQueryFields_t pQuery
        = m_pSession->Insert<Account::TableRowDef>( const_cast<Account::TableRowDef&>( pAccount->GetRow() ) );
    }
  }
  return pAccount;
}

// General Journal Entry

AccountsManager::pGLEntry_t AccountsManager::ConstructGeneralJournalEntry(
  const idReference_t& idReference, const idAccount_t& idAccount, const idUser_t& idUser,
  money_t mnyDebit, money_t mnyCredit,
  const std::string& sCode, const std::string& sDescription ) {
  pGLEntry_t pGLEntry;
  if ( 0 != m_pSession ) {
    pGLEntry.reset(
      new GeneralJournalEntry(
        0, GeneralJournalEntry::TableRowDefNoKey( idReference, idAccount, idUser, mnyDebit, mnyCredit, sCode, sDescription ) ) );
    pGLEntry->SetCreationTime( ou::TimeSource::LocalCommonInstance().Internal() );
    ou::db::QueryFields<GeneralJournalEntry::TableRowDefNoKey>::pQueryFields_t pQueryWrite
      = m_pSession->Insert<GeneralJournalEntry::TableRowDefNoKey>(
        const_cast<GeneralJournalEntry::TableRowDefNoKey&>( dynamic_cast<const GeneralJournalEntry::TableRowDefNoKey&>( pGLEntry->GetRow() ) ) );
    idTransaction_t idTransaction = m_pSession->GetLastRowId();
  }
  return pGLEntry;
}

void AccountsManager::HandleRegisterTables( ou::db::Session& session ) {
  session.RegisterTable<Account::TableCreateDef>( tablenames::sGeneralJournal );
  session.RegisterTable<GeneralJournalEntry::TableCreateDef>( tablenames::sGeneralJournal );
}

void AccountsManager::HandleRegisterRows( ou::db::Session& session ) {
  session.MapRowDefToTableName<Account::TableRowDef>( tablenames::sChartOfAccounts );
  session.MapRowDefToTableName<GeneralJournalEntry::TableRowDef>( tablenames::sGeneralJournal );
  session.MapRowDefToTableName<GeneralJournalEntry::TableRowDefNoKey>( tablenames::sGeneralJournal );
}

void AccountsManager::HandlePopulateTables( ou::db::Session& session ) {
}

void AccountsManager::AttachToSession( ou::db::Session* pSession ) {
  ManagerBase::AttachToSession( pSession );
  pSession->OnRegisterTables.Add( MakeDelegate( this, &AccountsManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Add( MakeDelegate( this, &AccountsManager::HandleRegisterRows ) );
  pSession->OnPopulate.Add( MakeDelegate( this, &AccountsManager::HandlePopulateTables ) );
}

void AccountsManager::DetachFromSession( ou::db::Session* pSession ) {
  pSession->OnRegisterTables.Remove( MakeDelegate( this, &AccountsManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Remove( MakeDelegate( this, &AccountsManager::HandleRegisterRows ) );
  pSession->OnPopulate.Remove( MakeDelegate( this, &AccountsManager::HandlePopulateTables ) );
  ManagerBase::DetachFromSession( pSession );
}


} // namespace dea
} // namespace ou
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

#pragma once

#include <OUCommon/ManagerBase.h>
#include <OUCommon/CurrencyCode.h>

#include "KeyTypes.h"

#include "Account.h"
#include "GeneralJournalEntry.h"

namespace ou { // One Unified
namespace dea { // double entry accounting

class AccountsManager: public ou::db::ManagerBase<AccountsManager> {
public:

  typedef ou::tables::CurrencyCode::idCurrency_t idCurrency_t;

  typedef keytypes::idAccount_t idAccount_t;
  typedef Account::pAccount_t pAccount_t;
  typedef ou::auth::keytypes::idUser_t idUser_t;

  typedef keytypes::idTransaction_t idTransaction_t;
  typedef keytypes::idReference_t idReference_t;
  typedef keytypes::money_t money_t;
  typedef GeneralJournalEntry::pGLEntry_t pGLEntry_t;

  pAccount_t ConstructAccount( // throws
    const idAccount_t& idAccount, const idCurrency_t& idCurrency, 
    const std::string& sLocation, const std::string& sDepartment, 
    const std::string& sCategory, const std::string& sSubCategory, const std::string& sDescription );
  pGLEntry_t ConstructGeneralJournalEntry( 
    const idReference_t& idReference, const idAccount_t& idAccount, const idUser_t& idUser, 
    money_t mnyDebit, money_t mnyCredit,
    const std::string& sCode, const std::string& sDescription );

  bool LocateAccount( const idAccount_t& idAccount, pAccount_t& pAccount );

  AccountsManager(void);
  ~AccountsManager(void);

  void AttachToSession( ou::db::Session* pSession );
  void DetachFromSession( ou::db::Session* pSession );

protected:
private:
  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );
  void HandlePopulateTables( ou::db::Session& session );
};

} // namespace dea
} // namespace ou
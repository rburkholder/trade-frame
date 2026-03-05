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

#pragma once

#include <map>

#include <OUCommon/ManagerBase.h>
#include <OUCommon/MultiKeyCompare.h>

#include "KeyTypes.h"

#include "Account.h"
#include "CashAccount.h"
#include "CashTransaction.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CashManager: public ou::db::ManagerBase<CashManager> {
public:

  typedef keytypes::idAccount_t idAccount_t;
  typedef Account::pAccount_t pAccount_t;

  typedef keytypes::idCurrency_t idCurrency_t;

  typedef CashTransaction::money_t money_t;

  typedef CashAccount::pCashAccount_t pCashAccount_t;
  typedef CashTransaction::pCashTransaction_t pCashTransaction_t;

  CashManager();
  ~CashManager();

  pCashAccount_t ConstructCashAccount( const idAccount_t& idAccount, const idCurrency_t& idCurrency );
  pCashAccount_t GetCashAccount( const idAccount_t& idAccount, const idCurrency_t& idCurrency );
  void DeleteAccount( const idAccount_t& idAccount, const idCurrency_t& idCurrency );

//  pCashTransaction_t AddCashTransaction(
//    const idAccount_t& idAccount, const idCurrency_t& idCurrency, money_t mnyCredit, money_t mnyDebit, const std::string& sCode, const std::string& sDescription );

  void AttachToSession( ou::db::Session* pSession );
  void DetachFromSession( ou::db::Session* pSession );

protected:
private:

  using key_t = ou::MultiKeyCompare<idAccount_t, idCurrency_t>;
  using pairCashAccount_t = std::pair<key_t, pCashAccount_t>;
  using mapCashAccount_t = std::map<key_t, pCashAccount_t>;
  using iterCashAccount_t = mapCashAccount_t::iterator;
  mapCashAccount_t mapCashAccount;

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );
  void HandlePopulateTables( ou::db::Session& session );

};

} // namespace tf
} // namespace ou

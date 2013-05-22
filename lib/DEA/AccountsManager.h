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

#include "KeyTypes.h"

#include "ChartOfAccounts.h"
#include "GeneralJournal.h"

namespace ou { // One Unified
namespace dea { // double entry accounting

class AccountsManager: public ou::db::ManagerBase<AccountsManager> {
public:

  typedef keytypes::idAccount_t idAccount_t;
  typedef ChartOfAccounts::pChartOfAccounts_t pChartOfAccounts_t;

  typedef keytypes::idTransaction_t idTransaction_t;
  typedef GeneralJournal::pGeneralJournal_t pGeneralJournal_t;

  //pChartOfAccounts_t ConstructAccount

  AccountsManager(void);
  ~AccountsManager(void);
protected:
private:
};

} // namespace dea
} // namespace ou
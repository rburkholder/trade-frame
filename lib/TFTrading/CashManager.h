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

#include "KeyTypes.h"

#include "ManagerBase.h"
#include "CashAccount.h"
#include "CashTransaction.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CashManager: public ManagerBase<CashManager> {
public:
  CashManager(void);
  ~CashManager(void);
protected:
private:
};

} // namespace tf
} // namespace ou

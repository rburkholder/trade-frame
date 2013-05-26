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

#include <OUCommon/ManagerBase.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class RiskManager: public ou::db::ManagerBase<RiskManager> {
public:
  RiskManager(void);
  ~RiskManager(void);
protected:
private:
};

} // namespace tf
} // namespace ou

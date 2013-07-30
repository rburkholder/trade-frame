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

// Started 2013/07/29

#pragma once

#include "PanelPortfolioPositionOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class ControllerPortfolioPositionOrderExecution {
public:
  ControllerPortfolioPositionOrderExecution( PanelPortfolioPositionOrderExecution* );
  ~ControllerPortfolioPositionOrderExecution(void);
protected:
private:

  PanelPortfolioPositionOrderExecution* m_ppppoe;

  void HandlePanelPortfolioPositionOrderExecutionClose( PanelPortfolioPositionOrderExecution* );

};

} // namespace tf
} // namespace ou

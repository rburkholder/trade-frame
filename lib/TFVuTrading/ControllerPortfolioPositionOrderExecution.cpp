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

#include "StdAfx.h"

#include "ControllerPortfolioPositionOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

ControllerPortfolioPositionOrderExecution::ControllerPortfolioPositionOrderExecution( PanelPortfolioPositionOrderExecution* pppoe ) 
  : m_ppppoe( pppoe )
{
}

ControllerPortfolioPositionOrderExecution::~ControllerPortfolioPositionOrderExecution(void) {
}

void ControllerPortfolioPositionOrderExecution::HandlePanelPortfolioPositionOrderExecutionClose( PanelPortfolioPositionOrderExecution* ) {
  m_ppppoe = 0;
  // also maybe set a flag for runtime issue checking
  // but not much more can happen with out event stimulus from the panel
}

} // namespace tf
} // namespace ou

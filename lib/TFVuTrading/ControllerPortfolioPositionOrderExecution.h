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

#include "ModelPortfolioPositionOrderExecution.h"
#include "PanelPortfolioPositionOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class ControllerPortfolioPositionOrderExecution {
public:

  typedef ModelPortfolioPositionOrderExecution MPPOE_t;
  typedef PanelPortfolioPositionOrderExecution PPPOE_t;

  ControllerPortfolioPositionOrderExecution( MPPOE_t*, PPPOE_t* );
  ~ControllerPortfolioPositionOrderExecution(void);
protected:
private:

  MPPOE_t* m_pMPPOE;
  PPPOE_t* m_pPPPOE;

  void HandlePanelPortfolioPositionOrderExecutionClose( PPPOE_t* );

  void HandleDVSelectionChanged( wxDataViewEvent& event );
  void HandleDVItemCollapsed( wxDataViewEvent& event );
  void HandleDVItemExpanded( wxDataViewEvent& event );
  void HandleDVCollapsing( wxDataViewEvent& event );
  void HandleDVExpanding( wxDataViewEvent& event );
  void HandleDVContextMenu( wxDataViewEvent& event );

};

} // namespace tf
} // namespace ou

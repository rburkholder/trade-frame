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

#include <wx/dataview.h>

#include "ControllerPortfolioPositionOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

ControllerPortfolioPositionOrderExecution::ControllerPortfolioPositionOrderExecution( MPPOE_t* pMPPOE, PPPOE_t* pPPPOE  ) 
  : m_pMPPOE( pMPPOE ), m_pPPPOE( pPPPOE )
{
  m_pMPPOE->LoadMasterPortfolio();
  m_pPPPOE->GetTree()->Bind( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, &ControllerPortfolioPositionOrderExecution::HandleDVSelectionChanged, this );
  m_pPPPOE->GetTree()->Bind( wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED, &ControllerPortfolioPositionOrderExecution::HandleDVItemCollapsed, this );
  m_pPPPOE->GetTree()->Bind( wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED, &ControllerPortfolioPositionOrderExecution::HandleDVItemExpanded, this );
  m_pPPPOE->GetTree()->Bind( wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSING, &ControllerPortfolioPositionOrderExecution::HandleDVCollapsing, this );
  m_pPPPOE->GetTree()->Bind( wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING, &ControllerPortfolioPositionOrderExecution::HandleDVExpanding, this );
  m_pPPPOE->GetTree()->Bind( wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &ControllerPortfolioPositionOrderExecution::HandleDVContextMenu, this );
}

ControllerPortfolioPositionOrderExecution::~ControllerPortfolioPositionOrderExecution(void) {
}

void ControllerPortfolioPositionOrderExecution::HandlePanelPortfolioPositionOrderExecutionClose( PanelPortfolioPositionOrderExecution* ) {
  m_pMPPOE = 0;
  m_pPPPOE = 0;
  // also maybe set a flag for runtime issue checking
  // but not much more can happen with out event stimulus from the panel
}

void ControllerPortfolioPositionOrderExecution::HandleDVSelectionChanged( wxDataViewEvent& event ) {
  //ModelBase::DataViewItemBase* 
  //event.GetItem()
  //switch 
  //;
  m_pMPPOE->ClickedOnTreeItem( event.GetItem().GetID() );
}

void ControllerPortfolioPositionOrderExecution::HandleDVItemCollapsed( wxDataViewEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVItemExpanded( wxDataViewEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVCollapsing( wxDataViewEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVExpanding( wxDataViewEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVContextMenu( wxDataViewEvent& event ) {
}


} // namespace tf
} // namespace ou

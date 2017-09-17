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

#include "stdafx.h"

#include <wx/dataview.h>

#include "ControllerPortfolioPositionOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

ControllerPortfolioPositionOrderExecution::ControllerPortfolioPositionOrderExecution( MPPOE_t* pMPPOE, PPPOE_t* pPPPOE  ) 
  : m_pMPPOE( pMPPOE ), m_pPPPOE( pPPPOE )
{
  //m_pMPPOE->LoadMasterPortfolio();
  m_pPPPOE->GetTree()->Bind( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, &ControllerPortfolioPositionOrderExecution::HandleDVSelectionChanged, this );
  m_pPPPOE->GetTree()->Bind( wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED, &ControllerPortfolioPositionOrderExecution::HandleDVItemCollapsed, this );
  m_pPPPOE->GetTree()->Bind( wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED, &ControllerPortfolioPositionOrderExecution::HandleDVItemExpanded, this );
  m_pPPPOE->GetTree()->Bind( wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSING, &ControllerPortfolioPositionOrderExecution::HandleDVCollapsing, this );
  m_pPPPOE->GetTree()->Bind( wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING, &ControllerPortfolioPositionOrderExecution::HandleDVExpanding, this );
  m_pPPPOE->GetTree()->Bind( wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &ControllerPortfolioPositionOrderExecution::HandleDVContextMenuCreate, this );
  // wxEVT_CONTEXT_MENU, wxContextMenuEvent
  //  wxEVT_COMMAND_MENU_SELECTED, wxCommandEvent
  m_pPPPOE->GetTree()->Bind( 
    wxEVT_COMMAND_MENU_SELECTED, 
    &ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickPortfolioMasterAddPortfolioCurrencySummary, this, 
    PanelPortfolioPositionOrderExecution::eTreeCMPortfolioMasterAddCurrencySummary );
  m_pPPPOE->GetTree()->Bind( 
    wxEVT_COMMAND_MENU_SELECTED, 
    &ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickPortfolioCurrencyAddPortfolio, this, 
    PanelPortfolioPositionOrderExecution::eTreeCMPortfolioCurrencyAddPortfolio );
  m_pPPPOE->GetTree()->Bind( 
    wxEVT_COMMAND_MENU_SELECTED, 
    &ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickPortfolioCurrencyAddPosition, this, 
    PanelPortfolioPositionOrderExecution::eTreeCMPortfolioCurrencyAddPosition );
  m_pPPPOE->GetTree()->Bind( 
    wxEVT_COMMAND_MENU_SELECTED, 
    &ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickPortfolioAddPortfolio, this, 
    PanelPortfolioPositionOrderExecution::eTreeCMPortfolioAddPortfolio );
  m_pPPPOE->GetTree()->Bind( 
    wxEVT_COMMAND_MENU_SELECTED, 
    &ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickPortfolioAddPosition, this, 
    PanelPortfolioPositionOrderExecution::eTreeCMPortfolioAddPosition );
  m_pPPPOE->GetTree()->Bind( 
    wxEVT_COMMAND_MENU_SELECTED, 
    &ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickPositionCreateOrder, this, 
    PanelPortfolioPositionOrderExecution::eTreeCMPositionCreateOrder );
  m_pPPPOE->GetTree()->Bind( 
    wxEVT_COMMAND_MENU_SELECTED, 
    &ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickPositionClosePosition, this, 
    PanelPortfolioPositionOrderExecution::eTreeCMPositionClosePosition );
  m_pPPPOE->GetTree()->Bind( 
    wxEVT_COMMAND_MENU_SELECTED, 
    &ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickOrderCancelOrder, this, 
    PanelPortfolioPositionOrderExecution::eTreeCMOrderCancelOrder );
}

ControllerPortfolioPositionOrderExecution::~ControllerPortfolioPositionOrderExecution(void) {
}

void ControllerPortfolioPositionOrderExecution::LoadInitialData( void ) {
  ou::tf::PortfolioManager::Instance().LoadActivePortfolios();
}

void ControllerPortfolioPositionOrderExecution::HandlePanelPortfolioPositionOrderExecutionClose( PanelPortfolioPositionOrderExecution* ) {
  m_pMPPOE = nullptr;
  m_pPPPOE = nullptr;
  // also maybe set a flag for runtime issue checking
  // but not much more can happen with out event stimulus from the panel
}

void ControllerPortfolioPositionOrderExecution::HandleDVSelectionChanged( wxDataViewEvent& event ) {
  m_dvLastClickedItem = event.GetItem();
  m_pMPPOE->ClickedOnTreeItem( reinterpret_cast<DataViewItemBase*>( m_dvLastClickedItem.GetID() ) );
}

void ControllerPortfolioPositionOrderExecution::HandleDVItemCollapsed( wxDataViewEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVItemExpanded( wxDataViewEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVCollapsing( wxDataViewEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVExpanding( wxDataViewEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVContextMenuCreate( wxDataViewEvent& event ) {
  m_dvItem = event.GetItem();  // used when the context menu item is clicked
  m_pPPPOE->ClickedOnTreeContextMenu( m_pMPPOE->GetModelType( m_dvItem ) );
}

// ==============

void ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickPortfolioMasterAddPortfolioCurrencySummary( wxCommandEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickPortfolioCurrencyAddPortfolio( wxCommandEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickPortfolioCurrencyAddPosition( wxCommandEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickPortfolioAddPortfolio( wxCommandEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickPortfolioAddPosition( wxCommandEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickPositionCreateOrder( wxCommandEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickPositionClosePosition( wxCommandEvent& event ) {
}

void ControllerPortfolioPositionOrderExecution::HandleDVContextMenuClickOrderCancelOrder( wxCommandEvent& event ) {
}


} // namespace tf
} // namespace ou

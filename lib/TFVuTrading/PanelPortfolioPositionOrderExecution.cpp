/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <wx/splitter.h>

#include "PanelPortfolioPositionOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// need to fix this constructor for proper use of xwPanel inherit
PanelPortfolioPositionOrderExecution::PanelPortfolioPositionOrderExecution(void) {
}

PanelPortfolioPositionOrderExecution::~PanelPortfolioPositionOrderExecution(void) {
}

void PanelPortfolioPositionOrderExecution::CreateControls( void ) {

  // size for encompassing panel
  wxBoxSizer* pSizerPanelOuter = new wxBoxSizer( wxVERTICAL );
  SetSizer( pSizerPanelOuter );

  // splitter for Portfolio/Position & Order/Execution Regions
  wxSplitterWindow* pSplitOuter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER );
  pSplitOuter->SetMinimumPaneSize( 20 );
  pSizerPanelOuter->Add( pSplitOuter, 1, wxEXPAND | wxALL, 5 );

  // panel a) Portfolio / Position
  wxPanel* pPanelPortfolioPosition = new wxPanel( pSplitOuter, wxID_ANY );
  wxBoxSizer* pSizerPanelPortfolioPosition = new wxBoxSizer( wxVERTICAL );
  pPanelPortfolioPosition->SetSizer( pSizerPanelPortfolioPosition );

  // panel b) Order / Execution
  wxPanel* pPanelOrderExecution = new wxPanel( pSplitOuter, wxID_ANY );
  wxBoxSizer* pSizerPanelOrderExecution = new wxBoxSizer( wxVERTICAL );
  pPanelOrderExecution->SetSizer( pSizerPanelOrderExecution );

  // add panels to outer splitter
  pSplitOuter->Initialize( pPanelPortfolioPosition );
  pSplitOuter->SplitHorizontally( pPanelPortfolioPosition, pPanelOrderExecution );

  // splitter in panel a
  wxSplitterWindow* pSplitPortfolioPosition = new wxSplitterWindow( pPanelPortfolioPosition, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER );
  pSplitPortfolioPosition->SetMinimumPaneSize( 20 );
  pSizerPanelPortfolioPosition->Add( pSplitPortfolioPosition, 1, wxEXPAND | wxALL, 5 );

  // splitter in panel b
  wxSplitterWindow* pSplitOrderExecution = new wxSplitterWindow( pPanelOrderExecution, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER );
  pSplitOrderExecution->SetMinimumPaneSize( 20 );
  pSizerPanelOrderExecution->Add( pSplitOrderExecution, 1, wxEXPAND | wxALL, 5 );

  // Portfolios & Positions
  VuPortfolios* pDVPortfolios = new VuPortfolios( pSplitPortfolioPosition, wxID_ANY );
  VuPositions* pDVPositions = new VuPositions( pSplitPortfolioPosition, wxID_ANY );
  pSplitPortfolioPosition->Initialize( pDVPortfolios );
  pSplitPortfolioPosition->SplitHorizontally( pDVPortfolios, pDVPositions );
  pPanelPortfolioPosition->Show( true );

  // Orders & Executions
  VuOrders* pDVOrders = new VuOrders( pSplitOrderExecution, wxID_ANY );
  VuExecutions* pDVExecutions = new VuExecutions( pSplitOrderExecution, wxID_ANY );
  pSplitOrderExecution->Initialize( pDVOrders );
  pSplitOrderExecution->SplitHorizontally( pDVOrders, pDVExecutions );
  pPanelOrderExecution->Show( true );

  Show( true );

}

} // namespace tf
} // namespace ou

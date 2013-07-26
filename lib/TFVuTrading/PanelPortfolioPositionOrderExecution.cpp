 /************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                  8                    *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#include "StdAfx.h"

#include <wx/splitter.h>
#include <wx/treectrl.h>

#include "PanelPortfolioPositionOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// need to fix this constructor for proper use of wxPanel inherit
PanelPortfolioPositionOrderExecution::PanelPortfolioPositionOrderExecution(void) {
  Init();
}

PanelPortfolioPositionOrderExecution::PanelPortfolioPositionOrderExecution( /*wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style*/
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style
  )
{
  Init();
  Create(parent, id, pos, size, style);
}

PanelPortfolioPositionOrderExecution::~PanelPortfolioPositionOrderExecution(void) {
}

bool PanelPortfolioPositionOrderExecution::Create( /*wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style*/
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style
  ) {

    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}

void PanelPortfolioPositionOrderExecution::Init( void ) {
}

void PanelPortfolioPositionOrderExecution::CreateControls( void ) {

  // panels should have splitter as the parent
  // http://wiki.wxwidgets.org/WxSplitterWindow

  PanelPortfolioPositionOrderExecution* pPanelOuter = this;

  // size for encompassing panel
  wxBoxSizer* pSizerPanelOuter = new wxBoxSizer( wxHORIZONTAL );
  pPanelOuter->SetSizer( pSizerPanelOuter );

  // splitter for Left Tree and Right Tables
  wxSplitterWindow* pSplitTreeAndTables = new wxSplitterWindow( pPanelOuter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER );
  pSplitTreeAndTables->SetMinimumPaneSize( 20 );
  pSizerPanelOuter->Add( pSplitTreeAndTables, 1, wxEXPAND | wxALL, 5 );

  // panel for tree
  wxPanel* pPanelTree = new wxPanel( pSplitTreeAndTables, wxID_ANY );
  wxBoxSizer* pSizerPanelTree = new wxBoxSizer( wxVERTICAL );
  pPanelTree->SetSizer( pSizerPanelTree );

  // add tree to left panel, redo the object at some point
  TreePortfolioPositionOrder* pTree = new TreePortfolioPositionOrder( pSplitTreeAndTables, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_ROW_LINES , wxDefaultValidator, "Test Tree" );
  pSizerPanelTree->Add( pTree, 1, wxEXPAND | wxALL, 1 );

  // panel for tables
  wxPanel* pPanelTables = new wxPanel( pSplitTreeAndTables, wxID_ANY );
  wxBoxSizer* pSizerPanelTables = new wxBoxSizer( wxVERTICAL );
  pPanelTables->SetSizer( pSizerPanelTables );

  // add panels to splitter
  pSplitTreeAndTables->Initialize( pPanelTree );
  pSplitTreeAndTables->SplitVertically( pPanelTree, pPanelTables );

  // splitter for Portfolio/Position & Order/Execution Regions
  wxSplitterWindow* pSplitTablesOuter = new wxSplitterWindow( pPanelTables, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER );
  pSplitTablesOuter->SetMinimumPaneSize( 20 );
  pSplitTablesOuter->SetSashGravity( 0.5 );
  pSizerPanelTables->Add( pSplitTablesOuter, 1, wxEXPAND | wxALL, 2 );

  // panel a) Portfolio & Position
  wxPanel* pPanelPortfolioPosition = new wxPanel( pSplitTablesOuter, wxID_ANY );
  wxBoxSizer* pSizerPanelPortfolioPosition = new wxBoxSizer( wxVERTICAL );
  pPanelPortfolioPosition->SetSizer( pSizerPanelPortfolioPosition );

  // panel b) Order & Execution
  wxPanel* pPanelOrderExecution = new wxPanel( pSplitTablesOuter, wxID_ANY );
  wxBoxSizer* pSizerPanelOrderExecution = new wxBoxSizer( wxVERTICAL );
  pPanelOrderExecution->SetSizer( pSizerPanelOrderExecution );

  // add panels to outer splitter
  pSplitTablesOuter->Initialize( pPanelPortfolioPosition );
  pSplitTablesOuter->SplitHorizontally( pPanelPortfolioPosition, pPanelOrderExecution );

  // splitter in panel a
  wxSplitterWindow* pSplitPortfolioPosition = new wxSplitterWindow( pPanelPortfolioPosition, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER );
  pSplitPortfolioPosition->SetMinimumPaneSize( 50 );
  pSplitPortfolioPosition->SetSashGravity( 0.5 );
  pSizerPanelPortfolioPosition->Add( pSplitPortfolioPosition, 1, wxEXPAND | wxALL, 1 );

  // splitter in panel b
  wxSplitterWindow* pSplitOrderExecution = new wxSplitterWindow( pPanelOrderExecution, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER );
  pSplitOrderExecution->SetMinimumPaneSize( 50 );
  pSplitOrderExecution->SetSashGravity( 0.5 );
  pSizerPanelOrderExecution->Add( pSplitOrderExecution, 1, wxEXPAND | wxALL, 1 );

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

void PanelPortfolioPositionOrderExecution::OnClose( wxCloseEvent& event ) {
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

void PanelPortfolioPositionOrderExecution::OnFocusChange( wxFocusEvent& event ) {
//  if ( 0 != OnFocusPropogate ) OnFocusPropogate( m_ixStruct );
}



} // namespace tf
} // namespace ou

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

#include "stdafx.h"

#include <wx/splitter.h>



#include "PanelFinancialChart.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelFinancialChart::PanelFinancialChart(void) 
: wxPanel(), m_pWinChartView( nullptr )
{
  Init();
}

PanelFinancialChart::PanelFinancialChart( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxPanel(), m_pWinChartView( nullptr )
{
  Init();
  Create(parent, id, pos, size, style);
}

PanelFinancialChart::~PanelFinancialChart(void) {
  m_pWinChartView->SetChartDataView( nullptr );
}

bool PanelFinancialChart::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer())     {
      GetSizer()->SetSizeHints(this);
  }
  //Centre();
  return true;
}

void PanelFinancialChart::Init() {
}

void PanelFinancialChart::CreateControls() {    

  PanelFinancialChart* itemPanel1 = this;

  wxBoxSizer* sizerMain = new wxBoxSizer( wxVERTICAL );
  itemPanel1->SetSizer( sizerMain );

  // splitter
  wxSplitterWindow* pSplitter;
  pSplitter = new wxSplitterWindow( this );
  pSplitter->SetMinimumPaneSize(10);
  pSplitter->SetSashGravity(0.2);

  // tree
  //wxTreeCtrl* tree;
  m_pTree = new wxTreeCtrl( pSplitter );
  //m_eLatestDatumType = CustomItemData::NoDatum;
  wxTreeItemId idRoot = m_pTree->AddRoot( "Total P/L", -1, -1, new CustomItemData( CustomItemData::PL ) );
  m_pTree->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &PanelFinancialChart::HandleTreeEventItemActivated, this, m_pTree->GetId() );
  //m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId() );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_MENU, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  tree->AppendItem( idRoot, "second" );
//  tree->AppendItem( idRoot, "third" );

  // panel for right side of splitter
  wxPanel* panelSplitterRightPanel;
  panelSplitterRightPanel = new wxPanel( pSplitter );

  pSplitter->SplitVertically( m_pTree, panelSplitterRightPanel, 0 );
  sizerMain->Add( pSplitter, 1, wxGROW|wxALL, 2);

  // sizer for right side of splitter
  wxBoxSizer* sizerRight;
  sizerRight = new wxBoxSizer( wxVERTICAL );
  panelSplitterRightPanel->SetSizer( sizerRight );

  m_pWinChartView = new WinChartView( panelSplitterRightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  sizerRight->Add( m_pWinChartView, 1, wxALL|wxEXPAND, 5);

  Bind( wxEVT_CLOSE_WINDOW, &PanelFinancialChart::OnClose, this );  // start close of windows and controls
}

void PanelFinancialChart::HandleTreeEventItemActivated( wxTreeEvent& event ) {
  wxTreeItemId id = event.GetItem();
  CustomItemData* pdata = dynamic_cast<CustomItemData*>( m_pTree->GetItemData( id ) );
}

void PanelFinancialChart::OnClose( wxCloseEvent& event ) {
  event.Skip();  // auto followed by Destroy();
}

wxBitmap PanelFinancialChart::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon PanelFinancialChart::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}

} // namespace tf
} // namespace ou

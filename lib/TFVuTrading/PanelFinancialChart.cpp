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
  wxSplitterWindow* pSplitter = new wxSplitterWindow( this );
  pSplitter->SetMinimumPaneSize(200);
  pSplitter->SetSashGravity(0.2);

  // tree
  //wxTreeCtrl* tree;
  m_pTree = new wxTreeCtrl( pSplitter );
  //m_eLatestDatumType = CustomItemData::NoDatum;
  //wxTreeItemId idRoot = m_pTree->AddRoot( "Total P/L", -1, -1, new CustomItemData( CustomItemData::PL ) );
  m_pTree->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &PanelFinancialChart::HandleTreeEventItemActivated, this, m_pTree->GetId() );
  //m_pTree->Bind( wxEVT_TREE_DELETE_ITEM, &PanelFinancialChart::HandleTreeEventItemDeleted, this, m_pTree->GetId() );
  //m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId() );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_MENU, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  tree->AppendItem( idRoot, "second" );
//  tree->AppendItem( idRoot, "third" );

  // panel for right side of splitter
  //wxPanel* panelSplitterRightPanel;
  //panelSplitterRightPanel = new wxPanel( pSplitter );

  m_pWinChartView = new WinChartView( pSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );

  pSplitter->SplitVertically( m_pTree, m_pWinChartView, 0 );
  sizerMain->Add( pSplitter, 1, wxEXPAND|wxALL, 2);

  // sizer for right side of splitter
  //wxBoxSizer* sizerRight = new wxBoxSizer( wxVERTICAL );
  //panelSplitterRightPanel->SetSizer( sizerRight );

  //sizerRight->Add( m_pWinChartView, 1, wxEXPAND|wxALL, 2);

  //pSplitter->SetSashPosition()

  Bind( wxEVT_CLOSE_WINDOW, &PanelFinancialChart::OnClose, this );  // start close of windows and controls
}

void PanelFinancialChart::Append( const std::string& sName, ou::ChartDataView::pChartDataView_t pChartDataView ) {
  // NOTE: CustomItemData is automatically deleted during destruction of Item.
  wxTreeItemId idRoot = m_pTree->GetRootItem();
  if ( idRoot.IsOk() ) {
    wxTreeItemId idCurrent = m_pTree->AppendItem( idRoot, sName, -1, -1, new CustomItemData( pChartDataView ) );
  }
  else {
    idRoot = m_pTree->AddRoot( sName, -1, -1, new CustomItemData( pChartDataView ) );
  }
}

void PanelFinancialChart::HandleTreeEventItemActivated( wxTreeEvent& event ) {
  wxTreeItemId id = event.GetItem();
  CustomItemData* pdata = dynamic_cast<CustomItemData*>( m_pTree->GetItemData( id ) );
  if ( pdata->m_pChartDataView ) {
    m_pWinChartView->SetChartDataView( pdata->m_pChartDataView.get() );
  }
  else {
    std::cout << "no chart data" << std::endl;
  }
  event.Skip();
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

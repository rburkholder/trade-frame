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

#include <stdexcept>

#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>

#include "PanelFinancialChart.h"

namespace {

  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  class CustomItemData: public wxTreeItemData {
  public:
    wxMenu* pMenuPopup;
    pChartDataView_t pChartDataView; // contains the various time series
    CustomItemData( pChartDataView_t pChartDataView_ )
    : pChartDataView( pChartDataView_ ), pMenuPopup( nullptr )
    {}
    CustomItemData( pChartDataView_t pChartDataView_, wxMenu* pMenuPopup_ )
    : pChartDataView( pChartDataView_ ), pMenuPopup( pMenuPopup_ )
    {}
    ~CustomItemData() {
      if ( nullptr != pMenuPopup ) {
        // TODO: maybe determine logic to remove the bind
        delete pMenuPopup;
        pMenuPopup = nullptr;
      }
    }
  };

}

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
  static const int pxLeftPanelSize = 150;
  m_pSplitter = new wxSplitterWindow( this );
  m_pSplitter->SetMinimumPaneSize( pxLeftPanelSize );
  m_pSplitter->SetSashGravity( 0.0 );

  // tree
  //wxTreeCtrl* tree;
  m_pTree = new wxTreeCtrl( m_pSplitter );
  m_pTree->ExpandAll();
  //m_eLatestDatumType = CustomItemData::NoDatum;
  //wxTreeItemId idRoot = m_pTree->AddRoot( "Total P/L", -1, -1, new CustomItemData( CustomItemData::PL ) );
  m_pTree->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &PanelFinancialChart::HandleTreeEventItemActivated, this, m_pTree->GetId() );
  //m_pTree->Bind( wxEVT_TREE_ITEM_RIGHT_CLICK, &PanelFinancialChart::HandleTreeEventItemRightClick, this, m_pTree->GetId() ); //wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP     wxEVT_TREE_ITEM_GETTOOLTIP
  m_pTree->Bind( wxEVT_TREE_ITEM_MENU, &PanelFinancialChart::HandleTreeEventItemMenu, this, m_pTree->GetId() ); //wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP     wxEVT_TREE_ITEM_GETTOOLTIP
  m_pTree->Bind( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, &PanelFinancialChart::HandleTreeEventItemGetToolTip, this, m_pTree->GetId() ); //wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP     wxEVT_TREE_ITEM_GETTOOLTIP
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

  m_pWinChartView = new WinChartView( m_pSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );

  m_pSplitter->SplitVertically( m_pTree, m_pWinChartView, 0 );
  sizerMain->Add( m_pSplitter, 1, wxEXPAND|wxALL, 2);
  m_pSplitter->SetSashPosition( pxLeftPanelSize );

  // sizer for right side of splitter
  //wxBoxSizer* sizerRight = new wxBoxSizer( wxVERTICAL );
  //panelSplitterRightPanel->SetSizer( sizerRight );

  //sizerRight->Add( m_pWinChartView, 1, wxEXPAND|wxALL, 2);

  //pSplitter->SetSashPosition()

  Bind( wxEVT_CLOSE_WINDOW, &PanelFinancialChart::OnClose, this );  // start close of windows and controls
}

  wxTreeItemId PanelFinancialChart::SetRoot( const std::string& sName, pChartDataView_t pChartDataView ) {
  if ( m_pTree->GetRootItem().IsOk() ) {
    throw std::runtime_error( "root item already exists" );
  }
  else {
    CustomItemData* cid = new CustomItemData( pChartDataView );
    return m_pTree->AddRoot( sName, -1, -1, cid );
  }
}

  wxTreeItemId PanelFinancialChart::AppendItem( wxTreeItemId parent, const std::string& sName, pChartDataView_t pChartDataView, wxMenu* pMenuPppup ) {
  CustomItemData* cid = new CustomItemData( pChartDataView, pMenuPppup );
  wxTreeItemId id = m_pTree->AppendItem( parent, sName, -1, 01, cid );
  m_pTree->SortChildren( parent );
  return id;
}

void PanelFinancialChart::DeleteItem( wxTreeItemId id ) {
  if ( 0 < m_pTree->GetChildrenCount( id ) ) throw std::runtime_error( "item has children" );
  wxTreeItemData* pData = m_pTree->GetItemData( id );
  if ( nullptr != pData ) {
    CustomItemData* pCustom = dynamic_cast<CustomItemData*>( pData );
    if ( pCustom->pChartDataView ) {
      m_pWinChartView->SetChartDataView( nullptr );
    }
    m_pTree->Delete( id );
  }
}

void PanelFinancialChart::HandleTreeEventItemActivated( wxTreeEvent& event ) {
  wxTreeItemData* pData = m_pTree->GetItemData( event.GetItem() );
  if ( nullptr != pData ) {
    CustomItemData* pCustom = dynamic_cast<CustomItemData*>( pData );
    if ( pCustom->pChartDataView ) {
      m_pWinChartView->SetChartDataView( pCustom->pChartDataView.get() );
    }
    else {
      std::cout << "no chart data" << std::endl;
      m_pWinChartView->SetChartDataView( nullptr );
    }
  }
  else {
    std::cout << "no item data" << std::endl;
    m_pWinChartView->SetChartDataView( nullptr );
  }
  event.Skip();
}

void PanelFinancialChart::HandleTreeEventItemRightClick( wxTreeEvent& event ) {
  std::cout << "HandleTreeEventItemRightClick: " << event.GetId() << std::endl;
}

void PanelFinancialChart::HandleTreeEventItemMenu( wxTreeEvent& event ) {
  std::cout << "HandleTreeEventItemMenu: " << event.GetId() << std::endl;

  wxTreeItemData* pData = m_pTree->GetItemData( event.GetItem() );
  if ( nullptr != pData ) {
    CustomItemData* pCustom = dynamic_cast<CustomItemData*>( pData );
    if ( pCustom->pMenuPopup ) {
      PopupMenu( pCustom->pMenuPopup, event.GetPoint() );
    }
  }
  else {
    std::cout << "no item data" << std::endl;
  }


  // Show popupmenu at position, example to be reworked
  //wxMenu menu( wxT("Test") );
  //menu.Append( wxID_ABOUT, wxT("&About") );
  //menu.Bind( wxEVT_COMMAND_MENU_SELECTED, &PanelFinancialChart::HandlePopUpClick, this );
  //PopupMenu(&menu, event.GetPoint() );
  //menu.Unbind( wxEVT_COMMAND_MENU_SELECTED, &PanelFinancialChart::HandlePopUpClick, this );
}

void PanelFinancialChart::HandlePopUpClick( wxCommandEvent& event ) {
  std::cout << "HandlePopUpClick: " << event.GetId() << std::endl;
}

void PanelFinancialChart::HandleTreeEventItemGetToolTip( wxTreeEvent& event ) {
  wxTreeItemData* pData = m_pTree->GetItemData( ( event.GetItem() ) );
  if ( nullptr != pData ) {
    CustomItemData* pCustom = dynamic_cast<CustomItemData*>( pData );
    if ( pCustom->pChartDataView ) {
      event.SetToolTip( pCustom->pChartDataView->GetName() );
    }
  }
  event.Skip();
}

void PanelFinancialChart::OnClose( wxCloseEvent& event ) {
  m_pTree->Unbind( wxEVT_COMMAND_TREE_SEL_CHANGED, &PanelFinancialChart::HandleTreeEventItemActivated, this, m_pTree->GetId() );
  m_pTree->Unbind( wxEVT_TREE_ITEM_RIGHT_CLICK, &PanelFinancialChart::HandleTreeEventItemRightClick, this, m_pTree->GetId() ); //wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP     wxEVT_TREE_ITEM_GETTOOLTIP
  m_pTree->Unbind( wxEVT_TREE_ITEM_MENU, &PanelFinancialChart::HandleTreeEventItemMenu, this, m_pTree->GetId() ); //wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP     wxEVT_TREE_ITEM_GETTOOLTIP
  m_pTree->Unbind( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, &PanelFinancialChart::HandleTreeEventItemGetToolTip, this, m_pTree->GetId() ); //wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP     wxEVT_TREE_ITEM_GETTOOLTIP
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

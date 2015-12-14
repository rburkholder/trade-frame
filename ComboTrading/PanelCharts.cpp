/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

// started December 6, 2015, 1:26 PM

#include <wx/mstream.h>
#include <wx/bitmap.h>
#include <wx/splitter.h>
#include <wx/sizer.h>
//#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/icon.h>
#include <wx/menu.h>

#include "PanelCharts.h"
#include "TreeItemGroup.h"

// 20151206 need to think about serialization of what is in the tree so it can be 
//   retrieved for next time

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelCharts::PanelCharts( void ): wxPanel() {
  Init();
}

PanelCharts::PanelCharts( 
  wxWindow* parent, wxWindowID id, 
  const wxPoint& pos, 
  const wxSize& size, 
  long style )
{
  
  Init();
  Create(parent, id, pos, size, style);
  
}

PanelCharts::~PanelCharts() {
}

void PanelCharts::Init( void ) {
  
}

bool PanelCharts::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer())     {
      GetSizer()->SetSizeHints(this);
  }
  Centre();
  return true;
}

/*
   m_sizerPM = new wxBoxSizer(wxVERTICAL);
  SetSizer(m_sizerPM);

  //m_scrollPM = new wxScrolledWindow( m_pFPPOE, -1, wxDefaultPosition, wxSize(200, 400), wxVSCROLL );
  m_scrollPM = new wxScrolledWindow( this, -1, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
  m_sizerPM->Add(m_scrollPM, 1, wxGROW|wxALL, 5);
  m_scrollPM->SetScrollbars(1, 1, 0, 0);

  m_sizerScrollPM = new wxBoxSizer(wxVERTICAL);
  m_scrollPM->SetSizer( m_sizerScrollPM );
*/

/*
void TreeItemProjectorArea::ConnectToSelectionEvent( TreeItemSceneElementBase* p ) {
  namespace args = boost::phoenix::arg_names;
  p->m_signalSelectionEventSetSelected.connect( boost::phoenix::bind( &TreeItemProjectorArea::HandleSetSelected, this, args::arg1 ) );
  p->m_signalSelectionEventRemoveSelected.connect( boost::phoenix::bind( &TreeItemProjectorArea::HandleRemoveSelected, this, args::arg1 ) );
}

} */

void PanelCharts::CreateControls() {    

    PanelCharts* itemPanel1 = this;

    wxBoxSizer* sizerMain = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(sizerMain);

  // splitter
  wxSplitterWindow* splitter;
  splitter = new wxSplitterWindow( this );
  splitter->SetMinimumPaneSize(10);
  splitter->SetSashGravity(0.2);

  // tree
  //wxTreeCtrl* tree;
//  m_pTreeSymbols = new wxTreeCtrl( splitter );
  //wxTreeItemId idRoot = m_pTreeSymbols->AddRoot( "/", -1, -1, new CustomItemData( CustomItemData::Root, m_eLatestDatumType ) );
//  wxTreeItemId idRoot = m_pTreeSymbols->AddRoot( "Collections", -1, -1, new CustomItemBase );
//  m_pTreeSymbols->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &PanelCharts::HandleTreeSelChanged, this, m_pTreeSymbols->GetId() );
  //m_pHdf5Root->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &PanelChartHdf5::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId() );
//  m_pTreeSymbols->Bind( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &PanelCharts::HandleTreeItemActivated, this, m_pTreeSymbols->GetId() );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pTreeSymbols->Bind( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, &PanelCharts::HandleTreeItemRightClick, this, m_pTreeSymbols->GetId() );
  //m_pTreeSymbols->Bind( wxEVT_COMMAND_TREE_ITEM_MENU, &PanelCharts::HandleTreeItemMenu, this, m_pTreeSymbols->GetId()  );
  //m_pTreeSymbols->AppendItem( idRoot, "equities" );
  //m_pTreeSymbols->AppendItem( idRoot, "futures" );
  //m_pTreeSymbols->AppendItem( idRoot, "foptions" );
  //m_pTreeSymbols->AppendItem( idRoot, "portfolios" );
  
  m_pTreeOps = new ou::tf::TreeOps( splitter );
  m_pTreeOps->PopulateResources( m_baseResources );
  
  wxTreeItemId id = m_pTreeOps->AddRoot( "Root" );  // can be renamed
  boost::shared_ptr<TreeItemRoot> p( new TreeItemRoot( id, m_baseResources, m_resources ) );
  m_pTreeOps->SetRoot( p );
  //m_pTreeItemRoot.reset( new TreeItemRoot( id, m_resources ) );
  //m_mapDecoder.insert( mapDecoder_t::value_type( id.GetID(), m_pTreeItemRoot ) );

  // panel for right side of splitter
  wxPanel* panelSplitterRightPanel;
  panelSplitterRightPanel = new wxPanel( splitter );

  splitter->SplitVertically( m_pTreeOps, panelSplitterRightPanel, 0 );
  sizerMain->Add( splitter, 1, wxGROW|wxALL, 5 );

  // sizer for right side of splitter
  wxBoxSizer* sizerRight;
  sizerRight = new wxBoxSizer( wxVERTICAL );
  panelSplitterRightPanel->SetSizer( sizerRight );

  // initialize the tree
  //m_pHdf5Root->DeleteChildren( m_pHdf5Root->GetRootItem() );

  //m_bPaintingChart = false;
  //m_bReadyToDrawChart = false;
  // 20151213 should this be a window or a panel?
  m_winChart = new wxWindow( panelSplitterRightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  sizerRight->Add( m_winChart, 1, wxALL|wxEXPAND, 5);
  wxWindowID idChart = m_winChart->GetId();
  m_winChart->Bind( wxEVT_PAINT, &PanelCharts::HandlePaint, this, idChart );
  m_winChart->Bind( wxEVT_SIZE, &PanelCharts::HandleSize, this, idChart );
  
  m_resources.m_pWin = m_winChart;

  Bind( wxEVT_CLOSE_WINDOW, &PanelCharts::OnClose, this );  // start close of windows and controls

}

void PanelCharts::HandleTreeSelChanged( wxTreeEvent& event ) {
  std::cout << "HandleTreeSelChanged" << std::endl;
}

void PanelCharts::HandleTreeItemRightClick( wxTreeEvent& event ) {
  std::cout << "HandleTreeItemRightClick" << std::endl;
  CustomItemBase* p = reinterpret_cast<CustomItemBase*>( event.GetClientObject() );
  assert( 0 != p );
  switch ( p->m_eNodeType ) {
    case CustomItemBase::Root:
      ShowContextMenuRoot();
      break;
    case CustomItemBase::Group:
      break;
    case CustomItemBase::Instrument:
      break;
    case CustomItemBase::Portfolio:
      break;
    case CustomItemBase::Position:
      break;
  }
}

void PanelCharts::ShowContextMenuRoot( void ) {
  wxMenu* pMenu = new wxMenu();
  pMenu->Append( MIGroup, "Append &Group" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &PanelCharts::HandleAddGroup, this, MIGroup );
  pMenu->Append( MIInstrument, "Append &Instrument" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &PanelCharts::HandleAddInstrument, this, MIInstrument );
  pMenu->Append( MIPortfolio, "Append P&ortfolio" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &PanelCharts::HandleAddPortfolio, this, MIPortfolio );
  pMenu->Append( MIPosition, "Append Pos&ition" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &PanelCharts::HandleAddPosition, this, MIPosition );
//  m_pTreeSymbols->PopupMenu( pMenu );
}

void PanelCharts::HandleAddGroup( wxCommandEvent& event ) {
  std::cout << "HandleAddGroup" << std::endl;
//  wxTreeItemId idParent = m_pTreeSymbols->GetItemParent( event. );
//  if ( !idParent.IsOk() ) {
    // on root so just add something
//  }
//  else {
    // try an insert at some point
//  }
//  wxTreeItemId id = m_pTreeSymbols->AppendItem( event.GetId(), "Group", -1, -1, new CustomItemGroup );
}

void PanelCharts::HandleAddInstrument( wxCommandEvent& event ) {
  std::cout << "HandleAddInstrument" << std::endl;
}

void PanelCharts::HandleAddPortfolio( wxCommandEvent& event ) {
  std::cout << "HandleAddPortfolio" << std::endl;
}

void PanelCharts::HandleAddPosition( wxCommandEvent& event ) {
  std::cout << "HandleAddPosition" << std::endl;
}

//void PanelCharts::HandleTreeItemMenu( wxTreeEvent& event ) {
//  std::cout << "HandleTreeItemMenu" << std::endl;
//}

void PanelCharts::HandleTreeItemActivated( wxTreeEvent& event ) {
  std::cout << "HandleTreeItemActivated" << std::endl;
}


void PanelCharts::OnClose( wxCloseEvent& event ) {


  // Exit Steps: #2 -> FrameMain::OnClose
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

void PanelCharts::HandlePaint( wxPaintEvent& event ) {
  if ( 0 != m_pChartDataView ) {
    try {
      //m_bPaintingChart = true;
      wxSize size = m_winChart->GetClientSize();
      m_chartMaster.SetChartDimensions( size.GetWidth(), size.GetHeight() );
      m_chartMaster.SetOnDrawChart( MakeDelegate( this, &PanelCharts::HandleDrawChart ) );
      m_chartMaster.DrawChart( );
    }
    catch (...) {
    }
  }
  //m_bPaintingChart = false;
}

void PanelCharts::HandleSize( wxSizeEvent& event ) { 
  m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
}

// http://www.chartdir.com/forum/download_thread.php?bn=chartdir_support&thread=1144757575#N1144760096
void PanelCharts::HandleDrawChart( const MemBlock& m ) {
  wxMemoryInputStream in( m.data, m.len );
  wxBitmap bmp( wxImage( in, wxBITMAP_TYPE_BMP) );
  wxPaintDC cdc( m_winChart );
  cdc.DrawBitmap(bmp, 0, 0);
}

wxBitmap PanelCharts::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelCharts::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou

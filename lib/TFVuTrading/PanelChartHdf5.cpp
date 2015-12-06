/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include <vector>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <wx/mstream.h>
#include <wx/bitmap.h>
#include <wx/splitter.h>

#include <TFHDF5TimeSeries/HDF5IterateGroups.h>

#include "PanelChartHdf5.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelChartHdf5::PanelChartHdf5(void): m_pChartDataView( 0 ), m_winChart( 0 ), m_pdm( 0 ) {
  Init();
}

PanelChartHdf5::PanelChartHdf5( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
  : m_pChartDataView( 0 ), m_winChart( 0 ), m_pdm( 0 ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelChartHdf5::~PanelChartHdf5(void) {
  m_chartMaster.SetChartDataView( 0 );
  if ( 0 != m_pChartDataView ) delete m_pChartDataView;
  if ( 0 != m_winChart ) delete m_winChart;
  if ( 0 != m_pdm ) delete m_pdm;
}

void PanelChartHdf5::Init() {
}

bool PanelChartHdf5::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer())     {
      GetSizer()->SetSizeHints(this);
  }
  Centre();
  return true;
}

void PanelChartHdf5::CreateControls() {    

    PanelChartHdf5* itemPanel1 = this;

    wxBoxSizer* sizerMain = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(sizerMain);

  // splitter
  wxSplitterWindow* splitter;
  splitter = new wxSplitterWindow( this );
  splitter->SetMinimumPaneSize(10);
  splitter->SetSashGravity(0.2);

  // tree
  //wxTreeCtrl* tree;
  m_pHdf5Root = new wxTreeCtrl( splitter );
  m_eLatestDatumType = CustomItemData::NoDatum;
  wxTreeItemId idRoot = m_pHdf5Root->AddRoot( "/", -1, -1, new CustomItemData( CustomItemData::Root, m_eLatestDatumType ) );
  m_pHdf5Root->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &PanelChartHdf5::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId() );
  //m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId() );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_MENU, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  tree->AppendItem( idRoot, "second" );
//  tree->AppendItem( idRoot, "third" );

  // panel for right side of splitter
  wxPanel* panelSplitterRightPanel;
  panelSplitterRightPanel = new wxPanel( splitter );

  splitter->SplitVertically( m_pHdf5Root, panelSplitterRightPanel, 0 );
  sizerMain->Add( splitter, 1, wxGROW|wxALL, 5 );

  // sizer for right side of splitter
  wxBoxSizer* sizerRight;
  sizerRight = new wxBoxSizer( wxVERTICAL );
  panelSplitterRightPanel->SetSizer( sizerRight );

  // initialize the tree
  //m_pHdf5Root->DeleteChildren( m_pHdf5Root->GetRootItem() );

  //m_bPaintingChart = false;
  //m_bReadyToDrawChart = false;
  m_winChart = new wxWindow( panelSplitterRightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  sizerRight->Add( m_winChart, 1, wxALL|wxEXPAND, 5);
  wxWindowID idChart = m_winChart->GetId();
  m_winChart->Bind( wxEVT_PAINT, &PanelChartHdf5::HandlePaint, this, idChart );
  m_winChart->Bind( wxEVT_SIZE, &PanelChartHdf5::HandleSize, this, idChart );

  m_pdm = new ou::tf::HDF5DataManager( ou::tf::HDF5DataManager::RO );

  m_sCurrentPath = "/";  

  namespace args = boost::phoenix::placeholders;
  ou::tf::hdf5::IterateGroups ig( 
    "/", 
    boost::phoenix::bind( &PanelChartHdf5::HandleLoadTreeHdf5Group, this, args::arg1, args::arg2 ), 
    boost::phoenix::bind( &PanelChartHdf5::HandleLoadTreeHdf5Object, this, args::arg1, args::arg2 ) 
    );


  Bind( wxEVT_CLOSE_WINDOW, &PanelChartHdf5::OnClose, this );  // start close of windows and controls

}

void PanelChartHdf5::OnClose( wxCloseEvent& event ) {

  delete m_pdm;
  m_pdm = 0;

  // Exit Steps: #2 -> FrameMain::OnClose
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

void PanelChartHdf5::HandleLoadTreeHdf5Group( const std::string& s1, const std::string& s2 ) {
  if ( "quotes" == s2 ) m_eLatestDatumType = CustomItemData::Quotes;
  if ( "trades" == s2 ) m_eLatestDatumType = CustomItemData::Trades;
  if ( "bar" == s2 ) m_eLatestDatumType = CustomItemData::Bars;
  //if ( "AtmIV" == s2 ) m_eLatestDatumType = CustomItemData::AtmIV;
  if ( "atmiv" == s2 ) m_eLatestDatumType = CustomItemData::AtmIV;
  if ( "greeks" == s2 ) m_eLatestDatumType = CustomItemData::Greeks;
  namespace args = boost::phoenix::placeholders;
  m_sCurrentPath = s1;
  m_curTreeItem = m_pHdf5Root->GetRootItem();  // should be '/'
  m_pdm->IteratePathParts( s1, boost::phoenix::bind( &PanelChartHdf5::HandleBuildTreePathParts, this, args::arg1 ) );
}

void PanelChartHdf5::HandleLoadTreeHdf5Object( const std::string& s1, const std::string& s2 ) {
  // assume group has us in the correct place, just add in the object now
  m_pHdf5Root->AppendItem( m_curTreeItem, s2, -1, -1, new CustomItemData( CustomItemData::Object, m_eLatestDatumType ) );
}

void PanelChartHdf5::HandleBuildTreePathParts( const std::string& sPathPart ) {
  wxTreeItemIdValue tiv;
  wxTreeItemId ti = m_pHdf5Root->GetFirstChild( m_curTreeItem, tiv );
  bool bItemFound( false );
  while ( ti.IsOk() ) {
    if ( sPathPart == m_pHdf5Root->GetItemText( ti ) ) {
      m_curTreeItem = ti;
      bItemFound = true;
      break;
    }
    else {
      ti = m_pHdf5Root->GetNextChild( m_curTreeItem, tiv );
    }
  }
  if ( !bItemFound ) {
    m_curTreeItem = m_pHdf5Root->AppendItem( m_curTreeItem, sPathPart, -1, -1, new CustomItemData( CustomItemData::Group, CustomItemData::NoDatum ) );
  }

}

void PanelChartHdf5::HandleTreeEventItemActivated( wxTreeEvent& event ) {

  wxTreeItemId id = event.GetItem();

  wxTreeItemId id2 = id;
  std::string sPath = std::string( m_pHdf5Root->GetItemText( id2 ).ToStdString() ); // start here and prefix the path
  while ( true ) {
    id2 = m_pHdf5Root->GetItemParent( id2 );
    if ( !id2.IsOk() ) break;
    sPath = "/" + sPath;
    std::string sTmpElement( m_pHdf5Root->GetItemText( id2 ) );
    if ( "/" != sTmpElement ) {
      sPath = sTmpElement + sPath;
    }
  }

  CustomItemData* pdata = dynamic_cast<CustomItemData*>( m_pHdf5Root->GetItemData( id ) );

  switch ( pdata->m_eNodeType ) {
  case CustomItemData::Root:
    break;
  case CustomItemData::Group:
    sPath += "/";
    break;
  case CustomItemData::Object:
    // load and view time series here
    LoadDataAndGenerateChart( pdata->m_eDatumType, sPath );
    break;
  }

  std::cout << sPath << std::endl;
  
}

//void PanelChartHdf5::HandleMenuActionStartChart( void ) {
//  m_bReadyToDrawChart = true;
//  m_pChart = new ChartTest( m_pData1Provider );

//  m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
//}

void PanelChartHdf5::HandlePaint( wxPaintEvent& event ) {
  if ( 0 != m_pChartDataView ) {
    try {
      //m_bPaintingChart = true;
      wxSize size = m_winChart->GetClientSize();
      m_chartMaster.SetChartDimensions( size.GetWidth(), size.GetHeight() );
      m_chartMaster.SetOnDrawChart( MakeDelegate( this, &PanelChartHdf5::HandleDrawChart ) );
      m_chartMaster.DrawChart( );
    }
    catch (...) {
    }
  }
  //m_bPaintingChart = false;
}

void PanelChartHdf5::HandleSize( wxSizeEvent& event ) { 
  m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
}

// http://www.chartdir.com/forum/download_thread.php?bn=chartdir_support&thread=1144757575#N1144760096
void PanelChartHdf5::HandleDrawChart( const MemBlock& m ) {
  wxMemoryInputStream in( m.data, m.len );
  wxBitmap bmp( wxImage( in, wxBITMAP_TYPE_BMP) );
  wxPaintDC cdc( m_winChart );
  cdc.DrawBitmap(bmp, 0, 0);
}

void PanelChartHdf5::LoadDataAndGenerateChart( CustomItemData::enumDatumType edt, const std::string& sPath ) {

  if ( 0 != m_pChartDataView ) {
    m_chartMaster.SetChartDataView( 0 );
    delete m_pChartDataView;
    m_pChartDataView = 0;
  }
  m_pChartDataView = new ou::ChartDataView;
  m_chartMaster.SetChartDataView( m_pChartDataView );

  switch ( edt ) {
  case CustomItemData::Bars:
    m_ModelChartHdf5.ChartTimeSeries<Bars>( m_pdm, m_pChartDataView, "Bars", sPath );
    m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
    break;
  case CustomItemData::Quotes:
    m_ModelChartHdf5.ChartTimeSeries<Quotes>( m_pdm, m_pChartDataView, "Quotes", sPath );
    m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
    break;
  case CustomItemData::Trades:
    m_ModelChartHdf5.ChartTimeSeries<Trades>( m_pdm, m_pChartDataView, "Trades", sPath );
    m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
    break;
  case CustomItemData::AtmIV:
    m_ModelChartHdf5.ChartTimeSeries<PriceIVs>( m_pdm, m_pChartDataView, "Price IV", sPath );
    m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
    break;
  case CustomItemData::Greeks:
    m_ModelChartHdf5.ChartTimeSeries<Greeks>( m_pdm, m_pChartDataView, "Greeks", sPath );
    m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
    break;
  case CustomItemData::NoDatum:
    std::cout << "Can't do this chart type" << std::endl;
    break;
  }
}

wxBitmap PanelChartHdf5::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelChartHdf5::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou

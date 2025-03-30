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

#include <wx/sizer.h>

#include <TFHDF5TimeSeries/HDF5IterateGroups.h>

#include "PanelChartHdf5.hpp"

// TODO: make use of smart pointers

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelChartHdf5::PanelChartHdf5()
: wxPanel(),
  m_pChartDataView( nullptr ), m_pWinChartView( nullptr ), m_pdm( nullptr )
{
  Init();
}

PanelChartHdf5::PanelChartHdf5( const std::string& sFileName )
: wxPanel(),
  m_pChartDataView( nullptr ), m_pWinChartView( nullptr ), m_pdm( nullptr )
{
  m_pdm = new ou::tf::HDF5DataManager( ou::tf::HDF5DataManager::RO, sFileName );
  Init();
}

PanelChartHdf5::PanelChartHdf5( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxPanel(),
  m_pChartDataView( nullptr ), m_pWinChartView( nullptr ), m_pdm( nullptr )
{
  Init();
  Create( parent, id, pos, size, style );
}

PanelChartHdf5::~PanelChartHdf5() {
  m_pWinChartView->SetChartDataView( nullptr );
  if ( nullptr != m_pChartDataView ) delete m_pChartDataView;
  if ( nullptr != m_pdm ) delete m_pdm;
}

void PanelChartHdf5::Init() {
}

bool PanelChartHdf5::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer()) {
      GetSizer()->SetSizeHints(this);
  }
  return true;
}

void PanelChartHdf5::CreateControls() {

  PanelChartHdf5* itemPanel1 = this;

  wxBoxSizer* sizerMain = new wxBoxSizer(wxVERTICAL);
  itemPanel1->SetSizer( sizerMain );

  // splitter
  m_pSplitter = new wxSplitterWindow( this );
  m_pSplitter->SetMinimumPaneSize( 20 );
  m_pSplitter->SetSashGravity( 0.2 );

  m_pSplitter->Bind(
    wxEVT_SPLITTER_SASH_POS_CHANGING,  // during sash dragging
    [this]( wxSplitterEvent& event ){
      const int sash = m_pSplitter->GetSashSize();
      const int min = m_pSplitter->GetMinimumPaneSize();
      const int pos = event.GetSashPosition();
      const int w3 = GetSize().GetWidth();
      const int w4 = m_pSplitter->GetSize().GetWidth();
      const int width = w4 - sash - min;
      const double gravity = (double)pos / (double)width;

      //std::cout
      //  << "wxEVT_SPLITTER_SASH_POS_CHANGING"
      //  << ',' << gravity
      //  << ',' << pos
      //  << ',' << w3 << ',' << w4
      //  << std::endl;
      m_pSplitter->SetSashGravity( gravity );
      event.Skip();
    } );

  //wxTreeCtrl* tree;
  m_pHdf5Root = new wxTreeCtrl( m_pSplitter );
  m_eLatestDatumType = CustomItemData::NoDatum;
  wxTreeItemId idRoot = m_pHdf5Root->AddRoot( "/", -1, -1, new CustomItemData( CustomItemData::Root, m_eLatestDatumType ) );
  m_pHdf5Root->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &PanelChartHdf5::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId() );

  // panel for right side of splitter
  wxPanel* panelSplitterRightPanel;
  panelSplitterRightPanel = new wxPanel( m_pSplitter );

  m_pSplitter->SplitVertically( m_pHdf5Root, panelSplitterRightPanel, 0 );
  sizerMain->Add( m_pSplitter, 1, wxGROW|wxALL, 5 );

  // sizer for right side of splitter
  wxBoxSizer* sizerRight;
  sizerRight = new wxBoxSizer( wxVERTICAL );
  panelSplitterRightPanel->SetSizer( sizerRight );

  m_pWinChartView = new WinChartView( panelSplitterRightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  sizerRight->Add( m_pWinChartView, 1, wxALL|wxEXPAND, 5);

  if ( nullptr == m_pdm ) {
    m_pdm = new ou::tf::HDF5DataManager( ou::tf::HDF5DataManager::RO );
  }

  m_sCurrentPath = "/";

  ou::tf::hdf5::IterateGroups ig(
    *m_pdm, std::string( "/" ),
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Group( group, name ); },
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Object( group, name ); }
    );

  Bind( wxEVT_DESTROY, &PanelChartHdf5::OnDestroy, this );  // start close of windows and controls

}

void PanelChartHdf5::OnDestroy( wxWindowDestroyEvent& event ) {

  delete m_pdm;
  m_pdm = nullptr;

  // Exit Steps: #2 -> FrameMain::OnClose
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  event.Skip();  // auto followed by Destroy();
}

void PanelChartHdf5::HandleLoadTreeHdf5Group( const std::string& s1, const std::string& s2 ) {
  m_eLatestDatumType = CustomItemData::NoDatum;
  if ( "quotes" == s2 ) m_eLatestDatumType = CustomItemData::Quotes;
  if ( "trades" == s2 ) m_eLatestDatumType = CustomItemData::Trades;
  if ( "bar" == s2 ) m_eLatestDatumType = CustomItemData::Bars;
  if ( "atmiv" == s2 ) m_eLatestDatumType = CustomItemData::AtmIV;
  if ( "priceiv" == s2 ) m_eLatestDatumType = CustomItemData::PriceIVs;
  if ( "greeks" == s2 ) m_eLatestDatumType = CustomItemData::Greeks;
  if ( "depths" == s2 ) m_eLatestDatumType = CustomItemData::DepthsByMM; // deprecated style 2022/04/30
  if ( "depths_mm" == s2 ) m_eLatestDatumType = CustomItemData::DepthsByMM;
  if ( "depths_o" == s2 ) m_eLatestDatumType = CustomItemData::DepthsByOrder;
  m_sCurrentPath = s1;
  m_curTreeItem = m_pHdf5Root->GetRootItem();  // should be '/'
  m_pdm->IteratePathParts( s1, [this]( const std::string& path){ HandleBuildTreePathParts( path ); } );
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

  size_t cntSeriesElements {};

  switch ( pdata->m_eNodeType ) {
  case CustomItemData::Root:
    break;
  case CustomItemData::Group:
    sPath += "/";
    break;
  case CustomItemData::Object:
    // load and view time series here
    cntSeriesElements = LoadDataAndGenerateChart( pdata->m_eDatumType, sPath );
    break;
  }

  std::cout << sPath << " with " << cntSeriesElements << " elements" << std::endl;

}

size_t PanelChartHdf5::LoadDataAndGenerateChart( CustomItemData::EDatumType edt, const std::string& sPath ) {

  if ( nullptr != m_pChartDataView ) {
    m_pWinChartView->SetChartDataView( nullptr );
    delete m_pChartDataView;
    m_pChartDataView = nullptr;
  }
  m_pChartDataView = new ou::ChartDataView;

  size_t cntSeriesElements {};

  if ( CustomItemData::NoDatum == edt ) {
    std::cout << "Can't do this chart type" << std::endl;
  }
  else {
    switch ( edt ) {
    case CustomItemData::Bars:
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<Bars>( m_pdm, m_pChartDataView, "Bars", sPath );
      break;
    case CustomItemData::Quotes:
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<Quotes>( m_pdm, m_pChartDataView, "Quotes", sPath );
      break;
    case CustomItemData::Trades:
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<Trades>( m_pdm, m_pChartDataView, "Trades", sPath );
      break;
    case CustomItemData::AtmIV:
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<PriceIVExpirys>( m_pdm, m_pChartDataView, "Price IV Expiry", sPath );
      break;
    case CustomItemData::PriceIVs:
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<PriceIVs>( m_pdm, m_pChartDataView, "Price IV", sPath );
      break;
    case CustomItemData::Greeks:
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<Greeks>( m_pdm, m_pChartDataView, "Greeks", sPath );
      break;
    case CustomItemData::DepthsByMM:
      std::cout << "DepthsByMM will not show data" << std::endl;
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<DepthsByMM>( m_pdm, m_pChartDataView, "DepthsByMM", sPath );
      break;
    case CustomItemData::DepthsByOrder:
      std::cout << "DepthsByOrder will not show data" << std::endl;
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<DepthsByOrder>( m_pdm, m_pChartDataView, "DepthsByOrder", sPath );
      break;
    default:
      throw std::runtime_error("unknown CustomItemData");
    }
  }

  m_pWinChartView->SetChartDataView( m_pChartDataView, false );

  return cntSeriesElements;
}

wxBitmap PanelChartHdf5::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

//wxIcon PanelChartHdf5::GetIconResource( const wxString& name ) {
//    wxUnusedVar(name);
//    return wxNullIcon;
//}

} // namespace tf
} // namespace ou

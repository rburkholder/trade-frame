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

#include <TFHDF5TimeSeries/HDF5IterateGroups.h>

#include "PanelChartHdf5.hpp"

// TODO: make use of smart pointers

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelChartHdf5::PanelChartHdf5()
: PanelFinancialChart(),
  m_pChartDataView( nullptr ), m_pdm( nullptr )
{
}

PanelChartHdf5::PanelChartHdf5( const std::string& sFileName )
: PanelFinancialChart(),
  m_pChartDataView( nullptr ), m_pdm( nullptr )
{
  m_pdm = std::make_unique<ou::tf::HDF5DataManager>( ou::tf::HDF5DataManager::RO, sFileName );
}

PanelChartHdf5::PanelChartHdf5( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: PanelFinancialChart(),
  m_pChartDataView( nullptr ), m_pdm( nullptr )
{
  Create( parent, id, pos, size, style );
}

PanelChartHdf5::~PanelChartHdf5() {
  m_pChartDataView.reset();
  m_pdm.reset();
}

bool PanelChartHdf5::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  PanelFinancialChart::Create( parent, id, pos, size, style );

  CreateControls();
  return true;
}

void PanelChartHdf5::CreateControls() {

  PanelChartHdf5* itemPanel1 = this;

  // wxTreeCtrl* tree;
  m_pTree->DeleteAllItems(); // start the tree fresh with our own flavbour
  m_eLatestDatumType = CustomItemData_Hdf5::NoDatum;
  wxTreeItemId idRoot = m_pTree->AddRoot( "/", -1, -1, new CustomItemData_Hdf5( CustomItemData_Hdf5::Root, m_eLatestDatumType ) );
  m_pTree->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &PanelChartHdf5::HandleTreeEventItemActivated, this, m_pTree->GetId() );

  if ( nullptr == m_pdm ) {
    m_pdm = std::make_unique<ou::tf::HDF5DataManager>( ou::tf::HDF5DataManager::RO );
  }

  IterateObjects();

}

void PanelChartHdf5::SetFileName( const std::string& sPathName ) {
  m_pdm = std::make_unique<ou::tf::HDF5DataManager>( ou::tf::HDF5DataManager::RO, sPathName );
  wxTreeItemId itemRoot = m_pTree->GetRootItem();
  m_pTree->DeleteChildren( itemRoot );
  IterateObjects();
}

void PanelChartHdf5::IterateObjects() {
  m_sCurrentPath = "/";

  ou::tf::hdf5::IterateGroups ig(
    *m_pdm, std::string( "/" ),
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Group( group, name ); },
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Object( group, name ); }
    );
}

void PanelChartHdf5::HandleLoadTreeHdf5Group( const std::string& s1, const std::string& s2 ) {
  m_eLatestDatumType = CustomItemData_Hdf5::NoDatum;
  if ( "quotes" == s2 ) m_eLatestDatumType = CustomItemData_Hdf5::Quotes;
  if ( "trades" == s2 ) m_eLatestDatumType = CustomItemData_Hdf5::Trades;
  if ( "bar" == s2 ) m_eLatestDatumType = CustomItemData_Hdf5::Bars;
  if ( "atmiv" == s2 ) m_eLatestDatumType = CustomItemData_Hdf5::AtmIV;
  if ( "priceiv" == s2 ) m_eLatestDatumType = CustomItemData_Hdf5::PriceIVs;
  if ( "greeks" == s2 ) m_eLatestDatumType = CustomItemData_Hdf5::Greeks;
  if ( "depths" == s2 ) m_eLatestDatumType = CustomItemData_Hdf5::DepthsByMM; // deprecated style 2022/04/30
  if ( "depths_mm" == s2 ) m_eLatestDatumType = CustomItemData_Hdf5::DepthsByMM;
  if ( "depths_o" == s2 ) m_eLatestDatumType = CustomItemData_Hdf5::DepthsByOrder;
  m_sCurrentPath = s1;
  m_curTreeItem = m_pTree->GetRootItem();  // should be '/'
  m_pdm->IteratePathParts( s1, [this]( const std::string& path){ HandleBuildTreePathParts( path ); } );
}

void PanelChartHdf5::HandleLoadTreeHdf5Object( const std::string& s1, const std::string& s2 ) {
  // assume group has us in the correct place, just add in the object now
  m_pTree->AppendItem( m_curTreeItem, s2, -1, -1, new CustomItemData_Hdf5( CustomItemData_Hdf5::Object, m_eLatestDatumType ) );
}

void PanelChartHdf5::HandleBuildTreePathParts( const std::string& sPathPart ) {
  wxTreeItemIdValue tiv;
  wxTreeItemId ti = m_pTree->GetFirstChild( m_curTreeItem, tiv );
  bool bItemFound( false );
  while ( ti.IsOk() ) {
    if ( sPathPart == m_pTree->GetItemText( ti ) ) {
      m_curTreeItem = ti;
      bItemFound = true;
      break;
    }
    else {
      ti = m_pTree->GetNextChild( m_curTreeItem, tiv );
    }
  }
  if ( !bItemFound ) {
    m_curTreeItem = m_pTree->AppendItem( m_curTreeItem, sPathPart, -1, -1, new CustomItemData_Hdf5( CustomItemData_Hdf5::Group, CustomItemData_Hdf5::NoDatum ) );
  }

}

void PanelChartHdf5::HandleTreeEventItemActivated( wxTreeEvent& event ) {

  wxTreeItemId id = event.GetItem();

  wxTreeItemId id2 = id;
  std::string sPath = std::string( m_pTree->GetItemText( id2 ).ToStdString() ); // start here and prefix the path
  while ( true ) {
    id2 = m_pTree->GetItemParent( id2 );
    if ( !id2.IsOk() ) break;
    sPath = "/" + sPath;
    std::string sTmpElement( m_pTree->GetItemText( id2 ) );
    if ( "/" != sTmpElement ) {
      sPath = sTmpElement + sPath;
    }
  }

  CustomItemData_Hdf5* pdata = dynamic_cast<CustomItemData_Hdf5*>( m_pTree->GetItemData( id ) );

  size_t cntSeriesElements {};

  switch ( pdata->m_eNodeType ) {
  case CustomItemData_Hdf5::Root:
    break;
  case CustomItemData_Hdf5::Group:
    sPath += "/";
    break;
  case CustomItemData_Hdf5::Object:
    // load and view time series here
    cntSeriesElements = LoadDataAndGenerateChart( pdata->m_eDatumType, sPath );
    break;
  }

  std::cout << sPath << " with " << cntSeriesElements << " elements" << std::endl;

}

size_t PanelChartHdf5::LoadDataAndGenerateChart( CustomItemData_Hdf5::EDatumType edt, const std::string& sPath ) {

  if ( nullptr != m_pChartDataView ) {
    SetChartDataView( nullptr );
    m_pChartDataView.reset();
  }
  m_pChartDataView = std::make_shared<ou::ChartDataView>();

  size_t cntSeriesElements {};

  if ( CustomItemData_Hdf5::NoDatum == edt ) {
    std::cout << "Can't do this chart type" << std::endl;
  }
  else {
    switch ( edt ) {
    case CustomItemData_Hdf5::Bars:
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<Bars>( m_pdm.get(), m_pChartDataView.get(), "Bars", sPath );
      break;
    case CustomItemData_Hdf5::Quotes:
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<Quotes>( m_pdm.get(), m_pChartDataView.get(), "Quotes", sPath );
      break;
    case CustomItemData_Hdf5::Trades:
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<Trades>( m_pdm.get(), m_pChartDataView.get(), "Trades", sPath );
      break;
    case CustomItemData_Hdf5::AtmIV:
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<PriceIVExpirys>( m_pdm.get(), m_pChartDataView.get(), "Price IV Expiry", sPath );
      break;
    case CustomItemData_Hdf5::PriceIVs:
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<PriceIVs>( m_pdm.get(), m_pChartDataView.get(), "Price IV", sPath );
      break;
    case CustomItemData_Hdf5::Greeks:
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<Greeks>( m_pdm.get(), m_pChartDataView.get(), "Greeks", sPath );
      break;
    case CustomItemData_Hdf5::DepthsByMM:
      std::cout << "DepthsByMM will not show data" << std::endl;
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<DepthsByMM>( m_pdm.get(), m_pChartDataView.get(), "DepthsByMM", sPath );
      break;
    case CustomItemData_Hdf5::DepthsByOrder:
      std::cout << "DepthsByOrder will not show data" << std::endl;
      cntSeriesElements = m_ModelChartHdf5.ChartTimeSeries<DepthsByOrder>( m_pdm.get(), m_pChartDataView.get(), "DepthsByOrder", sPath );
      break;
    default:
      throw std::runtime_error("unknown CustomItemData_Hdf5");
    }
  }

  SetChartDataView( m_pChartDataView, false );

  return cntSeriesElements;
}

} // namespace tf
} // namespace ou

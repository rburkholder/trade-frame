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

//#include <boost/log/trivial.hpp>

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

  m_eLatestDatumType = CustomItemData_Hdf5::NoDatum;

  // wxTreeCtrl* tree;
  m_ptiRoot = SetRoot(
    "/", nullptr,
    [this]( TreeItem* pti ){
      return new CustomItemData_Hdf5( pti, CustomItemData_Hdf5::Root, CustomItemData_Hdf5::NoDatum );
    } );

  if ( nullptr == m_pdm ) {
    m_pdm = std::make_unique<ou::tf::HDF5DataManager>( ou::tf::HDF5DataManager::RO );
  }

  IterateObjects();

}

void PanelChartHdf5::SetFileName( const std::string& sPathName ) {
  m_pdm = std::make_unique<ou::tf::HDF5DataManager>( ou::tf::HDF5DataManager::RO, sPathName );
  DeleteTree();
  IterateObjects();
}

void PanelChartHdf5::IterateObjects() {
  ou::tf::hdf5::IterateGroups ig(
    *m_pdm, std::string( "/" ),
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Group(  group, name ); }, // path
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Object( group, name ); }  // timeseries
    );
}

void PanelChartHdf5::HandleLoadTreeHdf5Group( const std::string& sGroup, const std::string& sName ) {
  //BOOST_LOG_TRIVIAL(info) << "1 Group  " << sGroup << ' ' << sName;
  m_eLatestDatumType = CustomItemData_Hdf5::NoDatum;
  if ( "quotes"    == sName ) m_eLatestDatumType = CustomItemData_Hdf5::Quotes;
  if ( "trades"    == sName ) m_eLatestDatumType = CustomItemData_Hdf5::Trades;
  if ( "bar"       == sName ) m_eLatestDatumType = CustomItemData_Hdf5::Bars;
  if ( "atmiv"     == sName ) m_eLatestDatumType = CustomItemData_Hdf5::AtmIV;
  if ( "priceiv"   == sName ) m_eLatestDatumType = CustomItemData_Hdf5::PriceIVs;
  if ( "greeks"    == sName ) m_eLatestDatumType = CustomItemData_Hdf5::Greeks;
  if ( "depths"    == sName ) m_eLatestDatumType = CustomItemData_Hdf5::DepthsByMM; // deprecated style 2022/04/30
  if ( "depths_mm" == sName ) m_eLatestDatumType = CustomItemData_Hdf5::DepthsByMM;
  if ( "depths_o"  == sName ) m_eLatestDatumType = CustomItemData_Hdf5::DepthsByOrder;
  m_ptiCurrent = m_ptiRoot;  // should be '/'
  m_pdm->IteratePathParts( sGroup, [this]( const std::string& path){ HandleBuildTreePathParts( path ); } );
}

void PanelChartHdf5::HandleLoadTreeHdf5Object( const std::string& sGroup, const std::string& sName ) {
  //BOOST_LOG_TRIVIAL(info) << "2 Object " << sGroup << ' ' << sName;
  // assume group has us in the correct place, just add in the object now
  m_ptiCurrent->AppendChild(
    sName,
    std::bind( &PanelChartHdf5::HandleTreeEventItemActivated, this , std::placeholders::_1 ),
    []( TreeItem* ) {},
    [this]( TreeItem* pti )->CustomItemData_Base* {
      return new CustomItemData_Hdf5( pti, CustomItemData_Hdf5::Object, m_eLatestDatumType );
    }
  );
}

void PanelChartHdf5::HandleBuildTreePathParts( const std::string& sPathPart ) {
  //BOOST_LOG_TRIVIAL(info) << "3 Parts at " << m_ptiCurrent->GetText() << '-' << sPathPart;
  bool bItemFound( false );
  m_ptiCurrent->IterateChildren(
    [this, &bItemFound, &sPathPart]( TreeItem* pti )->bool /* bContinue */ {
      //BOOST_LOG_TRIVIAL(info) << "3 Parts iter " << pti->GetText() << ',' << sPathPart;
      if ( sPathPart == pti->GetText() ) {
        m_ptiCurrent = pti;
        bItemFound = true;
        //BOOST_LOG_TRIVIAL(info) << "3 Parts found " << sPathPart;
        return false;
      }
      return true;
    } );

  if ( !bItemFound ) {
    //BOOST_LOG_TRIVIAL(info) << "3 Parts add " << m_ptiCurrent->GetText() << '+' << sPathPart;
    m_ptiCurrent = m_ptiCurrent->AppendChild(
      sPathPart,
      std::bind( &PanelChartHdf5::HandleTreeEventItemActivated, this , std::placeholders::_1 ),
      []( TreeItem* ) {},
      []( TreeItem* pti )->CustomItemData_Base* {
        return new CustomItemData_Hdf5( pti, CustomItemData_Hdf5::Group, CustomItemData_Hdf5::NoDatum );
      }
    );
  }
}

void PanelChartHdf5::HandleTreeEventItemActivated( TreeItem* pti ) {
  assert( pti );
  std::string sPath( pti->GetText() ); // build path towards root
  TreeItem* ptiCurrent( pti );
  while ( true ) {
    ptiCurrent = ptiCurrent->GetParent();
    if ( nullptr == ptiCurrent ) break;
    sPath = "/" + sPath;
    std::string sTmpElement( ptiCurrent->GetText() );
    if ( "/" != sTmpElement ) {
      sPath = sTmpElement + sPath;
    }
  }

  CustomItemData_Hdf5* pData = dynamic_cast<CustomItemData_Hdf5*>( pti->CustomItemData() );
  assert( pData );

  size_t cntSeriesElements {};

  switch ( pData->m_eNodeType ) {
    case CustomItemData_Hdf5::Root:
      break;
    case CustomItemData_Hdf5::Group:
      sPath += "/";
      break;
    case CustomItemData_Hdf5::Object:
      // load and view time series here
      cntSeriesElements = LoadDataAndGenerateChart( pData->m_eDatumType, sPath );
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

  SetChartDataView( m_pChartDataView );

  return cntSeriesElements;
}

} // namespace tf
} // namespace ou

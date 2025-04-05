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

#pragma once

// Started 2013/10/06

// used by Hdf5Chart

#include <memory>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/treectrl.h>

#include "ModelChartHdf5.h"
#include "TreeItem_ItemData.hpp"
#include "PanelFinancialChart.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANEL_CHARTHDF5_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_CHARTHDF5_TITLE _("Panel Chart HDF5 Data")
#define SYMBOL_PANEL_CHARTHDF5_IDNAME ID_PANEL_CHARTHDF5
#define SYMBOL_PANEL_CHARTHDF5_SIZE wxSize(400, 300)
#define SYMBOL_PANEL_CHARTHDF5_POSITION wxDefaultPosition

class PanelChartHdf5: public PanelFinancialChart {
  friend class boost::serialization::access;
public:

  PanelChartHdf5();
  PanelChartHdf5(
    wxWindow* parent, wxWindowID id = SYMBOL_PANEL_CHARTHDF5_IDNAME,
    const wxPoint& pos = SYMBOL_PANEL_CHARTHDF5_POSITION,
    const wxSize& size = SYMBOL_PANEL_CHARTHDF5_SIZE,
    long style = SYMBOL_PANEL_CHARTHDF5_STYLE );
  virtual ~PanelChartHdf5();

  bool Create( wxWindow* parent,
    wxWindowID id = SYMBOL_PANEL_CHARTHDF5_IDNAME,
    const wxPoint& pos = SYMBOL_PANEL_CHARTHDF5_POSITION,
    const wxSize& size = SYMBOL_PANEL_CHARTHDF5_SIZE,
    long style = SYMBOL_PANEL_CHARTHDF5_STYLE );

  PanelChartHdf5( const std::string& sFileName );
  void SetFileName( const std::string& sFileName );

protected:

  void CreateControls();

private:

  enum { ID_Null=wxID_HIGHEST, ID_PANEL_CHARTHDF5 };

  class CustomItemData_Hdf5: public CustomItemData_Base { // wxTreeCtrl node/leaf info
  public:
    enum ENodeType { Root, Group, Object } m_eNodeType;
    enum EDatumType { Quotes, Trades, Bars, Greeks, AtmIV, DepthsByMM, DepthsByOrder, PriceIVs, NoDatum } m_eDatumType;
    CustomItemData_Hdf5( TreeItem* pTreeItem, ENodeType eNodeType, EDatumType eDatumType )
    : CustomItemData_Base( pTreeItem )
    , m_eNodeType( eNodeType ), m_eDatumType( eDatumType ) {};
  };

  using pHDF5DataManager_t = std::unique_ptr<ou::tf::HDF5DataManager>;
  pHDF5DataManager_t m_pdm;

  TreeItem* m_ptiRoot;
  TreeItem* m_ptiCurrent;

  ModelChartHdf5 m_ModelChartHdf5;

  ou::ChartDataView::pChartDataView_t m_pChartDataView;

  CustomItemData_Hdf5::EDatumType m_eLatestDatumType;

  size_t LoadDataAndGenerateChart( CustomItemData_Hdf5::EDatumType, const std::string& sPath );

  void IterateObjects();

  void HandleLoadTreeHdf5Group( const std::string& s1, const std::string& s2 );
  void HandleLoadTreeHdf5Object( const std::string& s1, const std::string& s2 );

  void HandleBuildTreePathParts( const std::string& sPath );

  void HandleTreeEventItemActivated( TreeItem* );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    //PanelFinancialChart::save( ar, version );
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    //PanelFinancialChart::load( ar, version );
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::PanelChartHdf5, 1)

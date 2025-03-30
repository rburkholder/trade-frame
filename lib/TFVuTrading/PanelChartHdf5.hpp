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

// TODO: refactor to use PanelFinancialChart when complete?

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/treectrl.h>
#include <wx/splitter.h>

#include "WinChartView.h"
#include "ModelChartHdf5.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANEL_CHARTHDF5_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_CHARTHDF5_TITLE _("Panel Chart HDF5 Data")
#define SYMBOL_PANEL_CHARTHDF5_IDNAME ID_PANEL_CHARTHDF5
#define SYMBOL_PANEL_CHARTHDF5_SIZE wxSize(400, 300)
#define SYMBOL_PANEL_CHARTHDF5_POSITION wxDefaultPosition

class PanelChartHdf5: public wxPanel {
  friend class boost::serialization::access;
public:

  PanelChartHdf5();
  PanelChartHdf5( const std::string& sFileName );
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

  static bool ShowToolTips() { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  //wxIcon GetIconResource( const wxString& name );

protected:

  void Init();
  void CreateControls();

private:

  enum { ID_Null=wxID_HIGHEST, ID_PANEL_CHARTHDF5 };

  // TODO: convert to TreeItem
  class CustomItemData: public wxTreeItemData { // wxTreeCtrl node/leaf info
  public:
    enum ENodeType { Root, Group, Object } m_eNodeType;
    enum EDatumType { Quotes, Trades, Bars, Greeks, AtmIV, DepthsByMM, DepthsByOrder, PriceIVs, NoDatum } m_eDatumType;
    CustomItemData( ENodeType eNodeType, EDatumType eDatumType )
      : m_eNodeType( eNodeType ), m_eDatumType( eDatumType ) {};
  };

  ou::tf::HDF5DataManager* m_pdm;

  ou::ChartDataView* m_pChartDataView;
  WinChartView* m_pWinChartView;

  ModelChartHdf5 m_ModelChartHdf5;

  std::string m_sCurrentPath;  // used while traversing and building tree
  wxTreeItemId m_curTreeItem; // used while traversing and building tree
  CustomItemData::EDatumType m_eLatestDatumType;  // need this until all timeseries have a signature attribute associated

  wxTreeCtrl* m_pHdf5Root;  // http://docs.wxwidgets.org/trunk/classwx_tree_ctrl.html
  wxSplitterWindow* m_pSplitter;

  void OnDestroy( wxWindowDestroyEvent& event );

  size_t LoadDataAndGenerateChart( CustomItemData::EDatumType, const std::string& sPath );

  void HandleLoadTreeHdf5Group( const std::string& s1, const std::string& s2 );
  void HandleLoadTreeHdf5Object( const std::string& s1, const std::string& s2 );

  void HandleBuildTreePathParts( const std::string& sPath );

  void HandleTreeEventItemActivated( wxTreeEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    {
      double d( m_pSplitter->GetSashGravity() );
      ar & d;
      //std::cout << "PanelChartHdf5 gravity saved: " << d << std::endl;
    }
    {
      //const int x( m_pSplitter->GetSashPosition() - m_pSplitter->GetMinimumPaneSize() );
      //ar & x;
      //std::cout << "PanelChartHdf5 splitter saved: " << x << std::endl;
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {

    double d;
    ar & d;
    //std::cout << "PanelChartHdf5 gravity loaded: " << d << std::endl;
    m_pSplitter->SetSashGravity( d );
    // not perfect, sash is always GetMinimumPaneSize() too far right

    //int x;
    //ar & x;
    //std::cout << "PanelChartHdf5 splitter loaded: " << x << std::endl;
    //m_pSplitter->SetSashPosition( x );
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::PanelChartHdf5, 1)

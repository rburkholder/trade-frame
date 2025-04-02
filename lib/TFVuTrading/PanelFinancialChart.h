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

#pragma once

// used by BasketTrading
// similar to PanelChartHdf5 (which might be refactorable/replaceable)

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/panel.h>
#include <wx/splitter.h>

#include "WinChartView.h"

class wxMenu;
class wxTreeCtrl;
class wxTreeEvent;
class wxTreeItemId;

namespace ou { // One Unified
namespace tf { // TradeFrame

class TreeItem;

#define SYMBOL_PANELFINANCIALCHART_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELFINANCIALCHART_TITLE _("Panel Financial Chart")
#define SYMBOL_PANELFINANCIALCHART_IDNAME ID_PANELFINANCIALCHART
#define SYMBOL_PANELFINANCIALCHART_SIZE wxSize(400, 300)
#define SYMBOL_PANELFINANCIALCHART_POSITION wxDefaultPosition

class PanelFinancialChart: public wxPanel {
  friend class boost::serialization::access;
public:

  using TreeItem = ou::tf::TreeItem;

  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  PanelFinancialChart();
  PanelFinancialChart(
    wxWindow* parent,
    wxWindowID id = SYMBOL_PANELFINANCIALCHART_IDNAME,
    const wxPoint& pos = SYMBOL_PANELFINANCIALCHART_POSITION,
    const wxSize& size = SYMBOL_PANELFINANCIALCHART_SIZE,
    long style = SYMBOL_PANELFINANCIALCHART_STYLE );
  ~PanelFinancialChart();

  bool Create(
    wxWindow* parent, wxWindowID id = SYMBOL_PANELFINANCIALCHART_IDNAME,
    const wxPoint& pos = SYMBOL_PANELFINANCIALCHART_POSITION,
    const wxSize& size = SYMBOL_PANELFINANCIALCHART_SIZE,
    long style = SYMBOL_PANELFINANCIALCHART_STYLE );

  TreeItem* SetRoot( const std::string& sName, pChartDataView_t );
  void SetChartDataView( pChartDataView_t );

protected:

  enum { ID_Null=wxID_HIGHEST, ID_PANELFINANCIALCHART, ID_CHART
  };

private:

  wxTreeCtrl* m_pTree;
  TreeItem* m_pTreeItem; // root of custom tree items
  wxSplitterWindow* m_pSplitter;

  WinChartView* m_pWinChartView; // handles drawing the chart
  pChartDataView_t m_pChartDataView; // for use with tooltip

  void Init();
  void CreateControls();

  void HandleTreeEventItemGetToolTip( wxTreeEvent& );

  void OnDestroy( wxWindowDestroyEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    double d( m_pSplitter->GetSashGravity() );
    ar & d;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    double d;
    ar & d;
    m_pSplitter->SetSashGravity( d );
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

  static bool ShowToolTips() { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::PanelFinancialChart, 1)

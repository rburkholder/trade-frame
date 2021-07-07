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

#include <functional>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/panel.h>
#include <wx/splitter.h>

#include "WinChartView.h"

class wxTreeItemId;
class wxTreeCtrl;
class wxTreeEvent;

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANELFINANCIALCHART_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELFINANCIALCHART_TITLE _("Panel Financial Chart")
#define SYMBOL_PANELFINANCIALCHART_IDNAME ID_PANELFINANCIALCHART
#define SYMBOL_PANELFINANCIALCHART_SIZE wxSize(400, 300)
#define SYMBOL_PANELFINANCIALCHART_POSITION wxDefaultPosition

class PanelFinancialChart: public wxPanel {
  friend class boost::serialization::access;
public:

  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  PanelFinancialChart(void);
  PanelFinancialChart(
    wxWindow* parent,
    wxWindowID id = SYMBOL_PANELFINANCIALCHART_IDNAME,
    const wxPoint& pos = SYMBOL_PANELFINANCIALCHART_POSITION,
    const wxSize& size = SYMBOL_PANELFINANCIALCHART_SIZE,
    long style = SYMBOL_PANELFINANCIALCHART_STYLE );
  ~PanelFinancialChart(void);

  bool Create(
    wxWindow* parent, wxWindowID id = SYMBOL_PANELFINANCIALCHART_IDNAME,
    const wxPoint& pos = SYMBOL_PANELFINANCIALCHART_POSITION,
    const wxSize& size = SYMBOL_PANELFINANCIALCHART_SIZE,
    long style = SYMBOL_PANELFINANCIALCHART_STYLE );

  using fAdd_t = std::function<wxTreeItemId()>;
  using fDel_t = std::function<void(wxTreeItemId)>;
  // using fComposeMenu_t = std::funcion<

  struct TreeItemFunctions {
    fAdd_t fAdd;
    fDel_t fDel;
    TreeItemFunctions() {}
    TreeItemFunctions( fAdd_t&& fAdd_, fDel_t&& fDel_ )
    : fAdd( std::move( fAdd_ ) ), fDel( std::move( fDel_ ) )
    {}
    TreeItemFunctions( const TreeItemFunctions&& tif )
    : fAdd( std::move( tif.fAdd ) ), fDel( std::move( tif.fDel ) )
    {}

  };

//  wxTreeItemId SetRoot(                  const std::string& sName, pChartDataView_t, TreeItemFunctions&& );
//  wxTreeItemId AppendItem( wxTreeItemId, const std::string& sName, pChartDataView_t, TreeItemFunctions&& );
  wxTreeItemId SetRoot(                  const std::string& sName, pChartDataView_t );
  wxTreeItemId AppendItem( wxTreeItemId, const std::string& sName, pChartDataView_t );
  void DeleteItem( wxTreeItemId );

protected:

  enum { ID_Null=wxID_HIGHEST, ID_PANELFINANCIALCHART, ID_CHART
  };

private:

  wxSplitterWindow* m_pSplitter;
  wxTreeCtrl* m_pTree;
  WinChartView* m_pWinChartView; // handles drawing the chart

  void Init();
  void CreateControls();

  void HandleTreeEventItemActivated( wxTreeEvent& );
  void HandleTreeEventItemRightClick( wxTreeEvent& );
  void HandleTreeEventItemMenu( wxTreeEvent& );
  void HandleTreeEventItemGetToolTip( wxTreeEvent& );

  void OnClose( wxCloseEvent& event );

  static bool ShowToolTips() { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & m_pSplitter->GetSashPosition();
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    int x;
    ar & x;
    m_pSplitter->SetSashPosition( x );
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::PanelFinancialChart, 1)

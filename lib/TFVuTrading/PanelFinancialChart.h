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

#include <wx/treectrl.h>

#include "WinChartView.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANELFINANCIALCHART_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELFINANCIALCHART_TITLE _("Panel Financial Chart")
#define SYMBOL_PANELFINANCIALCHART_IDNAME ID_PANELFINANCIALCHART
#define SYMBOL_PANELFINANCIALCHART_SIZE wxSize(400, 300)
#define SYMBOL_PANELFINANCIALCHART_POSITION wxDefaultPosition

class PanelFinancialChart: public wxPanel {
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

};

} // namespace tf
} // namespace ou

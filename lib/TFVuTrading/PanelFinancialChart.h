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
// similar to PanelChartHdf5 (which might be refactorable)

#include <memory>

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

  void UpdateRoot( const std::string& sName, pChartDataView_t );
  void AppendActive( const std::string& sName, pChartDataView_t );
  void AppendInfo( const std::string& sName, pChartDataView_t );

  static bool ShowToolTips() { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

protected:

  enum { ID_Null=wxID_HIGHEST, ID_PANELFINANCIALCHART, ID_CHART
  };

private:

  wxTreeCtrl* m_pTree;
  WinChartView* m_pWinChartView; // handles drawing the chart

  wxTreeItemId m_idRoot;
  wxTreeItemId m_idActive;
  wxTreeItemId m_idInfo;

  void Init();
  void CreateControls();

  void HandleTreeEventItemActivated( wxTreeEvent& event );
  void HandleTreeEventItemGetToolTip( wxTreeEvent& event );
  void OnClose( wxCloseEvent& event );

};

} // namespace tf
} // namespace ou

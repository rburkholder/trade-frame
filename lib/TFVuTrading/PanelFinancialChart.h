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

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANELFINANCIALCHART_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELFINANCIALCHART_TITLE _("PanelFinancialChart")
#define SYMBOL_PANELFINANCIALCHART_IDNAME ID_PANELFINANCIALCHART
#define SYMBOL_PANELFINANCIALCHART_SIZE wxSize(400, 300)
#define SYMBOL_PANELFINANCIALCHART_POSITION wxDefaultPosition

class PanelFinancialChart: public wxPanel {
public:

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
  void Init();
  void CreateControls();

  static bool ShowToolTips() { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

protected:
private:
  enum { ID_Null=wxID_HIGHEST, ID_PANELFINANCIALCHART, ID_CHART
  };
  wxWindow* m_winChart;
};

} // namespace tf
} // namespace ou

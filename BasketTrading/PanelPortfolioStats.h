/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// Started 2012/12/22

#include <string>

#include <wx/panel.h>

#define SYMBOL_PANELPORTFOLIOSTATS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELPORTFOLIOSTATS_TITLE _("PanelPortfolioStats")
#define SYMBOL_PANELPORTFOLIOSTATS_IDNAME ID_PANELPORTFOLIOSTATS
//#define SYMBOL_PANELPORTFOLIOSTATS_SIZE wxSize(400, 300)
#define SYMBOL_PANELPORTFOLIOSTATS_SIZE wxDefaultSize
#define SYMBOL_PANELPORTFOLIOSTATS_POSITION wxDefaultPosition


class PanelPortfolioStats: public wxPanel {
public:
  PanelPortfolioStats(void);
  PanelPortfolioStats( 
    wxWindow* parent, 
    wxWindowID id = SYMBOL_PANELPORTFOLIOSTATS_IDNAME, 
    const wxPoint& pos = SYMBOL_PANELPORTFOLIOSTATS_POSITION, 
    const wxSize& size = SYMBOL_PANELPORTFOLIOSTATS_SIZE, 
    long style = SYMBOL_PANELPORTFOLIOSTATS_STYLE );
  bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANELPORTFOLIOSTATS_IDNAME, 
    const wxPoint& pos = SYMBOL_PANELPORTFOLIOSTATS_POSITION, 
    const wxSize& size = SYMBOL_PANELPORTFOLIOSTATS_SIZE, 
    long style = SYMBOL_PANELPORTFOLIOSTATS_STYLE );
  ~PanelPortfolioStats(void);
  void SetStats( const std::string& sMin, const std::string& sCurrent, const std::string& sMax );
protected:
private:
  enum { ID_NULL=wxID_HIGHEST, SYMBOL_PANELPORTFOLIOSTATS_IDNAME,
    ID_TxtPortfolioLow, ID_TxtPortfolioCurrent, ID_TxtPortfolioHigh
  };

    wxTextCtrl* m_txtPortfolioLow;
    wxTextCtrl* m_txtPortfolioCurrent;
    wxTextCtrl* m_txtPortfolioHigh;

  void Init();
  void CreateControls();
  bool ShowToolTips( void ) { return true; };

};


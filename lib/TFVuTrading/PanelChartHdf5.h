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

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANEL_CHARTHDF5_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_CHARTHDF5_TITLE _("Panel Chart HDF5 Data")
#define SYMBOL_PANEL_CHARTHDF5_IDNAME ID_PANEL_CHARTHDF5
#define SYMBOL_PANEL_CHARTHDF5_SIZE wxSize(400, 300)
#define SYMBOL_PANEL_CHARTHDF5_POSITION wxDefaultPosition

class PanelChartHdf5: public wxPanel {
public:

  PanelChartHdf5(void);
  PanelChartHdf5( 
    wxWindow* parent, wxWindowID id = SYMBOL_PANEL_CHARTHDF5_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_CHARTHDF5_POSITION, 
    const wxSize& size = SYMBOL_PANEL_CHARTHDF5_SIZE, 
    long style = SYMBOL_PANEL_CHARTHDF5_STYLE );
  ~PanelChartHdf5(void);

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_PANEL_CHARTHDF5_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_CHARTHDF5_POSITION, 
    const wxSize& size = SYMBOL_PANEL_CHARTHDF5_SIZE, 
    long style = SYMBOL_PANEL_CHARTHDF5_STYLE );
  void Init();
  void CreateControls();

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

protected:
private:
  enum { ID_Null=wxID_HIGHEST, ID_PANEL_CHARTHDF5 };
};

} // namespace tf
} // namespace ou

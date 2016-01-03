/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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
// Started December 30, 2015, 3:40 PM

#pragma once

#include <memory>

#include <wx/panel.h>

#include <TFInteractiveBrokers/IBTWS.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANEL_AccountDetails_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_AccountDetails_TITLE _("Panel IB Account Details")
#define SYMBOL_PANEL_AccountDetails_IDNAME ID_PANELACCOUNTDETAILS
#define SYMBOL_PANEL_AccountDetails_SIZE wxSize(400, 300)
#define SYMBOL_PANEL_AccountDetails_POSITION wxDefaultPosition

class PanelIBPositionDetails_impl;  // Forward declaration

class PanelIBPositionDetails: public wxPanel {
    friend class PanelIBPositionDetails_impl;
public:
  PanelIBPositionDetails(void);
  PanelIBPositionDetails( 
    wxWindow* parent, wxWindowID id = SYMBOL_PANEL_AccountDetails_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_AccountDetails_POSITION, 
    const wxSize& size = SYMBOL_PANEL_AccountDetails_SIZE, 
    long style = SYMBOL_PANEL_AccountDetails_STYLE );
  ~PanelIBPositionDetails(void);

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_PANEL_AccountDetails_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_AccountDetails_POSITION, 
    const wxSize& size = SYMBOL_PANEL_AccountDetails_SIZE, 
    long style = SYMBOL_PANEL_AccountDetails_STYLE );
  
  void UpdatePositionDetailRow( const ou::tf::IBTWS::PositionDetail& ad );

protected:
    void Init();
private:
  
  enum { ID_Null=wxID_HIGHEST, ID_PANELACCOUNTDETAILS, 
    ID_GRID_ACCOUNTDETAILS
  };
  
  std::unique_ptr<PanelIBPositionDetails_impl> m_pimpl;

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };
};

} // namespace tf
} // namespace ou

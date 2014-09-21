/************************************************************************
 * Copyright(c) 2014, One Unified. All rights reserved.                 *
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
// Started 2014/09/10

#pragma once

#include <memory>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANEL_OPTIONDETAILS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_OPTIONDETAILS_TITLE _("Panel Option Details")
#define SYMBOL_PANEL_OPTIONDETAILS_IDNAME ID_PANEL_OPTIONDETAILS
#define SYMBOL_PANEL_OPTIONDETAILS_SIZE wxSize(-1, -1)
#define SYMBOL_PANEL_OPTIONDETAILS_POSITION wxDefaultPosition

class PanelOptionDetails_impl;  // Forward Declaraion

class PanelOptionDetails: public wxPanel {
  friend PanelOptionDetails_impl;
public:

  PanelOptionDetails(void);
  PanelOptionDetails( 
    wxWindow* parent, wxWindowID id = SYMBOL_PANEL_OPTIONDETAILS_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_OPTIONDETAILS_POSITION, 
    const wxSize& size = SYMBOL_PANEL_OPTIONDETAILS_SIZE, 
    long style = SYMBOL_PANEL_OPTIONDETAILS_STYLE );
  ~PanelOptionDetails(void);

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_PANEL_OPTIONDETAILS_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_OPTIONDETAILS_POSITION, 
    const wxSize& size = SYMBOL_PANEL_OPTIONDETAILS_SIZE, 
    long style = SYMBOL_PANEL_OPTIONDETAILS_STYLE );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

protected:

  void Init( void );

private:
  enum { 
    ID_Null=wxID_HIGHEST, ID_PANEL_OPTIONDETAILS
  };

  std::unique_ptr<PanelOptionDetails_impl> m_pimpl;
};

} // namespace tf
} // namespace ou
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
// Started 2013/11/06

#pragma once

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANEL_PORTFOLIOPOSITION_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_PORTFOLIOPOSITION_TITLE _("Panel Portfolio Position")
#define SYMBOL_PANEL_PORTFOLIOPOSITION_IDNAME ID_PANEL_PORTFOLIOPOSITION
#define SYMBOL_PANEL_PORTFOLIOPOSITION_SIZE wxSize(400, 300)
#define SYMBOL_PANEL_PORTFOLIOPOSITION_POSITION wxDefaultPosition

class PanelPortfolioPosition: public wxPanel {
public:
  PanelPortfolioPosition(void);
  PanelPortfolioPosition( 
    wxWindow* parent, wxWindowID id = SYMBOL_PANEL_PORTFOLIOPOSITION_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_PORTFOLIOPOSITION_POSITION, 
    const wxSize& size = SYMBOL_PANEL_PORTFOLIOPOSITION_SIZE, 
    long style = SYMBOL_PANEL_PORTFOLIOPOSITION_STYLE );
  ~PanelPortfolioPosition(void);

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_PANEL_PORTFOLIOPOSITION_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_PORTFOLIOPOSITION_POSITION, 
    const wxSize& size = SYMBOL_PANEL_PORTFOLIOPOSITION_SIZE, 
    long style = SYMBOL_PANEL_PORTFOLIOPOSITION_STYLE );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

protected:

  void Init();
  void CreateControls();

private:

  enum { ID_Null=wxID_HIGHEST, ID_PANEL_PORTFOLIOPOSITION };

  void OnClose( wxCloseEvent& event );

};


} // namespace tf
} // namespace ou

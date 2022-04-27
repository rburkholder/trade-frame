/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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

/*
 * File:    PanelSideBySide.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: April 27, 2022 16:38
 */

#pragma once

#include <wx/timer.h>
#include <wx/window.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

#define SYMBOL_PANEL_SIDEBYSIDE_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_SIDEBYSIDE_TITLE _("Market Depth Side by Side")
#define SYMBOL_PANEL_SIDEBYSIDE_IDNAME ID_PANEL_SIDEBYSIDE
#define SYMBOL_PANEL_SIDEBYSIDE_SIZE wxDefaultSize
#define SYMBOL_PANEL_SIDEBYSIDE_POSITION wxDefaultPosition

class PanelTrade: public wxWindow {
public:

  PanelTrade();
  PanelTrade(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANEL_SIDEBYSIDE_IDNAME,
   const wxPoint& pos = SYMBOL_PANEL_SIDEBYSIDE_POSITION,
   const wxSize& size = SYMBOL_PANEL_SIDEBYSIDE_SIZE,
   long style = SYMBOL_PANEL_SIDEBYSIDE_STYLE
   );
  ~PanelTrade(void);

  bool Create(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANEL_SIDEBYSIDE_IDNAME,
   const wxPoint& pos = SYMBOL_PANEL_SIDEBYSIDE_POSITION,
   const wxSize& size = SYMBOL_PANEL_SIDEBYSIDE_SIZE,
   long style = SYMBOL_PANEL_SIDEBYSIDE_STYLE
   );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST, ID_PANEL_SIDEBYSIDE
  };

  //fTimer_t m_fTimer;



};

} // market depth
} // namespace tf
} // namespace ou

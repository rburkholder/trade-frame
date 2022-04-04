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
 * File:    PanelTrade.h
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created on October 28, 2021, 15:55
 */

#pragma once

#include <vector>

#include <wx/window.h>

#include "WinRow.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

#define SYMBOL_PANELTRADE_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELTRADE_TITLE _("Trade Market Depth")
#define SYMBOL_PANELTRADE_IDNAME ID_PANELTRADE
#define SYMBOL_PANELTRADE_SIZE wxDefaultSize
#define SYMBOL_PANELTRADE_POSITION wxDefaultPosition

class PanelTrade: public wxWindow {
public:

  PanelTrade();
  PanelTrade(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANELTRADE_IDNAME,
   const wxPoint& pos = SYMBOL_PANELTRADE_POSITION,
   const wxSize& size = SYMBOL_PANELTRADE_SIZE,
   long style = SYMBOL_PANELTRADE_STYLE
   );
  ~PanelTrade(void);

  bool Create(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANELTRADE_IDNAME,
   const wxPoint& pos = SYMBOL_PANELTRADE_POSITION,
   const wxSize& size = SYMBOL_PANELTRADE_SIZE,
   long style = SYMBOL_PANELTRADE_STYLE
   );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST, ID_PANELTRADE
  };

  using pWinRow_t = WinRow::pWinRow_t;

  unsigned int m_nRowCount; // use m_vRowElements.size() instead?
  unsigned int m_nVisibleRows;
  unsigned int m_nFramedRows;
  unsigned int m_nCenteredRows;

  pWinRow_t m_pWinRow_Header;

  using vWinRow_t = std::vector<pWinRow_t>;
  vWinRow_t m_vWinRow;

  void Init();
  void CreateControls();
  bool ShowToolTips() { return true; };

  void DrawRows();

  void OnPaint( wxPaintEvent& );

  void OnResize( wxSizeEvent& );
  void OnResizing( wxSizeEvent& );
  void OnDestroy( wxWindowDestroyEvent& );

};

} // market depth
} // namespace tf
} // namespace ou

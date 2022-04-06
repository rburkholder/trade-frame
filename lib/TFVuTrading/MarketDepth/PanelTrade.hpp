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

#include <mutex>
#include <vector>

#include <wx/timer.h>
#include <wx/window.h>

#include "WinRow.hpp"
#include "DataRows.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

  class Quote;
  class Trade;

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

  void SetInterval( double );

  void OnQuote( const ou::tf::Quote& );
  void OnTrade( const ou::tf::Trade& );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST, ID_PANELTRADE
  };

  using pWinRow_t = WinRow::pWinRow_t;

  unsigned int m_nFramedRows;
  unsigned int m_nCenteredRows;

  unsigned int m_cntTotalWinRows; // includes header row: TODO: verify all usage locations are correct
  unsigned int m_cntWinRows; // without header row

  int m_ixFirstVisibleRow;
  int m_ixLastVisibleRow;

  int m_ixHiRecenterFrame;
  int m_ixLoRecenterFrame;

  int m_ixLastAsk;
  int m_ixLastBid;

  double m_dblLastPrice;

  pWinRow_t m_pWinRow_Header;

  using vWinRow_t = std::vector<pWinRow_t>;
  vWinRow_t m_vWinRow;

  DataRows m_DataRows;

  std::mutex m_mutexTimer;
  wxTimer m_timerRefresh; // TODO: need to sync foreground & background

  void ReCenterVisible( int ix );

  void DrawRows();
  void DeleteAllRows();

  void OnPaint( wxPaintEvent& );
  void HandleTimerRefresh( wxTimerEvent& );

  void OnResize( wxSizeEvent& );
  void OnResizing( wxSizeEvent& );
  void OnDestroy( wxWindowDestroyEvent& );

  void Init();
  void CreateControls();
  bool ShowToolTips() { return true; };

};

} // market depth
} // namespace tf
} // namespace ou

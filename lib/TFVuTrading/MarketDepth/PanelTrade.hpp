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
#include "PriceRows.hpp"

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
  virtual ~PanelTrade();

  bool Create(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANELTRADE_IDNAME,
   const wxPoint& pos = SYMBOL_PANELTRADE_POSITION,
   const wxSize& size = SYMBOL_PANELTRADE_SIZE,
   long style = SYMBOL_PANELTRADE_STYLE
   );

  // Interface - In - Settings
  void SetInterval( double );  // price step from rung to rung

  // Interface - In - Updates - Feed
  void AppendStaticIndicator( double, const std::string& );
  void UpdateDynamicIndicator( const std::string&, double );

  void OnQuote( const ou::tf::Quote& ); // l1 quote for recentering
  void OnTrade( const ou::tf::Trade& ); // l1 trade for colour, recentering

  void OnQuoteAsk( double price, unsigned int volume ); // l2 update at level
  void OnQuoteBid( double price, unsigned int volume ); // l2 update at level

  // Interface - Events - Out - Timer
  using fTimer_t = std::function<void()>; // triggered on visible ladder refresh
  void SetOnTimer( fTimer_t&& fTimer ) { m_fTimer = std::move( fTimer); }

  // Interface - In - Events - Execution
  using fClick_t = ou::tf::l2::PriceRow::fClick_t;
  void Set( fClick_t&& );

  // Interface - In - Updates - Pending Orders
  using EColour = WinRowElement::EColour;
  void SetAsk( double, int, EColour ); // update pending quantity@price
  void SetBid( double, int, EColour ); // update pending quantity@price

  void UpdateProfitLoss( const int quantity, const double price ); // quantity at the zero profit price

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST, ID_PANELTRADE
  };

  fTimer_t m_fTimer;

  using pWinRow_t = WinRow::pWinRow_t;

  unsigned int m_nFramedRows;
  unsigned int m_nCenteredRows;

  unsigned int m_cntWinRows_Total; // includes header row: TODO: verify all usage locations are correct
  unsigned int m_cntWinRows_Data; // without header row

  int m_ixFirstPriceRow;
  int m_ixLastPriceRow;

  int m_ixHiRecenterFrame;
  int m_ixLoRecenterFrame;

  double m_dblLastAsk;
  double m_dblLastBid;
  double m_dblLastPrice;

  pWinRow_t m_pWinRow_Header;

  using vWinRow_t = std::vector<pWinRow_t>;
  vWinRow_t m_vWinRow; // non header rows only

  PriceRows m_PriceRows;

  bool m_bReCenter;

  std::mutex m_mutexTimer;
  wxTimer m_timerRefresh; // TODO: need to sync foreground & background

  fClick_t m_fClick;

  using mapDynamicIndicator_t = std::map<std::string,int>;
  mapDynamicIndicator_t m_mapDynamicIndicator;

  void ReCenterVisible( int ix );

  void DrawWinRows();
  void DeleteWinRows();

  void OnPaint( wxPaintEvent& );
  void HandleTimerRefresh( wxTimerEvent& );

  void OnResize( wxSizeEvent& );
  void OnResizing( wxSizeEvent& );
  void OnDestroy( wxWindowDestroyEvent& );

  void Init();
  void CreateControls();

};

} // market depth
} // namespace tf
} // namespace ou

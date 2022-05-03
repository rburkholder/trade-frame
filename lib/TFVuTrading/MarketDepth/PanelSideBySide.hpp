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

#include <map>
#include <mutex>

#include <wx/timer.h>
#include <wx/window.h>

#include "WinRow.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

#define SYMBOL_PANEL_SIDEBYSIDE_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_SIDEBYSIDE_TITLE _("Market Depth Side by Side")
#define SYMBOL_PANEL_SIDEBYSIDE_IDNAME ID_PANEL_SIDEBYSIDE
#define SYMBOL_PANEL_SIDEBYSIDE_SIZE wxDefaultSize
#define SYMBOL_PANEL_SIDEBYSIDE_POSITION wxDefaultPosition

class PanelSideBySide: public wxWindow {
public:

  PanelSideBySide();
  PanelSideBySide(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANEL_SIDEBYSIDE_IDNAME,
   const wxPoint& pos = SYMBOL_PANEL_SIDEBYSIDE_POSITION,
   const wxSize& size = SYMBOL_PANEL_SIDEBYSIDE_SIZE,
   long style = SYMBOL_PANEL_SIDEBYSIDE_STYLE
   );
  virtual ~PanelSideBySide();

  bool Create(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANEL_SIDEBYSIDE_IDNAME,
   const wxPoint& pos = SYMBOL_PANEL_SIDEBYSIDE_POSITION,
   const wxSize& size = SYMBOL_PANEL_SIDEBYSIDE_SIZE,
   long style = SYMBOL_PANEL_SIDEBYSIDE_STYLE
   );

  void OnL2Ask( double price, int volume, bool bOnAdd );
  void OnL2Bid( double price, int volume, bool bOnAdd );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST, ID_PANEL_SIDEBYSIDE
  };

  using pWinRow_t = WinRow::pWinRow_t;
  pWinRow_t m_pWinRow_Header;

  using vWinRow_t = std::vector<pWinRow_t>;
  vWinRow_t m_vWinRow; // non header rows only

  unsigned int m_cntWinRows_Total; // includes header row: TODO: verify all usage locations are correct
  unsigned int m_cntWinRows_Data; // without header row

  // TODO: turn into DataRow, and link WinElement into DataRowElement
  //   remember to blank out WinElement prior to unlink
  struct PriceLevel {
    int nVolume;
    //int nVolumeAggregate; // may not be needed
    int nOrders;
    //int nOrdersAggregate; // may not be needed
    //PriceLevel(): nVolume {}, nVolumeAggregate {}, nOrders {}, nOrdersAggregate {} {}
    PriceLevel(): nVolume {}, nOrders {} {}
    //PriceLevel( int nVolume_ ): nVolume( nVolume_ ), nVolumeAggregate {}, nOrders {}, nOrdersAggregate {} {}
    PriceLevel( int nVolume_ ): nVolume( nVolume_ ), nOrders {} {}
  };

  using mapPriceLevel_t = std::map<double,PriceLevel>;

  mapPriceLevel_t m_mapAskPriceLevel;
  mapPriceLevel_t m_mapBidPriceLevel;

  std::mutex m_mutexMaps;
  wxTimer m_timerRefresh;

  void Init();
  void CreateControls();
  void DrawWinRows();
  void DeleteWinRows();

  void UpdateMap( mapPriceLevel_t& map, double price, int volume );
  void CalculateStatistics();

  void HandleTimerRefresh( wxTimerEvent& );

  void OnResize( wxSizeEvent& );
  void OnResizing( wxSizeEvent& );
  void OnDestroy( wxWindowDestroyEvent& );

};

} // market depth
} // namespace tf
} // namespace ou

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
#include "DataRowElement.hpp"

extern const ou::tf::l2::vElement_t vElement;

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

  using fImbalanceStats_t = std::function<void(double,double)>; // mean, slope
  void Set( fImbalanceStats_t&& f ) {
    m_fImbalanceStats = std::move( f );
  }

  enum class EStatsField: int {
    BSizeAgg, BSize, BPrice, Imbalance, APrice, ASize, ASizeAgg
  };

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

  static const std::string sFmtInteger;
  static const std::string sFmtPrice;
  static const std::string sFmtString;

  // TODO: need to fix the colours from the quick_fix defaults

  struct DataRow_Book { // one for Bid, one for Ask

    bool m_bChanged;
    ou::tf::l2::DataRowElement<double> m_drePrice;
    ou::tf::l2::DataRowElement<unsigned int> m_dreSize;
    ou::tf::l2::DataRowElement<unsigned int> m_dreSizeAgg;

    DataRow_Book()
    : m_bChanged( false )
    , m_drePrice( m_bChanged, sFmtPrice, EColour::Black, EColour::LightSeaGreen )
    , m_dreSize( m_bChanged, sFmtInteger, EColour::Black, EColour::DodgerBlue )
    , m_dreSizeAgg( m_bChanged, sFmtInteger, EColour::Black, EColour::LightSkyBlue )
    {}

    DataRow_Book( const DataRow_Book& rhs )  // don't copy or move anything
    : m_bChanged( false )
    , m_drePrice( m_bChanged, rhs.m_drePrice )
    , m_dreSize(  m_bChanged, rhs.m_dreSize )
    , m_dreSizeAgg( m_bChanged, m_dreSizeAgg )
    {
      m_drePrice.Set( rhs.m_drePrice.Get() );
      m_dreSize.Set( rhs.m_dreSize.Get() );
    }

    DataRow_Book( double price, unsigned int volume )
    : m_bChanged( false )
    , m_drePrice( m_bChanged, sFmtPrice, EColour::Black, EColour::LightSeaGreen )
    , m_dreSize( m_bChanged, sFmtInteger, EColour::Black, EColour::DodgerBlue )
    , m_dreSizeAgg( m_bChanged, sFmtInteger, EColour::Black, EColour::LightSkyBlue )
    {
      m_drePrice.Set( price );
      m_dreSize.Set( volume );
    }

    DataRow_Book( DataRow_Book&& ) = delete; // due to m_bChanged usage

    void Set( unsigned int volume ) { m_dreSize.Set( volume ); }

    void Update() {
      m_drePrice.UpdateWinRowElement();
      m_dreSize.UpdateWinRowElement();
      m_dreSizeAgg.UpdateWinRowElement();
      m_bChanged = false;
    }
  };

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

  using mapPriceLevel_t = std::map<double,DataRow_Book>;

  mapPriceLevel_t m_mapAskPriceLevel;
  mapPriceLevel_t m_mapBidPriceLevel;

  struct DataRow_Statistics {
    bool m_bChanged;
    ou::tf::l2::DataRowElement<double> m_dreImbalance;
    DataRow_Statistics()
    : m_bChanged( false )
    , m_dreImbalance( m_bChanged, sFmtPrice, EColour::Black, EColour::DimGray )
    {}
    void Update() {
      if ( m_bChanged ) {
        m_dreImbalance.UpdateWinRowElement();
        m_bChanged = false;
      }
    }
  };

  using pDataRow_Statistics_t = std::unique_ptr<DataRow_Statistics>;
  using vStatistics_t = std::vector<pDataRow_Statistics_t>;
  vStatistics_t m_vStatistics;

  std::mutex m_mutexMaps;
  wxTimer m_timerRefresh;

  fImbalanceStats_t m_fImbalanceStats;

  void Init();
  void CreateControls();
  void DrawWinRows();
  void DeleteWinRows();

  void UpdateMap( mapPriceLevel_t& map, double price, int volume, bool bOnAdd );
  void CalculateStatistics();

  void HandleTimerRefresh( wxTimerEvent& );

  void OnResize( wxSizeEvent& );
  void OnResizing( wxSizeEvent& );
  void OnDestroy( wxWindowDestroyEvent& );
  void OnCloseWindow( wxCloseEvent& );

};

} // market depth
} // namespace tf
} // namespace ou

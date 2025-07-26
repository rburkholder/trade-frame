/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    InstrumentViews.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 18, 2025 10:26:28
 */

#pragma once

#include <set>
#include <memory>
#include <unordered_map>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/panel.h>
#include <wx/timer.h>

#include "Common.hpp"
#include "OptionChainView.hpp"
#include "SessionBarModel.hpp"
#include "DailyBarModel.hpp"

#define SYMBOL_INSTRUMENTVIEWS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_INSTRUMENTVIEWS_TITLE _("Instrument Views")
#define SYMBOL_INSTRUMENTVIEWS_IDNAME ID_BOOKOPTIONCHAINS
#define SYMBOL_INSTRUMENTVIEWS_SIZE wxDefaultSize
#define SYMBOL_INSTRUMENTVIEWS_POSITION wxDefaultPosition

class wxTreeCtrl;
class wxTreeEvent;
class OptionChainModel;

namespace ou { // One Unified
namespace tf { // TradeFrame

class TreeItem;
class WinChartView;
class ComposeInstrument;

class InstrumentViews
: public wxPanel
{
  friend class boost::serialization::access;
public:

  InstrumentViews();
  InstrumentViews(
    wxWindow* parent, wxWindowID id = SYMBOL_INSTRUMENTVIEWS_IDNAME,
    const wxPoint& pos = SYMBOL_INSTRUMENTVIEWS_POSITION,
    const wxSize& size = SYMBOL_INSTRUMENTVIEWS_SIZE,
    long style = SYMBOL_INSTRUMENTVIEWS_STYLE,
    const wxString& name = SYMBOL_INSTRUMENTVIEWS_TITLE );
  virtual ~InstrumentViews();

  bool Create( wxWindow* parent,
    wxWindowID id = SYMBOL_INSTRUMENTVIEWS_IDNAME,
    const wxPoint& pos = SYMBOL_INSTRUMENTVIEWS_POSITION,
    const wxSize& size = SYMBOL_INSTRUMENTVIEWS_SIZE,
    long style = SYMBOL_INSTRUMENTVIEWS_STYLE,
    const wxString& name = SYMBOL_INSTRUMENTVIEWS_TITLE );

  using pComposeInstrument_t = std::shared_ptr<ComposeInstrument>;

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  using fBuildWatch_t = std::function<pWatch_t( pInstrument_t )>;
  using fBuildOption_t = std::function<pOption_t( pInstrument_t )>;

  using pOptionEngine_t = std::shared_ptr<ou::tf::option::Engine>;

  using fHistory_Bar_t = std::function<void( const ou::tf::Bar& )>;
  using fHistory_Done_t = std::function<void()>;
  using fHistoryRequest_session_t = std::function<void( const std::string&, unsigned int, unsigned int, fHistory_Bar_t&&, fHistory_Done_t&& )>;
  using fHistoryRequest_daily_t = std::function<void( const std::string&, unsigned int, fHistory_Bar_t&&, fHistory_Done_t&& )>;

  void Set(
    pComposeInstrument_t&
  , fBuildWatch_t&&
  , fBuildOption_t&&
  , pOptionEngine_t&
  , fHistoryRequest_session_t&&
  , ou::tf::WinChartView* pWinChartView_session
  , fHistoryRequest_daily_t&&
  , ou::tf::WinChartView* pWinChartView_daily
  );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST
  , ID_BOOKOPTIONCHAINS
  , ID_TREECTRL
  };

  wxTimer m_timerRefresh;

  wxTreeCtrl* m_pTreeCtrl;
  TreeItem* m_pRootTreeItem; // // root of custom tree items

  using setInstrumentName_t = std::set<std::string>;
  setInstrumentName_t m_setInstrumentName;

  pComposeInstrument_t m_pComposeInstrument;  // IQFeed only, or IQFeed & IB

  fBuildWatch_t m_fBuildWatch;
  fBuildOption_t m_fBuildOption;

  pOptionEngine_t m_pOptionEngine;

  fHistoryRequest_session_t m_fHistoryBars_session;
  ou::tf::WinChartView* m_pWinChartView_session;

  fHistoryRequest_daily_t m_fHistoryBars_daily;
  ou::tf::WinChartView* m_pWinChartView_daily;

  struct Instrument {

    ou::tf::TreeItem* pti;
    pInstrument_t pInstrument;
    pWatch_t pWatch;
    OptionChainView* pChainView;
    mapChains_t mapChains;
    SessionBarModel sbm;
    DailyBarModel dbm;

    Instrument()
    : pti( nullptr ), pChainView( nullptr )
    {}
    Instrument( Instrument&& rhs ) {}

    void Set( pWatch_t pWatch_ ) {
      assert( !pWatch );
      pWatch = pWatch_;
      pWatch->StartWatch();
      sbm.Set( pWatch );
    }

    ~Instrument() {
      if ( pWatch ) {
        pWatch->StopWatch();
        pWatch.reset();
      }
      pInstrument.reset();
      pChainView = nullptr;
    }
  };

  using mapInstrument_t = std::unordered_map<std::string, Instrument>;
  mapInstrument_t m_mapInstrument;

  wxWindow* m_pcurView; // OptionChainView, LiveView, BarChart, ... show/hide current
  OptionChainView* m_pOptionChainView; // only one OptionChainView required, should be able to simply swap models
  OptionChainModel* m_pOptionChainModel; // model switches out for each option chain, ensure retired prior to View

  void Init();
  void CreateControls();
  void HandleTreeEventItemGetToolTip( wxTreeEvent& );
  void HandleTreeEventItemExpanded( wxTreeEvent& );
  void OnDestroy( wxWindowDestroyEvent& event );

  void SizeTreeCtrl();

  void DialogSymbol();
  void BuildInstrument( const std::string& );
  void AddInstrument( pInstrument_t& );
  void BuildOptionChains( mapInstrument_t::iterator );
  void PresentOptionChains( mapInstrument_t::iterator );
  void OptionChainView_select();

  void BuildSessionBarModel( Instrument& );
  void BuildDailyBarModel( Instrument& );

  void HandleTimer( wxTimerEvent& );

  wxBitmap GetBitmapResource( const wxString& name );
  static bool ShowToolTips() { return true; };

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & m_mapInstrument.size();
    for ( const mapInstrument_t::value_type& vt: m_mapInstrument ) {
      ar & vt.first;
    }
    ar & *m_pOptionChainView;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    size_t nNames;
    ar & nNames;
    std::string sName;
    while ( 0 != nNames ) {
      ar & sName;
      m_setInstrumentName.insert( sName ); // process once iqfeed connected
      --nNames;
    }
    if ( 2 <= version ) {
      ar & *m_pOptionChainView;
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::InstrumentViews, 2)

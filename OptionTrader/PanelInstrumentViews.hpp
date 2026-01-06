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
 * File:    PanelInstrumentViews.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 18, 2025 10:26:28
 */

#pragma once

#include <memory>
#include <unordered_map>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/panel.h>
#include <wx/timer.h>
#include <wx/checklst.h>

#include <TFVuTrading/PanelDividendNotes.hpp>

#include "Common.hpp"
#include "OptionChainView.hpp"
#include "SessionBarModel.hpp"
#include "ManualDailyBarModel.hpp"
#include "TagSymbolMap.hpp"

#define SYMBOL_INSTRUMENTVIEWS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_INSTRUMENTVIEWS_TITLE _("Instrument Views")
#define SYMBOL_INSTRUMENTVIEWS_IDNAME ID_BOOKOPTIONCHAINS
#define SYMBOL_INSTRUMENTVIEWS_SIZE wxDefaultSize
#define SYMBOL_INSTRUMENTVIEWS_POSITION wxDefaultPosition

class wxTreeCtrl;
class wxTreeEvent;
class wxStatusBar;

class wxCommandEvent;

class OptionChainModel;

namespace ou { // One Unified
namespace tf { // TradeFrame

namespace iqfeed {
  class BarHistory;
}
namespace option {
  class Engine;
}

class TreeItem;
class WinChartView;
class WatchOnStatusBar;
class ComposeInstrument;
class GridOptionComboOrder;

class PanelInstrumentViews
: public wxPanel
{
  friend class boost::serialization::access;
public:

  PanelInstrumentViews();
  PanelInstrumentViews(
    wxWindow* parent, wxWindowID id = SYMBOL_INSTRUMENTVIEWS_IDNAME,
    const wxPoint& pos = SYMBOL_INSTRUMENTVIEWS_POSITION,
    const wxSize& size = SYMBOL_INSTRUMENTVIEWS_SIZE,
    long style = SYMBOL_INSTRUMENTVIEWS_STYLE,
    const wxString& name = SYMBOL_INSTRUMENTVIEWS_TITLE );
  virtual ~PanelInstrumentViews();

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

  using pBarHistory_t = std::unique_ptr<ou::tf::iqfeed::BarHistory>;

  using fUpdateDividendFields_t = std::function<void( const ou::tf::PanelDividendNotes::Fields&, const wxArrayString& )>;

  void Set(
    pComposeInstrument_t&
  , fBuildWatch_t&&
  , fBuildOption_t&&
  , pOptionEngine_t&
  , pBarHistory_t&&
  , ou::tf::WinChartView* pWinChartView_session
  , ou::tf::WinChartView* pWinChartView_daily
  , fUpdateDividendFields_t&&
  );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST
  , ID_BOOKOPTIONCHAINS
  , ID_TREECTRL
  , ID_STATUSBAR
  };

  wxTimer m_timerRefresh;

  wxCheckListBox* m_clbTags;

  wxTreeCtrl* m_pTreeCtrl;
  TreeItem* m_pRootTreeItem; // root of custom tree items

  //OptionComboGridView* m_pOptionComboGridView;
  //OptionComboGridTable* m_pOptionComboGridTable;

  GridOptionComboOrder* m_pGridOptionComboOrder;

  wxStatusBar* m_pStatusBar;

  struct StateCache {
    //std::string sIQFeedSymbolName;
    std::string sDvidendNotes;
    StateCache() {}
    StateCache( StateCache&& rhs )
    : sDvidendNotes( std::move( rhs.sDvidendNotes ) )
    {}
  };
  using mapStateCache_t = std::unordered_map<std::string,StateCache>; // key = sIQFeedSymbolName
  mapStateCache_t m_mapStateCache;

  pComposeInstrument_t m_pComposeInstrument;  // IQFeed only, or IQFeed & IB

  fBuildWatch_t m_fBuildWatch;
  fBuildOption_t m_fBuildOption;

  pOptionEngine_t m_pOptionEngine;

  pBarHistory_t m_pBarHistory;

  ou::tf::WinChartView* m_pWinChartView_session;
  ou::tf::WinChartView* m_pWinChartView_daily;

  fUpdateDividendFields_t m_fUpdateDividendFields;

  struct DividendNotes {

    bool bLoaded;  // todo:  is this necessary?

    double dblLast;
    double dblRate;
    double dblYield;
    double dblAmount;

    std::string sCompanyName;
    std::string sNotes;

    boost::gregorian::date datePayed;
    boost::gregorian::date dateExDividend;

    DividendNotes()
    : bLoaded( false )
    , dblLast {}
    , dblRate {}
    , dblYield {}
    , dblAmount {}
    {}

    using Fundamentals = ou::tf::Watch::Fundamentals;
    void OnFundamentals( const Fundamentals& fundamentals ) {
      dblRate = fundamentals.dblDividendRate;
      dblYield = fundamentals.dblDividendYield;
      dblAmount = fundamentals.dblDividendAmount;
      sCompanyName = fundamentals.sCompanyName;
      dateExDividend = fundamentals.dateExDividend;
      datePayed = fundamentals.datePayed;
    }

    using Summary = ou::tf::Watch::Summary;
    void OnSummary( const Summary& summary ) {
      dblLast = summary.dblTrade;
      //dblLast = summary.dblOpen;
    }

  };

  struct Instrument {

    ou::tf::TreeItem* pti;
    pInstrument_t pInstrument;
    pWatch_t pWatch;
    mapChains_t mapChains;
    SessionBarModel sbm;
    ManualDailyBarModel mdbm;
    DividendNotes notesDividend;

    Instrument()
    : pti( nullptr )
    {}
    Instrument( Instrument&& rhs ) {}

    void Set( pWatch_t pWatch_ ) {
      assert( !pWatch );
      pWatch = pWatch_;
      pWatch->OnFundamentals.Add( MakeDelegate( &notesDividend, &DividendNotes::OnFundamentals ) );
      pWatch->OnSummary.Add( MakeDelegate( &notesDividend, &DividendNotes::OnSummary ) ) ;
      pWatch->StartWatch();
      sbm.Set( pWatch );
      sbm.Set( mdbm.Statistics() );
    }

    ~Instrument() {
      if ( pWatch ) {
        pWatch->StopWatch();
        pWatch->OnFundamentals.Remove( MakeDelegate( &notesDividend, &DividendNotes::OnFundamentals ) );
        pWatch->OnSummary.Remove( MakeDelegate( &notesDividend, &DividendNotes::OnSummary ) ) ;
        pWatch.reset();
      }
      mapChains.clear();
      pInstrument.reset();
    }
  };

  // key is sIQFeedSymbolName
  using mapInstrument_t = std::unordered_map<std::string, Instrument>;
  mapInstrument_t m_mapInstrument;

  TagSymbolMap m_TagSymbolMap;

  wxWindow* m_pcurView; // OptionChainView, LiveView, BarChart, ... show/hide current
  OptionChainView* m_pOptionChainView; // only one OptionChainView required, should be able to simply swap models
  OptionChainModel* m_pOptionChainModel; // model switches out for each option chain, ensure retired prior to View

  using pWatchOnStatusBar_t = std::unique_ptr<WatchOnStatusBar>;
  pWatchOnStatusBar_t m_pWatchOnStatusBar;

  void Init();
  void CreateControls();
  void HandleTreeEventItemGetToolTip( wxTreeEvent& );
  void HandleTreeEventItemExpanded( wxTreeEvent& );
  void OnDestroy( wxWindowDestroyEvent& event );

  void SizeTreeCtrl();

  void DialogSymbol();
  void BuildInstrument( const std::string& );
  void AddInstrument( pInstrument_t& );
  void AddInstrumentToTree( Instrument& );
  void BuildOptionChains( Instrument& );
  void PresentOptionChains( Instrument& );
  void OptionChainView_select();

  void UpdateDividendNotes( Instrument& );

  void BuildSessionBarModel( Instrument& );
  void BuildDailyBarModel( Instrument& );

  void HandleTimer( wxTimerEvent& );

  void AddTag( const TagSymbolMap::sTag_t&, const TagSymbolMap::sSymbol_t& );
  void DelTag( const TagSymbolMap::sTag_t&, const TagSymbolMap::sSymbol_t& );
  void FilterByTag();
  void HandleCheckListBoxEvent( wxCommandEvent& );

  wxBitmap GetBitmapResource( const wxString& name );
  static bool ShowToolTips() { return true; };

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pOptionChainView;

    ar & m_TagSymbolMap;

    ar & m_mapInstrument.size();
    for ( const mapInstrument_t::value_type& vt: m_mapInstrument ) {
      ar & vt.first;
      ar & vt.second.notesDividend.sNotes;
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    assert( 5 <= version ); // ordering has been changed

    ar & *m_pOptionChainView;

    ar & m_TagSymbolMap;
    wxArrayString rTag;
    m_TagSymbolMap.TagList(
      [this,&rTag]( const TagSymbolMap::sTag_t& sTag ){
        rTag.Add( sTag );
      } );
    if ( 0 < rTag.size() ) {
      m_clbTags->InsertItems( rTag, 0 );
    }

    size_t nNames;
    ar & nNames;
    std::string sName;
    while ( 0 != nNames ) {
      std::string sIQFeedSymbolName;
      StateCache sc;
      ar & sIQFeedSymbolName;
      ar & sc.sDvidendNotes;
      // todo: check for duplicates?
      auto result = m_mapStateCache.emplace( sIQFeedSymbolName, std::move( sc ) );
      assert( result.second );
      --nNames;
    }

  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

class WatchOnStatusBar {
  using pWatch_t = ou::tf::Watch::pWatch_t;
public:
  WatchOnStatusBar( pWatch_t pWatch )
  : m_price {}
  {
    assert( pWatch );
    m_pWatch = pWatch;
    m_pWatch->OnTrade.Add( MakeDelegate( this, &WatchOnStatusBar::HandleTrade ) );
    m_pWatch->StartWatch();
  }
  ~WatchOnStatusBar() {
    if ( m_pWatch ) {
      m_pWatch->StopWatch();
      m_pWatch->OnTrade.Remove( MakeDelegate( this, &WatchOnStatusBar::HandleTrade ) );
      m_pWatch.reset();
    }
  }
  double Price() const { return m_price; }
protected:
private:
  double m_price;
  pWatch_t m_pWatch;
  void HandleTrade( const ou::tf::Trade& trade ) {
    m_price = trade.Price();
  }
};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::PanelInstrumentViews, 5)

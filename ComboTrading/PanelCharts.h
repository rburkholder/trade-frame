/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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

// started December 6, 2015, 1:26 PM

#pragma once

#include <map>
#include <functional>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/treectrl.h>
#include <wx/panel.h>
#include <wx/event.h>
#include <wx/splitter.h>
#include <wx/notebook.h>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/Watch.h>

#include <TFOptions/Option.h>
#include <TFOptions/NoRiskInterestRateSeries.h>

#include <TFBitsNPieces/TreeOps.h>
#include <TFBitsNPieces/GridColumnSizer.h>

#include <OUCharting/ChartDataView.h>
#include <TFVuTrading/ModelChartHdf5.h>

#include <TFVuTrading/GridOptionChain.h>

#include "TreeItem.h"
#include "TreeItemGroup.h"
#include "InstrumentActions.h"


namespace ou { // One Unified
namespace tf { // TradeFrame

#define PANEL_CHARTS_STYLE wxTAB_TRAVERSAL
#define PANEL_CHARTS_TITLE _("Panel Chart Data")
#define PANEL_CHARTS_IDNAME ID_PANEL_CHARTS
#define PANEL_CHARTS_SIZE wxSize(400, 300)
#define PANEL_CHARTS_POSITION wxDefaultPosition

class PanelCharts: public wxPanel {
  friend class boost::serialization::access;
public:

  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::Watch::pWatch_t pWatch_t;
  typedef ou::tf::option::Option::pOption_t pOption_t;

  PanelCharts( void );
  PanelCharts( wxWindow* parent, wxWindowID id = PANEL_CHARTS_IDNAME,
    const wxPoint& pos = PANEL_CHARTS_POSITION,
    const wxSize& size = PANEL_CHARTS_SIZE,
    long style =         PANEL_CHARTS_STYLE );
  virtual ~PanelCharts();

  bool Create( wxWindow* parent,
    wxWindowID id =      PANEL_CHARTS_IDNAME,
    const wxPoint& pos = PANEL_CHARTS_POSITION,
    const wxSize& size = PANEL_CHARTS_SIZE,
    long style =         PANEL_CHARTS_STYLE );

  typedef boost::signals2::signal<void(pInstrument_t)> signalRegisterInstrument_t;
  typedef signalRegisterInstrument_t::slot_type slotRegisterInstrument_t;
  signalRegisterInstrument_t signalRegisterInstrument;

  typedef boost::signals2::signal<pInstrument_t(const std::string&), ou::tf::FirstOrDefault<pInstrument_t> > signalLoadInstrument_t;
  typedef signalLoadInstrument_t::slot_type slotLoadInstrument_t;
  signalLoadInstrument_t signalLoadInstrument;

  typedef std::function<void(const ou::tf::iqfeed::MarketSymbol::TableRowDef&)> fSymbol_t;
  typedef boost::signals2::signal<void(const std::string&,fSymbol_t)> signalRetrieveOptionList_t;
  typedef signalRetrieveOptionList_t::slot_type slotRetrieveOptionList_t;
  signalRetrieveOptionList_t signalRetrieveOptionList;

  std::function<pInstrument_t(const ou::tf::Allowed::enumInstrument, const wxString&)> m_fSelectInstrument; // pop up for symbol / instrument selection
  std::function<pInstrument_t(const std::string&)> m_fBuildInstrumentFromIqfeed; // build instrument from grid / option chain click

  typedef std::function<void(pInstrument_t)> fBuildOptionInstrumentComplete_t;

  typedef std::function<void(pInstrument_t /* underlying */, const std::string& /* iqfeed option name */, boost::gregorian::date, double, fBuildOptionInstrumentComplete_t )> fBuildOptionInstrument_t;
  fBuildOptionInstrument_t m_fBuildOptionInstrument; // build registered option instrument with IQF and IB info.

  typedef std::function<void(pOption_t, pWatch_t)> fCalcOptionGreek_Add_t;
  fCalcOptionGreek_Add_t m_fCalcOptionGreek_Add;

  typedef std::function<void(pOption_t, pWatch_t)> fCalcOptionGreek_Remove_t;
  fCalcOptionGreek_Remove_t m_fCalcOptionGreek_Remove;

  typedef std::function<void(pInstrument_t, pOption_t&)> fBuildOption_t;
  fBuildOption_t m_fBuildOption;

  typedef std::function<void(pInstrument_t, pWatch_t&)> fBuildWatch_t;
  fBuildWatch_t m_fBuildWatch;

  // providers may change, so what happens to providers already registered with an instrument?
  typedef ou::tf::ProviderManager::pProvider_t pProvider_t;
  //void SetProviders( pProvider_t pData1Provider, pProvider_t pData2Provider, pProvider_t pExecutionProvider );

  // called from owner to perform regular updates
  //void CalcIV( boost::posix_time::ptime dt, ou::tf::LiborFromIQFeed& libor );  // can this be removed now?

  void SaveSeries( const std::string& sPrefix, const std::string& sDaily );

protected:

  void Init();
  void CreateControls();

private:

  enum {
    ID_Null=wxID_HIGHEST, ID_PANEL_CHARTS,
    MIRoot, MIGroup, MIInstrument, MIPortfolio, MIPosition
  };

  typedef InstrumentActions::pInstrumentActions_t pInstrumentActions_t;
  pInstrumentActions_t m_pInstrumentActions;

  ou::tf::GridColumnSizer m_gcsGridOptionChain;

  // =======
  // purpose: populates m_chartData for display of indicators on the LiveChartPanel
  // maybe need to put a lock on the structure or the container for threaded greek calc?
  // or not, as m_mapInstrumentWatch is used for calcs
  struct WatchInfo {
  public:
    typedef boost::shared_ptr<WatchInfo> pWatchInfo_t;
    WatchInfo( pWatch_t pWatch )
    : m_bActive( false )
    { Set( pWatch ); }
    void Set( pWatch_t pWatch ) {
      //std::cout << "WatchInfo::Set " << pWatch->GetInstrument()->GetInstrumentName() << std::endl;
      if ( m_bActive ) {
        std::cout << "WatchInfo::Set menu item already activated" << std::endl;
      }
      else {
        //std::cout << "WatchInfo::Set (activate) " << pWatch->GetInstrument()->GetInstrumentName() << std::endl;
        m_bActive = true;
	      m_pWatch = pWatch;
        pInstrument_t pInstrument = m_pWatch->GetInstrument();
        if ( pInstrument->IsOption() || pInstrument->IsFuturesOption() ) {
          ou::tf::option::Option* pOption = dynamic_cast<ou::tf::option::Option*>( m_pWatch.get() );
          pOption->OnGreek.Add( MakeDelegate( &m_chartData, &ou::tf::ModelChartHdf5::HandleGreek ) );
        }
        m_pWatch->OnQuote.Add( MakeDelegate( &m_chartData, &ou::tf::ModelChartHdf5::HandleQuote ) );
        m_pWatch->OnTrade.Add( MakeDelegate( &m_chartData, &ou::tf::ModelChartHdf5::HandleTrade ) );
        //std::cout << "WatchInfo::Set (before) " << pWatch->GetInstrument()->GetInstrumentName() << std::endl;
        m_pWatch->StartWatch();
        //std::cout << "WatchInfo::Set (after) " << pWatch->GetInstrument()->GetInstrumentName() << std::endl;
      }
    }
    virtual ~WatchInfo( void ) {
      //std::cout << "WatchInfo::~WatchInfo " << m_pWatch->GetInstrument()->GetInstrumentName() << std::endl;
      if ( 0 == m_pWatch.use_count() ) {
        std::cout << "WatchInfo use_count is 0, bad thing" << std::endl;
      }
      else {
        if ( m_bActive ) {
          m_bActive = false;
          m_pWatch->StopWatch();
          if ( m_pWatch->GetInstrument()->IsOption() || m_pWatch->GetInstrument()->IsFuturesOption() ) {
            ou::tf::option::Option* pOption = dynamic_cast<ou::tf::option::Option*>( m_pWatch.get() );
            pOption->OnGreek.Remove( MakeDelegate( &m_chartData, &ou::tf::ModelChartHdf5::HandleGreek ) );
          }
          m_pWatch->OnQuote.Remove( MakeDelegate( &m_chartData, &ou::tf::ModelChartHdf5::HandleQuote ) );
          m_pWatch->OnTrade.Remove( MakeDelegate( &m_chartData, &ou::tf::ModelChartHdf5::HandleTrade ) );
        }
      }
    }
    //pWatch_t GetWatch() { return m_pWatch; }
    //void EmitValues( void ) { m_pWatch->EmitValues(); }
    void ApplyDataTo( ou::ChartDataView* view ) {
      assert( m_pWatch );
      pInstrument_t pInstrument = m_pWatch->GetInstrument();
//      if ( pInstrument->IsOption() || pInstrument->IsFuturesOption() ) {
//        m_chartData.DefineChartOptions( view );
//      }
//      else {
//        m_chartData.DefineChartEquities( view );
//      }
    }
  private:

    bool m_bActive;
    pWatch_t m_pWatch;
    ou::tf::ModelChartHdf5 m_chartData;

  };
  // =======

  typedef WatchInfo::pWatchInfo_t pWatchInfo_t;

  // TODO: better .second to reset chain display when map is cleared
  //   chains shouldn't capture time series
  typedef std::map<std::string,pOption_t> mapOption_t; // iqfeed updates in the option chains

  typedef std::map<void*,ou::tf::Instrument::idInstrument_t> mapItemToInstrument_t;
  mapItemToInstrument_t m_mapItemToInstrument;  // translate menu item id to instrument [many::1]

  // TODO: need to check move semantics to see if things get watched/unwatched
  struct InstrumentEntry {
    size_t m_cntMenuDependents;
    bool m_bAddedToEngine;
    pWatch_t m_pWatch;  // primary watch entry
    pWatch_t m_pWatchUnderlying;  // established from hierarchical menu items, when entry is option or foption
    pWatchInfo_t m_pWatchInfo;  // live chart view
    mapOption_t m_mapSelectedChainOptions;  // contains list of active watch options in chains
    // position list -- to be implemented
    // portfolio list -- to be implemented
    InstrumentEntry( pWatch_t m_pWatch_, pWatchInfo_t pWatchInfo_ )
      : m_cntMenuDependents {}, m_bAddedToEngine( false ), m_pWatch( m_pWatch_ ), m_pWatchInfo( pWatchInfo_ ) {}
    virtual ~InstrumentEntry() {
      if ( m_bAddedToEngine ) {
        std::cout << "~InstrumentEntry: " << m_pWatch->GetInstrument()->GetInstrumentName() << " hasn't been removed from Engine" << std::endl;
      }
      m_mapSelectedChainOptions.clear();
      m_pWatchInfo.reset();
      m_pWatchUnderlying.reset();
      m_pWatch.reset();
    }
  };

  // unique list of instrument/watches, for all listed instruments
  // instruments have been registered with instrument manager
  // used for saving series, and the underlying for option calcs
  typedef ou::tf::Instrument::idInstrument_t idInstrument_t;
  typedef std::map<idInstrument_t, InstrumentEntry> mapInstrumentEntry_t;
  mapInstrumentEntry_t m_mapInstrumentEntry;

  //pProvider_t m_pData1Provider;
  //pProvider_t m_pData2Provider;
  //pProvider_t m_pExecutionProvider;

  ou::tf::TreeItemResources m_baseResources;
  Resources m_resources;
  ou::tf::TreeOps* m_pTreeOps;

  boost::signals2::connection m_connGetInstrumentActions;
  boost::signals2::connection m_connNewInstrument;
  boost::signals2::connection m_connLoadInstrument;
  boost::signals2::connection m_connEmitValues;
  boost::signals2::connection m_connLiveChart;
  boost::signals2::connection m_connOptionList;
  boost::signals2::connection m_connDelete;
  boost::signals2::connection m_connChanging;

  ou::ChartDataView m_ChartDataView;

  wxWindow* m_winRightDetail;

  wxPanel* m_panelSplitterRightPanel;
  wxSplitterWindow* m_splitter;
  wxBoxSizer* m_sizerRight;

  void HandleTreeOpsChanging( wxTreeItemId id );

  InstrumentActions::values_t HandleNewInstrumentRequest(
    const wxTreeItemId& item,
    const ou::tf::Allowed::enumInstrument,
    const wxString& sUnderlying
  );

  void HandleLoadInstrument( const wxTreeItemId& item, const std::string& sName, const std::string& sUnderlying );
  void ConstructInstrumentEntry( const wxTreeItemId& item, pInstrument_t, const std::string& sUnderlying );

  void HandleInstrumentLiveChart( const wxTreeItemId& );

  void HandleEmitValues( const wxTreeItemId& );
  void HandleOptionChainList( const wxTreeItemId& );

  void HandleMenuItemDelete( const wxTreeItemId& id );

  void RemoveRightDetail();
  void ReplaceRightDetail( wxWindow* );
  void HandleGridClick(
    idInstrument_t,
    boost::gregorian::date date, double strike, bool bSelected,
    const ou::tf::GridOptionChain::OptionUpdateFunctions& funcCall,
    const ou::tf::GridOptionChain::OptionUpdateFunctions& funcPut );

  void OnOptionChainPageChanging( boost::gregorian::date );
  void OnOptionChainPageChanged( boost::gregorian::date );

  pInstrumentActions_t HandleGetInstrumentActions( const wxTreeItemId& );

  void OnClose( wxCloseEvent& event );
  void OnWindowDestroy( wxWindowDestroyEvent& );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & m_splitter->GetSashPosition();
    m_pTreeOps->Save<TreeItemRoot>( ar );
    if ( 2 <= version ) {
      ar & m_gcsGridOptionChain;
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    int pos;  // for SashPosition
    ar & pos;
    m_splitter->SetSashPosition( pos );
    m_pTreeOps->Load<TreeItemRoot>( ar );
    if ( 2 <= version ) {
      ar & m_gcsGridOptionChain;
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::PanelCharts, 2)

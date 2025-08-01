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

#pragma once

#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/thread.hpp>

#include <wx/splitter.h>

#include <OUCommon/Worker.h>

#include <TFTrading/DBOps.h>
#include <TFTrading/PortfolioManager.h>

#include <TFOptions/Engine.hpp>
#include <TFOptions/NoRiskInterestRateSeries.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/PanelManualOrder.h>
#include <TFVuTrading/PanelOptionCombo.h>
#include <TFVuTrading/PanelPortfolioPosition.h>
#include <TFVuTrading/GridColumnSizer.h>
#include <TFVuTrading/GridIBPositionDetails.h>
#include <TFVuTrading/GridIBAccountValues.h>

#include <TFVuTrading/ModelPortfolioPositionOrderExecution.h>
#include <TFVuTrading/PanelPortfolioPositionOrderExecution.h>
#include <TFVuTrading/ControllerPortfolioPositionOrderExecution.h>

#include <TFBitsNPieces/FrameWork01.h>
#include <TFBitsNPieces/IQFeedSymbolListOps.h>
#include <TFBitsNPieces/IQFeedInstrumentBuild.h>

#include "Process.h"
#include "PanelCharts.h"
#include "BundleTracking.h"

/// \brief Option Combo Trading, code started 2015/11/08
///
/// Based upon the book 'Day Trading Options' by Jeff Augen

class AppComboTrading:
  public wxApp, public ou::tf::FrameWork01<AppComboTrading> {
    friend class boost::serialization::access;
    friend ou::tf::FrameWork01<AppComboTrading>;
public:
protected:
private:

  typedef ou::tf::Portfolio::idPortfolio_t idPortfolio_t;

  typedef ou::tf::Portfolio::pPortfolio_t pPortfolio_t;
  typedef ou::tf::Position::pPosition_t pPosition_t;

  typedef ou::tf::ModelPortfolioPositionOrderExecution MPPOE_t;
  typedef ou::tf::PanelPortfolioPositionOrderExecution PPPOE_t;
  typedef ou::tf::ControllerPortfolioPositionOrderExecution CPPOE_t;

  typedef ou::tf::Instrument::idInstrument_t idInstrument_t;
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;

  typedef ou::tf::Watch::pWatch_t pWatch_t;
  typedef ou::tf::option::Option::pOption_t pOption_t;

  typedef ou::tf::PanelPortfolioPosition::fAddPosition_t fAddPostion_t;

  typedef std::vector<BundleTracking> vBundleTracking_t;

  typedef std::function<void(pInstrument_t)> fInstrumentFromIB_t;

  struct structManualOrder {
//    ou::tf::PanelManualOrder* pDialogManualOrder;
    ou::tf::ib::TWS::ContractDetails details;
    pInstrument_t pInstrument;
  } m_IBInstrumentInfo;

  struct structConstructEquityPosition {
    pPortfolio_t pPortfolio;
    fAddPostion_t fAddPosition;
    //DelegateAddPosition_t function;
  } m_EquityPositionCallbackInfo;

  template<typename T>
  struct structAutoNull {
    T* pT;
    structAutoNull(): pT( nullptr ) {}
    structAutoNull( T* pT_ ): pT( pT_ ) {}
    ~structAutoNull() { pT = nullptr; }
  };

  typedef structAutoNull<ou::tf::PanelPortfolioPosition> structPortfolioTrading;
  typedef structAutoNull<ou::tf::PanelOptionCombo> structPortfolioSandbox;

  typedef std::map<std::string,structPortfolioTrading> mapPortfoliosTrading_t;
  typedef std::map<std::string,structPortfolioSandbox> mapPortfoliosSandbox_t;

  ou::action::Worker m_worker;

  wxTimer m_timerGuiRefresh;

  double m_dblMaxPL;
  double m_dblMinPL;

  //ou::tf::LiborFromIQFeed m_libor; // deprecated
  ou::tf::FedRateFromIQFeed m_fedrate;

  std::unique_ptr<ou::tf::option::Engine> m_pOptionEngine;

  Process m_process;  // single position

  vBundleTracking_t m_vBundleTracking; // one per underlying and related options to track

  ou::tf::keytypes::idPortfolio_t m_idPortfolioMaster;
  pPortfolio_t m_pPortfolioMaster;

  mapPortfoliosTrading_t m_mapPortfoliosTrading;
  mapPortfoliosSandbox_t m_mapPortfoliosSandbox;

  using mapWatch_t = std::unordered_map<std::string, pWatch_t>;
  mapWatch_t m_mapWatch;

  using mapOption_t = std::unordered_map<std::string, pOption_t>;
  mapOption_t m_mapOption;

  ou::tf::PanelPortfolioPosition* m_pLastPPP;  // helps getting new positions to correct window

  FrameMain* m_pFrameMain;
  FrameMain* m_pFPP;
  FrameMain* m_pFPPOE;
  FrameMain* m_pFCharts;
  FrameMain* m_pFInteractiveBrokers;
  FrameMain* m_pFOC;

  ou::tf::PanelCharts* m_pPanelCharts;

//  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;
  ou::tf::PanelManualOrder* m_pPanelManualOrder;
  //ou::tf::PanelOptionCombo* m_pPanelOptionCombo;

  typedef ou::tf::PanelOptionCombo PanelOptionCombo;
  typedef PanelOptionCombo* pPanelOptionCombo_t;
  //typedef std::vector<PanelOptionCombo*> vPanelOptionCombo_t;
  //vPanelOptionCombo_t m_vPanelOptionCombo;

  MPPOE_t* m_pMPPOE;
  PPPOE_t* m_pPPPOE;
  CPPOE_t* m_pCPPOE;

    wxSplitterWindow* m_splitPanels;
    ou::tf::GridIBAccountValues* m_pPanelIBAccountValues;
    ou::tf::GridIBPositionDetails* m_pPanelIBPositionDetails;

  wxBoxSizer* m_sizerPM;
  wxScrolledWindow* m_scrollPM;
  wxBoxSizer* m_sizerScrollPM;

  ou::tf::GridColumnSizer m_gcsPanelPortfolioPosition;
  ou::tf::GridColumnSizer m_gcsPanelOptionCombo;

  ou::tf::DBOps m_db;

  wxBoxSizer* m_sizerFOC;
  wxScrolledWindow* m_scrollFOC;
  wxBoxSizer* m_sizerScrollOC;

  ou::tf::iqfeed::InMemoryMktSymbolList m_listIQFeedSymbols;
  ou::tf::IQFeedSymbolListOps* m_pIQFeedSymbolListOps;
  ou::tf::IQFeedSymbolListOps::vExchanges_t m_vExchanges;
  ou::tf::IQFeedSymbolListOps::vClassifiers_t m_vClassifiers;
  void LookupDescription( const std::string& sSymbolName, std::string& sDescription );
  void BuildInstrument( ou::tf::IQFeedInstrumentBuild::ValuesForBuildInstrument&, fInstrumentFromIB_t );
  void RegisterInstrument( pInstrument_t );
  pInstrument_t LoadInstrument( const std::string& );

  void HandleSave( wxCommandEvent& event );
  void HandleLoad( wxCommandEvent& event );

  void GetContractFor( const std::string& sBaseName, pInstrument_t pInstrument, fInstrumentFromIB_t );
  void LoadUpBundle( ou::tf::Instrument::pInstrument_t pInstrument );

  void ConstructEquityPosition1a( const std::string& sName, pPortfolio_t, fAddPostion_t);  // step 1
  void ConstructEquityPosition1b( pInstrument_t, pPortfolio_t, fAddPostion_t);  // step 1
  void ConstructEquityPosition2( pInstrument_t& pInstrument ); // step 2

  virtual bool OnInit();
  //virtual int OnExit();
  virtual void OnAssertFailure( const wxChar*, int, const wxChar*, const wxChar*, const wxChar* );
  //virtual void HandleEvent( wxEvtHandler*, wxEventFunction, wxEvent& ) const;
  void OnClose( wxCloseEvent& event );

  void TestSymbols( void );

  void HandleMenuActionEmitYieldCurve( void );
  void HandleMenuActionSaveSeries( void );

  void HandleSaveValues( void );

  void BuildFrameCharts( void );
  void BuildFramePortfolioPosition( void );
  void BuildFrameInteractiveBrokers( void );

  void BuildFrameOptionCombo( void );

  void HandlePanelNewOrder( const ou::tf::PanelManualOrder::Order_t& order );
  void HandlePanelSymbolText( const std::string& sName );  // use IB to start, use IQFeed symbol file later on
  void HandlePanelFocusPropogate( unsigned int ix );

  void OnData1Connecting( int );
  void OnData1Connected( int );
  void OnData1Disconnecting( int );
  void OnData1Disconnected( int );

  void OnExecConnecting( int );
  void OnExecConnected( int );
  void OnExecDisconnecting( int );
  void OnExecDisconnected( int );

  void OnIQFeedConnected( int );
  void OnIQFeedDisconnecting( int );

  void HandleSaveButton( void );

  void HandlePopulateDatabase( void );
  void HandleLoadDatabase( void );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );

  void HandleGuiRefresh( wxTimerEvent& event );

  void HandlePortfolioLoad( pPortfolio_t& pPortfolio );
  void HandlePositionLoad( pPosition_t& pPosition );

  pWatch_t BuildWatch( pInstrument_t );
  pOption_t BuildOption( pInstrument_t );

  void ProvideOptionList( const std::string& sSymbol, ou::tf::PanelCharts::fSymbol_t );

//  void HandleMenuActionInitializeSymbolSet();
  void HandleMenuActionSaveSymbolSubset();
  void HandleMenuActionLoadSymbolSubset();

  pPanelOptionCombo_t HandleNewPanelOptionCombo( const idPortfolio_t& idPortfolio, const std::string& sDescription );

  void HandleConstructPortfolio( ou::tf::PanelPortfolioPosition&,const std::string&, const std::string& ); // portfolioid, description

  void UpdateColumns_PanelPortfolioPositions();
  void UpdateColumns_PanelOptionCombo();

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    //ar & boost::serialization::base_object<const TreeItemResources>(*this);
    ar & *m_pFrameMain;
    ar & *m_pFCharts;
    ar & *m_pFInteractiveBrokers;
    ar & *m_pFPP;
    ar & *m_pFOC;
    ar & *m_pFPPOE;
    ar & m_splitPanels->GetSashPosition();
    ar & *m_pPanelIBAccountValues;
    ar & *m_pPanelIBPositionDetails;
    ar & *m_pPanelCharts;
    ar & m_mapPortfoliosSandbox.size();
    std::for_each( m_mapPortfoliosSandbox.begin(), m_mapPortfoliosSandbox.end(), [&ar](const mapPortfoliosSandbox_t::value_type& vt){
      ar & vt.first;
      ar & *vt.second.pT;
    });
    ar & m_gcsPanelPortfolioPosition;
    ar & m_gcsPanelOptionCombo;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    //ar & boost::serialization::base_object<TreeItemResources>(*this);
    ar & *m_pFrameMain;
    ar & *m_pFCharts;
    ar & *m_pFInteractiveBrokers;
    if ( 3 <= version ) {
      ar & *m_pFPP;
    }
    if ( 4 <= version ) {
      ar & *m_pFOC;
    }
    if ( 2 <= version ) {
      ar & *m_pFPPOE;
    }
    int x;
    ar & x;
    m_splitPanels->SetSashPosition( x );
    ar & *m_pPanelIBAccountValues;
    ar & *m_pPanelIBPositionDetails;
    if ( 5 <= version ) {
      ar & *m_pPanelCharts;
    }
    if ( 8 <= version ) {
      mapPortfoliosSandbox_t::size_type size;
      ar & size;
      assert( 1 <= size );  // ComboTrading creates at least one panel
      ou::tf::PortfolioGreek::idPortfolio_t idPortfolio;
      ar & idPortfolio;  // discard the first idPortfolio
      ar & *m_mapPortfoliosSandbox.begin()->second.pT; // recreate the prebuilt entry
      size--;  // account for the prebuilt entry
      for ( mapPortfoliosSandbox_t::size_type ix = 0; ix < size; ix++ ) {
        ar & idPortfolio;
        pPanelOptionCombo_t pPanelOptionCombo;
        pPanelOptionCombo = HandleNewPanelOptionCombo( idPortfolio, "" );
        ar & *pPanelOptionCombo;
        //m_sizerScrollOC->FitInside( pPanelOptionCombo );
        //m_sizerFOC->FitInside( m_scrollFOC );
      }
    }
    else {
      if ( 7 <= version ) {
        //pPanelOptionCombo_t pPanelOptionCombo;
        //pPanelOptionCombo = HandleNewPanelOptionCombo( "sandbox", "pre-load" );
        ar & *m_mapPortfoliosSandbox.begin()->second.pT; // should only be one entry;
      }
    }
    if ( 6 <= version ) {
      ar & m_gcsPanelPortfolioPosition;
      UpdateColumns_PanelPortfolioPositions();
      ar & m_gcsPanelOptionCombo;
      UpdateColumns_PanelOptionCombo();
    }

    //m_sizerFOC->FitInside( m_scrollFOC );
    //m_sizerScrollOC->FitInside( m_scrollFOC );
    m_sizerScrollOC->Layout();
    m_sizerFOC->Layout();

  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppComboTrading, 8)
DECLARE_APP(AppComboTrading)


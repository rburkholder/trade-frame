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

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/splitter.h>

#include <TFBitsNPieces/FrameWork01.h>
#include <TFBitsNPieces/IQFeedSymbolListOps.h>

// may need to inherit and add more functionality to the class:
#include <TFTrading/DBOps.h>
#include <TFTrading/PortfolioManager.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
//#include <TFVuTrading/PanelManualOrder.h>
#include <TFVuTrading/PanelPortfolioPosition.h>
#include <TFVuTrading/PanelIBPositionDetails.h>
#include <TFVuTrading/PanelIBAccountValues.h>

#include <TFInteractiveBrokers/EventIBInstrument.h>

#include "Process.h"
#include "BundleTracking.h"
#include "PanelCharts.h"

//#include <TFVuTrading/ModelPortfolioPositionOrderExecution.h>
//#include <TFVuTrading/PanelPortfolioPositionOrderExecution.h>
//#include <TFVuTrading/ControllerPortfolioPositionOrderExecution.h>

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

//  typedef ou::tf::ModelPortfolioPositionOrderExecution MPPOE_t;
//  typedef ou::tf::PanelPortfolioPositionOrderExecution PPPOE_t;
//  typedef ou::tf::ControllerPortfolioPositionOrderExecution CPPOE_t;

  //typedef ou::tf::IBTWS::pInstrument_t pInstrument_t;
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;

  typedef ou::tf::PanelPortfolioPosition::DelegateAddPosition_t DelegateAddPosition_t;
  
  typedef std::vector<BundleTracking> vBundleTracking_t;
  
  typedef boost::signals2::signal<void (pInstrument_t)> signalInstrumentFromIB_t;
  typedef signalInstrumentFromIB_t::slot_type slotInstrumentFromIB_t;
  signalInstrumentFromIB_t signalInstrumentFromIB;

  struct structManualOrder {
//    ou::tf::PanelManualOrder* pDialogManualOrder;
    ou::tf::IBTWS::ContractDetails details;
    pInstrument_t pInstrument;
  } m_IBInstrumentInfo;

  struct structConstructEquityPosition {
    pPortfolio_t pPortfolio;
    DelegateAddPosition_t function;
  } m_EquityPositionCallbackInfo;

  struct structPortfolio {
    ou::tf::PanelPortfolioPosition* pPPP;
    structPortfolio( void ): pPPP( 0 ) {};
    structPortfolio( ou::tf::PanelPortfolioPosition* pPPP_ ): pPPP( pPPP_ ) {}
  };

  typedef std::map<std::string,structPortfolio> mapPortfolios_t;

  // maybe serialize these settings?
  //bool m_bData1Connected;  // available in FrameWork01
  //bool m_bExecConnected;  // available in FrameWork01
  bool m_bStarted;

  std::string m_sDbName;

  wxTimer m_timerGuiRefresh;

  double m_dblMaxPL;
  double m_dblMinPL;
  
  Process m_process;  // single position
  
  vBundleTracking_t m_vBundleTracking; // one per underlying and related options to track

  ou::tf::keytypes::idPortfolio_t m_idPortfolioMaster;
  pPortfolio_t m_pPortfolioMaster;
  mapPortfolios_t m_mapPortfolios;

  ou::tf::PanelPortfolioPosition* m_pLastPPP;  // helps getting new positions to correct window

  FrameMain* m_pFrameMain;
  FrameMain* m_pFPPOE;
  FrameMain* m_pFCharts;
  FrameMain* m_pFInteractiveBrokers;
  
  ou::tf::PanelCharts* m_pPanelCharts;
  
//  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;
//  ou::tf::PanelManualOrder* m_pPanelManualOrder;
//  MPPOE_t* m_pMPPOE;
//  PPPOE_t* m_pPPPOE;
//  CPPOE_t* m_pCPPOE;
  
    wxSplitterWindow* m_splitPanels;
    ou::tf::PanelIBAccountValues* m_pPanelIBAccountValues;
    ou::tf::PanelIBPositionDetails* m_pPanelIBPositionDetails;  
    
  wxBoxSizer* m_sizerPM;
  wxScrolledWindow* m_scrollPM;
  wxBoxSizer* m_sizerScrollPM;

  ou::tf::DBOps m_db;
  std::string m_sWorkingDirectory;
  std::string m_sfnState;

  ou::tf::iqfeed::InMemoryMktSymbolList m_listIQFeedSymbols;
  ou::tf::IQFeedSymbolListOps* m_pIQFeedSymbolListOps;
  ou::tf::IQFeedSymbolListOps::vExchanges_t m_vExchanges;
  ou::tf::IQFeedSymbolListOps::vClassifiers_t m_vClassifiers;
  void LookupDescription( const std::string& sSymbolName, std::string& sDescription );
  void BuildInstrument( ou::tf::PanelCharts::ValuesForBuildInstrument& );
  void RegisterInstrument( pInstrument_t );
  pInstrument_t LoadInstrument( const std::string& );
  
  void HandleSave( wxCommandEvent& event );
  void HandleLoad( wxCommandEvent& event );

  void GetContractFor( const std::string& sBaseName, pInstrument_t pInstrument );
  void LoadUpBundle( ou::tf::Instrument::pInstrument_t pInstrument );
  
  void ConstructEquityPosition0( const std::string& sName, pPortfolio_t, DelegateAddPosition_t);  // step 1
  void ConstructEquityPosition1( pInstrument_t& pInstrument ); // step 2

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void Start( void );
  void Stop( void );
  void TestSymbols( void );
  
  void BuildFrameCharts( void );
  void BuildFramePortfolioPosition( void );
  void BuildFrameInteractiveBrokers( void );

//  void HandlePanelNewOrder( const ou::tf::PanelManualOrder::Order_t& order );
  void HandlePanelSymbolText( const std::string& sName );  // use IB to start, use IQFeed symbol file later on
  void HandlePanelFocusPropogate( unsigned int ix );

  void HandleIBContractDetails( const ou::tf::IBTWS::ContractDetails&, pInstrument_t& pInstrument );
  void HandleIBContractDetailsDone( void );

  void OnData1Connecting( int );
  void OnData1Connected( int );
  void OnData1Disconnecting( int );
  void OnData1Disconnected( int );

  void OnExecConnecting( int );
  void OnExecConnected( int );
  void OnExecDisconnecting( int );
  void OnExecDisconnected( int );

  void HandleSaveButton( void );

  void HandlePopulateDatabase( void );
  void HandleLoadDatabase( void );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );

  void HandleGuiRefresh( wxTimerEvent& event );

  void HandleIBInstrument( EventIBInstrument& event );

  void HandlePortfolioLoad( pPortfolio_t& pPortfolio );
  void HandlePositionLoad( pPosition_t& pPosition );

//  void HandleMenuActionInitializeSymbolSet( void );
  void HandleMenuActionSaveSymbolSubset( void );
  void HandleMenuActionLoadSymbolSubset( void );

  void HandleConstructPortfolio( ou::tf::PanelPortfolioPosition&,const std::string&, const std::string& ); // portfolioid, description

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    //ar & boost::serialization::base_object<const TreeItemResources>(*this);
    ar & *m_pFrameMain;
    ar & *m_pFCharts;
    ar & *m_pFInteractiveBrokers;
    ar & m_splitPanels->GetSashPosition();
    ar & *m_pPanelIBAccountValues;
    ar & *m_pPanelIBPositionDetails;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    //ar & boost::serialization::base_object<TreeItemResources>(*this);
    ar & *m_pFrameMain;
    ar & *m_pFCharts;
    ar & *m_pFInteractiveBrokers;
    int x;
    ar & x;
    m_splitPanels->SetSashPosition( x );
    ar & *m_pPanelIBAccountValues;
    ar & *m_pPanelIBPositionDetails;
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
    
};

BOOST_CLASS_VERSION(AppComboTrading, 1)
DECLARE_APP(AppComboTrading)


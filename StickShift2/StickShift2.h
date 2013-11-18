/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 2013/11/06
// This is manual based trading strategy, hence the name stick shift, for manual trading

#include <map>

#include <TFBitsNPieces/FrameWork01.h>
#include <TFBitsNPieces/IQFeedSymbolListOps.h>

// may need to inherit and add more functionality to the class:
#include <TFTrading/DBOps.h>
#include <TFTrading/PortfolioManager.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/PanelManualOrder.h>

#include "PanelPortfolioPosition.h"
#include "EventIBInstrument.h"

//#include <TFVuTrading/ModelPortfolioPositionOrderExecution.h>
//#include <TFVuTrading/PanelPortfolioPositionOrderExecution.h>
//#include <TFVuTrading/ControllerPortfolioPositionOrderExecution.h>

class AppStickShift:
  public wxApp, public ou::tf::FrameWork01<AppStickShift> {
    friend ou::tf::FrameWork01<AppStickShift>;
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

  bool m_bData1Connected;
  bool m_bExecConnected;

  wxTimer m_timerGuiRefresh;

  double m_dblMaxPL;
  double m_dblMinPL;

  ou::tf::keytypes::idPortfolio_t m_idPortfolioMaster;
  pPortfolio_t m_pPortfolioMaster;
  mapPortfolios_t m_mapPortfolios;

  FrameMain* m_pFrameMain;
  FrameMain* m_pFPPOE;
//  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;
  ou::tf::PanelManualOrder* m_pPanelManualOrder;
//  MPPOE_t* m_pMPPOE;
//  PPPOE_t* m_pPPPOE;
//  CPPOE_t* m_pCPPOE;

  wxBoxSizer* m_sizerPM;
  wxScrolledWindow* m_scrollPM;
  wxBoxSizer* m_sizerScrollPM;

  ou::tf::DBOps m_db;

  ou::tf::iqfeed::InMemoryMktSymbolList m_listIQFeedSymbols;
  ou::tf::IQFeedSymbolListOps* m_pIQFeedSymbolListOps;
  ou::tf::IQFeedSymbolListOps::vExchanges_t m_vExchanges;
  ou::tf::IQFeedSymbolListOps::vClassifiers_t m_vClassifiers;
  void LookupDescription( const std::string& sSymbolName, std::string& sDescription );

  void ConstructEquityPosition0( const std::string& sName, pPortfolio_t, DelegateAddPosition_t);  // step 1
  void ConstructEquityPosition1( pInstrument_t& pInstrument ); // step 2

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void HandlePanelNewOrder( const ou::tf::PanelManualOrder::Order_t& order );
  void HandlePanelSymbolText( const std::string& sName );  // use IB to start, use IQFeed symbol file later on
  void HandlePanelFocusPropogate( unsigned int ix );

  void HandleIBContractDetails( const ou::tf::IBTWS::ContractDetails&, pInstrument_t& pInstrument );
  void HandleIBContractDetailsDone( void );

  void OnData1Connected( int );
//  void OnData2Connected( int ) {};
  void OnExecConnected( int );
  void OnData1Disconnected( int );
//  void OnData2Disconnteted( int ) {};
  void OnExecDisconnected( int );

  void HandleSaveButton( void );

  void HandlePopulateDatabase( void );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );

  void HandleGuiRefresh( wxTimerEvent& event );

  void HandleIBInstrument( EventIBInstrument& event );

  void HandlePortfolioLoad( const idPortfolio_t& idPortfolio );

//  void HandleMenuActionInitializeSymbolSet( void );
  void HandleMenuActionSaveSymbolSubset( void );
  void HandleMenuActionLoadSymbolSubset( void );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppStickShift)


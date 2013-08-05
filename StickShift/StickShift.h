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

// Started 2013/01/06
// This is manual based trading strategy, hence the name stick shift, for manual trading

#include <string>

#include <wx/timer.h>

#include <TFBitsNPieces/FrameWork01.h>

// may need to inherit and add more functionality to the class:
#include <TFTrading/DBOps.h>
#include <TFTrading/PortfolioManager.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/PanelManualOrder.h>
#include <TFVuTrading/ModelPortfolioPositionOrderExecution.h>
#include <TFVuTrading/PanelPortfolioPositionOrderExecution.h>
#include <TFVuTrading/ControllerPortfolioPositionOrderExecution.h>

class AppStickShift:
  public wxApp, public ou::tf::FrameWork01<AppStickShift> {
    friend ou::tf::FrameWork01<AppStickShift>;
public:
protected:
private:

  typedef ou::tf::Portfolio::pPortfolio_t pPortfolio_t;
  typedef ou::tf::Position::pPosition_t pPosition_t;

  typedef ou::tf::ModelPortfolioPositionOrderExecution MPPOE_t;
  typedef ou::tf::PanelPortfolioPositionOrderExecution PPPOE_t;
  typedef ou::tf::ControllerPortfolioPositionOrderExecution CPPOE_t;

  //typedef ou::tf::IBTWS::pInstrument_t pInstrument_t;
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;

  struct structManualOrder {
//    ou::tf::PanelManualOrder* pDialogManualOrder;
    ou::tf::IBTWS::ContractDetails details;
    pInstrument_t pInstrument;
  } m_IBInstrumentInfo;

  bool m_bData1Connected;
  bool m_bExecConnected;

  wxTimer m_timerGuiRefresh;

  double m_dblMaxPL;
  double m_dblMinPL;

  ou::tf::keytypes::idPortfolio_t m_idPortfolio;

  //std::string m_sDbPortfolioName;

  pPortfolio_t m_pPortfolio;
  pPosition_t m_pPosition;

  FrameMain* m_pFrameMain;
  FrameMain* m_pFPPOE;
//  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;
  ou::tf::PanelManualOrder* m_pPanelManualOrder;
  MPPOE_t* m_pMPPOE;
  PPPOE_t* m_pPPPOE;
  CPPOE_t* m_pCPPOE;

  ou::tf::DBOps m_db;

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

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppStickShift)


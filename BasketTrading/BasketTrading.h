/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// Started 2012/10/15
// 2013/01/06:
// Auto selects and trades a basket of equities, enters at market start, exits prior to end of regular trading hours
// Once hedging and option calculations are complete, can run auto hedging on basket and see if that works
// Current trading rules are too variable, big gains on one day when in directional market,
//   big losses on another when market moves sideways
// Data has been saved for both types of days, perhaps running GP routines on the data will
//  yield more consistently positive results.  Could try for better money management as well.
// 2019/03/24
//   using pivots, then checking opening against pivot to see which direction to trade based
//     upon probabilities. Use weeklies for symbol selection.

#include <string>
#include <memory>

#include <wx/timer.h>

#include <TFBitsNPieces/FrameWork01.h>

// may need to inherit and add more functionality to the class:
#include <TFTrading/DBOps.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include <TFBitsNPieces/IQFeedSymbolListOps.h>

#include "MasterPortfolio.h"
//#include "PanelBasketTradingMain.h"
#include "PanelPortfolioStats.h"

class AppBasketTrading:
  public wxApp, public ou::tf::FrameWork01<AppBasketTrading> {
    friend ou::tf::FrameWork01<AppBasketTrading>;
public:
protected:
private:

  typedef ou::tf::PortfolioManager::pPortfolio_t pPortfolio_t;

  ptime m_dtLatestEod;

  std::string m_sPortfolioStrategyAggregate;

  FrameMain* m_pFrameMain;
//  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;
//  PanelBasketTradingMain* m_pPanelBasketTradingMain;
  PanelPortfolioStats* m_pPanelPortfolioStats;

  ou::tf::DBOps m_db;

  std::unique_ptr<MasterPortfolio> m_pMasterPortfolio;
  pPortfolio_t m_pPortfolioMaster;
  pPortfolio_t m_pPortfolioCurrencyUSD;
  pPortfolio_t m_pPortfolioStrategyAggregate;

  bool m_bData1Connected;
  bool m_bExecConnected;

  wxTimer m_timerGuiRefresh;

  double m_dblMaxPL;
  double m_dblMinPL;

  ou::tf::iqfeed::InMemoryMktSymbolList m_listIQFeedSymbols;
  ou::tf::IQFeedSymbolListOps* m_pIQFeedSymbolListOps;
  ou::tf::IQFeedSymbolListOps::vExchanges_t m_vExchanges;
  ou::tf::IQFeedSymbolListOps::vClassifiers_t m_vClassifiers;

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void OnData1Connected( int );
//  void OnData2Connected( int ) {};
  void OnExecConnected( int );
  void OnData1Disconnected( int );
//  void OnData2Disconnteted( int ) {};
  void OnExecDisconnected( int );

  void HandleLoadButton( void );
  void HandleStartButton( void );
  void HandleExitPositionsButton( void );
  void HandleStopButton( void );
  void HandleSaveButton( void );

  void HandlePopulateDatabase( void );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );

  void HandleGuiRefresh( wxTimerEvent& event );

  void HandleMenuActionTestSelection( void );

  void HandleMenuActionSaveSymbolSubset( void );
  void HandleMenuActionLoadSymbolSubset( void );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppBasketTrading)


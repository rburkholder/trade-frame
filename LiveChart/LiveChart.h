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

// Started 2013/09/26

#include <wx/timer.h>

#include <OUCommon/Worker.h>

#include <TFBitsNPieces/FrameWork01.h>

#include <OUCharting/ChartMaster.h>

#include <TFTrading/DBOps.h>
#include <TFTrading/PortfolioManager.h>
#include <TFTrading/NoRiskInterestRateSeries.h>
#include <TFOptions/Bundle.h>

#include <TFIQFeed/LoadMktSymbols.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
//#include <TFVuTrading/PanelManualOrder.h>

#include <OUCommon/Worker.h>

#include "Chart.h"

class AppLiveChart:
  public wxApp, public ou::tf::FrameWork01<AppLiveChart> {
    friend ou::tf::FrameWork01<AppLiveChart>;
public:
protected:
private:

  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::Portfolio::pPortfolio_t pPortfolio_t;

  ou::action::Worker m_worker;

  FrameMain* m_pFrameMain;
//  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;
//  ou::tf::PanelManualOrder* m_pPanelManualOrder;

  wxWindow* m_winChart;
  bool m_bReadyToDrawChart;
  ou::ChartMaster m_chartMaster;
  ChartTest* m_pChart;
  bool m_bPaintingChart;

//  Strategy* m_pStrategy;

  ou::tf::DBOps m_db;

  pPortfolio_t m_pPortfolioMaster;
  pPortfolio_t m_pPortfolioCurrencyUSD;

  wxTimer m_timerGuiRefresh;
  ptime m_dtTopOfMinute;
  bool m_bIVCalcActive;

  boost::thread* m_pIVCalc;
  ou::tf::LiborFromIQFeed m_libor;

  ou::tf::iqfeed::InMemoryMktSymbolList m_listIQFeedSymbols;

  ou::tf::option::MultiExpiryBundle* m_pBundle;

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );
  void HandlePopulateDatabase( void );

  void HandleGuiRefresh( wxTimerEvent& event );
    
  //void AutoStartCollection( void );

  void OnData1Connected( int );
  void OnData2Connected( int );
  void OnExecConnected( int );
  void OnData1Disconnected( int );
  void OnData2Disconnected( int );
  void OnExecDisconnected( int );

  void CalcIV( ptime dt );

  void HandleMenuAction0ObtainNewIQFeedSymbolListRemote( void );
  void HandleMenuAction1ObtainNewIQFeedSymbolListLocal( void );
  void HandleMenuAction2LoadIQFeedSymbolList( void );
  void HandleMenuActionInitializeSymbolSet( void );
  void HandleMenuActionStartWatch( void );
  void HandleMenuActionStopWatch( void );
  void HandleMenuActionSaveValues( void );
  void HandleMenuActionEmitYieldCurve( void );
  void HandleMenuActionStartChart( void );

  void HandleObtainNewIQFeedSymbolListRemote( void );
  void HandleObtainNewIQFeedSymbolListLocal( void );
  void HandleLoadIQFeedSymbolList( void );
  void HandleSaveValues( void );

  void HandleDrawChart( const MemBlock& );
  void HandlePaint( wxPaintEvent& event );
  void HandleSize( wxSizeEvent& event );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppLiveChart)


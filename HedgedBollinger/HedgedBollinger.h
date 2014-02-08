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

// Started 2013/09/23

#include <boost/thread/thread_only.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

#include <wx/timer.h>
#include <wx/treectrl.h>

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

#include "EventUpdateOptionTree.h"
#include "EventDrawChart.h"
#include "Strategy1.h"

class AppHedgedBollinger:
  public wxApp, public ou::tf::FrameWork01<AppHedgedBollinger> {
    friend ou::tf::FrameWork01<AppHedgedBollinger>;
public:
protected:
private:

  enum EProviderUsage { EProviderUsageUnknown, EProviderUsageRead, EProviderUsageReadTrade };

  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::Portfolio::pPortfolio_t pPortfolio_t;

  std::string m_sNameUnderlying;
  std::string m_sNameUnderlyingIQFeed;
  std::string m_sNameOptionUnderlying;

  //boost::gregorian::date m_dateFrontMonthFuture;
  //boost::gregorian::date m_dateSecondMonthFuture;
  boost::gregorian::date m_dateFrontMonthOption;
  boost::gregorian::date m_dateSecondMonthOption;

  ou::action::Worker m_worker;

  FrameMain* m_pFrameMain;
//  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;
//  ou::tf::PanelManualOrder* m_pPanelManualOrder;

  wxWindow* m_winChart;
  bool m_bReadyToDrawChart;
  bool m_bInDrawChart;
  ou::ChartMaster m_chart;
  wxBitmap* m_pChartBitmap;
  boost::posix_time::time_duration m_tdViewPortWidth;

  Strategy* m_pStrategy;

  ou::tf::DBOps m_db;

  pPortfolio_t m_pPortfolioMaster;
  pPortfolio_t m_pPortfolioCurrencyUSD;

  pPortfolio_t m_pPortfolioGC;
  //pPortfolio_t m_pPortfolioLongs;
  //pPortfolio_t m_pPortfolioShorts;

  wxTimer m_timerGuiRefresh;
  ptime m_dtTopOfMinute;
  unsigned int m_cntIVCalc;  // calc IV every nth gui refresh
  static unsigned int m_nthIVCalc;
  volatile bool m_bIVCalcActive;

  boost::mutex m_mutexThreadDrawChart;
  boost::condition_variable m_cvThreadDrawChart;
  bool m_bThreadDrawChartActive;
  boost::thread* m_pThreadDrawChart;

  wxTreeCtrl* m_ptreeChartables;  // http://docs.wxwidgets.org/trunk/classwx_tree_ctrl.html

  boost::thread* m_pIVCalc;
  ou::tf::LiborFromIQFeed m_libor;

  ou::tf::iqfeed::InMemoryMktSymbolList m_listIQFeedSymbols;

  EProviderUsage m_eProviderUsage;
  ou::tf::option::MultiExpiryBundle* m_pBundle;

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );
  void HandlePopulateDatabase( void );

  void HandleGuiRefresh( wxTimerEvent& event );

  void UpdateTree( ou::tf::option::Option*, bool bWatching );
    
  //void AutoStartCollection( void );

  void OnData1Connected( int );
  void OnData2Connected( int );
  void OnExecConnected( int );
  void OnData1Disconnected( int );
  void OnData2Disconnected( int );
  void OnExecDisconnected( int );

  void CalcIV( ptime dt );

  void StartDrawChart( void );
  void ThreadDrawChart1( void );  // thread starts here
  void ThreadDrawChart2( const MemBlock& m );  // a callback here to perform bitmap

  void HandleMenuAction0ObtainNewIQFeedSymbolListRemote( void );
  void HandleMenuAction1ObtainNewIQFeedSymbolListLocal( void );
  void HandleMenuAction2LoadIQFeedSymbolList( void );
  void HandleMenuActionInitializeSymbolSet( void );
  void HandleMenuActionSaveSymbolSubset( void );
  void HandleMenuActionLoadSymbolSubset( void );
  void HandleMenuActionStartWatch( void );
  void HandleMenuActionStopWatch( void );
  void HandleMenuActionSaveValues( void );
  void HandleMenuActionEmitYieldCurve( void );
  void HandleMenuActionStartChart( void );
  void HandleMenuActionStopChart( void );
  void HandleMenuActionEmitStrategyValues( void );

  void HandleIBUnderlyingContractDetails( const ou::tf::IBTWS::ContractDetails&, pInstrument_t& );
  void HandleIBUnderlyingContractDetailsDone( void );
  void FinishStrategyInitialization( pInstrument_t pInstrumentUnderlying );

  void HandleObtainNewIQFeedSymbolListRemote( void );
  void HandleObtainNewIQFeedSymbolListLocal( void );
  void HandleLoadIQFeedSymbolList( void );
  void HandleSaveValues( void );

//  void HandleDrawChart( const MemBlock& );
  void HandlePaint( wxPaintEvent& event );
  void HandleSize( wxSizeEvent& event );
  void HandleMouse( wxMouseEvent& event );

  void HandleStrikeWatchOn( ou::tf::option::Strike& );
  void HandleStrikeWatchOff( ou::tf::option::Strike& );

  void HandleGuiUpdateOptionTree( EventUpdateOptionTree& event );
  void HandleGuiDrawChart( EventDrawChart& event );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppHedgedBollinger)


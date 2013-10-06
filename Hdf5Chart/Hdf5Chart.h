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
#include <wx/treectrl.h>

#include <OUCommon/Worker.h>

#include <OUCharting/ChartMaster.h>

#include <TFBitsNPieces/FrameWork01.h>

#include <TFTrading/DBOps.h>
#include <TFTrading/PortfolioManager.h>
#include <TFTrading/NoRiskInterestRateSeries.h>

#include <TFOptions/Bundle.h>

#include <TFIQFeed/LoadMktSymbols.h>

#include <TFHDF5TimeSeries/HDF5DataManager.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
//#include <TFVuTrading/PanelManualOrder.h>

#include "ChartControl.h"

class AppHdf5Chart:
  public wxApp, public ou::tf::FrameWork01<AppHdf5Chart> {
    friend ou::tf::FrameWork01<AppHdf5Chart>;
public:
protected:
private:

  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::Portfolio::pPortfolio_t pPortfolio_t;

  ou::action::Worker m_worker;

  ou::tf::HDF5DataManager* m_pdm;

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

  class CustomItemData: public wxTreeItemData { // wxTreeCtrl node/leaf info
  public:
    enum enumNodeType { Root, Group, Object } m_eNodeType;
    enum enumDatumType { Quotes, Trades, Bars, NoDatum } m_eDatumType;
    CustomItemData( enumNodeType eNodeType, enumDatumType eDatumType )
      : m_eNodeType( eNodeType ), m_eDatumType( eDatumType ) {};
  };
  wxTreeCtrl* m_pHdf5Root;  // http://docs.wxwidgets.org/trunk/classwx_tree_ctrl.html
  std::string m_sCurrentPath;  // used while traversing and building tree
  wxTreeItemId m_curTreeItem; // used while traversing and building tree
  CustomItemData::enumDatumType m_eLatestDatumType;  // need this until all timeseries have a signature attribute associated

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
  void HandleMenuActionLoadTree( void );

  void HandleObtainNewIQFeedSymbolListRemote( void );
  void HandleObtainNewIQFeedSymbolListLocal( void );
  void HandleLoadIQFeedSymbolList( void );
  void HandleSaveValues( void );

  void HandleLoadTreeHdf5Group( const std::string& s1, const std::string& s2 );
  void HandleLoadTreeHdf5Object( const std::string& s1, const std::string& s2 );

  void HandleDrawChart( const MemBlock& );
  void HandlePaint( wxPaintEvent& event );
  void HandleSize( wxSizeEvent& event );

  void HandleBuildTreePathParts( const std::string& sPath );

  void HandleTreeEventItemActivated( wxTreeEvent& event );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppHdf5Chart)


/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    AppAutoTrade.h
 * Author:  raymond@burkholder.net
 * Project: rdaf/at
 * Created: March 7, 2022 14:35
 */

#pragma once

#include <string>
#include <memory>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>
#include <wx/timer.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTrading/DBWrapper.h>

#include <TFBitsNPieces/FrameWork01.h>

#include "ConfigParser.hpp"

class TRint;
class TFile;

class Strategy;
class FrameMain;

class wxTreeCtrl;
class wxTreeEvent;
class wxTimerEvent;
class wxSplitterWindow;

namespace ou {
namespace tf {
  class PanelLogging;
  class WinChartView;
  class BuildInstrument;
}
}

class AppAutoTrade:
  public wxApp,
  public ou::tf::FrameWork01<AppAutoTrade>
{
  friend ou::tf::FrameWork01<AppAutoTrade>;
  friend class boost::serialization::access;
public:
protected:
private:

  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;
  ou::tf::WinChartView* m_pWinChartView;

  wxSplitterWindow* m_splitterData;
  wxTreeCtrl* m_treeSymbols;

  wxTimer m_timerOneSecond;

  std::string m_sSymbol;
  std::string m_sTSDataStreamStarted;
  int m_nTSDataStreamSequence;

  ou::tf::config::choices_t m_choices;

  std::unique_ptr<ou::tf::BuildInstrument> m_pBuildInstrument;

  std::unique_ptr<ou::tf::db> m_pdb;

  ou::ChartEntryIndicator m_ceUnRealized;
  ou::ChartEntryIndicator m_ceRealized;
  ou::ChartEntryIndicator m_ceCommissionsPaid;
  ou::ChartEntryIndicator m_ceTotal;

  ou::ChartDataView m_dvChart; // the data

  pPortfolio_t m_pPortfolioUSD;

  using pStrategy_t = std::unique_ptr<Strategy>;
  using mapStrategy_t = std::map<std::string,pStrategy_t>;
  mapStrategy_t m_mapStrategy;

  std::unique_ptr<TRint> m_prdafApp;
  std::shared_ptr<TFile> m_pFile;

  void StartRdaf( const std::string& sFilePrefix );

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void OnData1Connected( int );
  void OnData2Connected( int );
  void OnExecConnected( int );
  void OnData1Disconnected( int );
  void OnData2Disconnected( int );
  void OnExecDisconnected( int );

  void HandleOneSecondTimer( wxTimerEvent& event );

  void HandleMenuActionCloseAndDone();
  void HandleMenuActionSaveValues();

  void HandleTreeEventItemMenu( wxTreeEvent& );
  void HandleTreeEventItemChanged( wxTreeEvent& );

  void ConstructIBInstrument(  const std::string& sNamePortfolio, const std::string& sSymbol );
  void ConstructSimInstrument( const std::string& sNamePortfolio, const std::string& sSymbol );

  void LoadPortfolio( const std::string& sName );
  void ConfirmProviders();

  void HandleMenuActionSimStart();
  void HandleMenuActionSimStop();
  void HandleMenuActionSimEmitStats();

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppAutoTrade, 1)

DECLARE_APP(AppAutoTrade)


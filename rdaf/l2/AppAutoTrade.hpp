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
 * File:    AppAutoTrade.hpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: December 9, 2022  16:18:19
 */

#pragma once

#ifndef RDAF
#define RDAF false
#endif

#include <string>
#include <memory>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>
#include <wx/timer.h>
#include <wx/splitter.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <Telegram/Bot.hpp>

#include <TFTrading/DBWrapper.h>

#include <TFIQFeed/Provider.h>
#include <TFIQFeed/Level2/Symbols.hpp>

#include <TFInteractiveBrokers/IBTWS.h>

#include <TFSimulation/SimulationProvider.h>

#include <TFBitsNPieces/FrameWork02.hpp>

#include "ConfigParser.hpp"

#if RDAF
class TRint;
class TFile;
#endif

class Strategy;
class FrameMain;

class wxTreeCtrl;
class wxTreeEvent;
class wxTimerEvent;

namespace ou {
namespace tf {
  class TreeItem;
  class PanelLogging;
  class WinChartView;
  class BuildInstrument;
namespace v2 {
  class PanelProviderControl;
}
} // namespace tf
} // namespace ou

class AppAutoTrade:
  public wxApp,
  public ou::tf::FrameWork02<AppAutoTrade>
{
  friend ou::tf::FrameWork02<AppAutoTrade>;
  friend class boost::serialization::access;
public:
protected:
private:

  using TreeItem = ou::tf::TreeItem;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;
  using pProviderSim_t = ou::tf::SimulationProvider::pProvider_t;
  using pProviderIB_t = ou::tf::ib::TWS::pProvider_t;
  using pProviderIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;

  using fInstrumentConstructed_t = std::function<void(const std::string&)>;

  ou::tf::config::choices_t m_choices;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;
  ou::tf::WinChartView* m_pWinChartView;
  ou::tf::v2::PanelProviderControl* m_pPanelProviderControl;

  wxSplitterWindow* m_splitterData;

  TreeItem* m_pTreeItemRoot; // root of custom tree items
  TreeItem* m_pTreeItemPortfolio;
  wxTreeCtrl* m_treeSymbols;

  wxTimer m_timerOneSecond;

  pProvider_t       m_data;
  pProvider_t       m_exec;

  pProviderSim_t    m_sim;    // simulation - [ data,execution ]
  pProviderIB_t     m_tws;    // live - [ execution ]
  pProviderIQFeed_t m_iqf;    // live - [ data ], simulation - [ execution ]

  bool m_bL2Connected;

  std::unique_ptr<ou::tf::iqfeed::l2::Symbols> m_pL2Symbols;
  std::string m_sSimulationDateTime;  // used for l2 output streaming file name

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

  ou::Delegate<int> m_OnSimulationComplete;

#if RDAF
  std::unique_ptr<TRint> m_prdafApp;
  std::shared_ptr<TFile> m_pFile; // primary timeseries
  std::shared_ptr<TFile> m_pFileUtility;  // scratch pad use

  using vRdafFiles_t = std::vector<std::string>;
  vRdafFiles_t m_vRdafFiles;
#endif

  void StartRdaf( const std::string& sFilePrefix );

  std::unique_ptr<telegram::Bot> m_telegram_bot;
  void Telegram_GetMe();
  void Telegram_SendMessage();

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void HandleOneSecondTimer( wxTimerEvent& event );

  void HandleMenuActionCloseAndDone();
  void HandleMenuActionSaveValues();

  //void RecreateUtilityFile();
  //void UpdateUtilityFile();
  //void HandleMenuActionUtilityFlush();
  //void HandleMenuActionUtilitySave();
  //void HandleMenuActionUtilityClear();

  void ConstructInstrument_Live(
    const std::string& sNamePortfolio
  , const std::string& sSymbol
  , fInstrumentConstructed_t&&
    );
  void ConstructInstrument_Sim( const std::string& sNamePortfolio, const std::string& sSymbol );

  void LoadPortfolio( const std::string& sName );
  void ConfirmProviders();

  void HandleSimConnected( int );

  void HandleMenuActionSimStart();
  void HandleMenuActionSimStop();
  void HandleMenuActionSimEmitStats();

  void HandleSimComplete();

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    ar & m_splitterData->GetSashPosition();
    ar & ( ( m_telegram_bot ) ? m_telegram_bot->GetChatId() : 0 );
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    if ( 2 <= version ) {
      int x;
      ar & x;
      m_splitterData->SetSashPosition( x );
    }
    if ( 3 <= version ) {
      uint64_t id;
      ar & id;
      if ( m_telegram_bot ) {
        m_telegram_bot->SetChatId( id );
      }
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppAutoTrade, 3)

DECLARE_APP(AppAutoTrade)


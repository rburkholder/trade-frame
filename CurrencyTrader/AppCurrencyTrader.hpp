/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    AppCurrencyTrader.hpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: March 09, 2024 19:58:27
 */

#pragma once

#include <thread>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include <wx/app.h>
#include <wx/timer.h>
#include <wx/splitter.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTrading/DBWrapper.h>

#include <TFIQFeed/Provider.h>
#include <TFInteractiveBrokers/IBTWS.h>
#include <TFSimulation/SimulationProvider.hpp>

#include <TFBitsNPieces/FrameWork02.hpp>

#include "Common.hpp"
#include "Config.hpp"
#include "PanelCurrencyStats.hpp"

namespace asio = boost::asio; // from <boost/asio/context.hpp>

class Strategy;
class FrameMain;

class wxBoxSizer;
class wxTreeCtrl;
class wxTreeEvent;
class wxSplitterWindow;

namespace ou {
namespace tf {
  class TreeItem;
  class PanelLogging;
  class WinChartView;
  //class BuildInstrument;
namespace v2 {
  class PanelProviderControl;
}
} // namespace tf
} // namespace ou

class AppCurrencyTrader:
  public wxApp,
  public ou::tf::FrameWork02<AppCurrencyTrader>
{
  friend ou::tf::FrameWork02<AppCurrencyTrader>;
  friend class boost::serialization::access;
public:
protected:
private:

  using TreeItem = ou::tf::TreeItem;

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;
  using pProviderSim_t = ou::tf::SimulationProvider::pProvider_t;
  using pProviderIB_t = ou::tf::ib::TWS::pProvider_t;
  using pProviderIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;

  using fInstrumentConstructed_t = std::function<void(const std::string&)>;

  config::Choices m_choices;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;
  ou::tf::WinChartView* m_pWinChartView;
  ou::tf::v2::PanelProviderControl* m_pPanelProviderControl;
  PanelCurrencyStats* m_pPanelCurrencyStats;

  wxSplitterWindow* m_splitterData;

  TreeItem* m_pTreeItemRoot; // root of custom tree items
  TreeItem* m_pTreeItemPortfolio;
  wxTreeCtrl* m_treeSymbols;

  wxTimer m_timerOneSecond;

  bool m_bProvidersConfirmed;

  pProvider_t       m_data;
  pProvider_t       m_exec;

  pProviderIB_t     m_tws;    // live - [ execution ]
  pProviderSim_t    m_sim;    // simulation - [ data,execution ]
  pProviderIQFeed_t m_iqf;    // live - [ data ], simulation - [ execution ]

  std::string m_sTSDataStreamStarted;
  int m_nTSDataStreamSequence;

  //std::unique_ptr<ou::tf::BuildInstrument> m_pBuildInstrument;

  std::unique_ptr<ou::tf::db> m_pdb;

  ou::ChartEntryIndicator m_ceUnRealized;
  ou::ChartEntryIndicator m_ceRealized;
  ou::ChartEntryIndicator m_ceCommissionsPaid;
  ou::ChartEntryIndicator m_ceTotal;

  ou::tf::Currency::ECurrency m_currencyBase;

  struct Currency {
    double amount;  // TODO: convert to decimal?
    PanelCurrencyStats::fUpdateCurrency_t fUpdateCurrency;

    Currency(): amount {}, fUpdateCurrency( nullptr ) {}

    Currency( double amount_ )
    : amount( amount_ ), fUpdateCurrency( nullptr )
    {}

    Currency( Currency&& rhs )
    : amount( rhs.amount )
    , fUpdateCurrency( std::move( rhs.fUpdateCurrency ) )
    {}
  };
  using mapCurrency_t = std::map<ou::tf::Currency::ECurrency, Currency>;
  mapCurrency_t m_mapCurrency;
  double m_dblCommissionTotal;

  ou::ChartDataView m_dvPortfolio; // the data

  pPortfolio_t m_pPortfolioUSD; // base currency
  //pPortfolio_t m_pPortfolioForex;

  using pStrategy_t = std::unique_ptr<Strategy>;
  struct Pair {

    EBase eBase;
    ou::tf::Currency::ECurrency currencyNonBase;
    pStrategy_t pStrategy;
    PanelCurrencyStats::fUpdatePair_t fUpdatePair;
    std::unique_ptr<boost::asio::system_timer> ptimerSoftwareReset;
    //bool bAllowTimer;

    Pair( pStrategy_t pStrategy_ )
    : pStrategy( std::move( pStrategy_ ) )
    , fUpdatePair( nullptr )
    , eBase( EBase::Unknown )
    //, bAllowTimer( false )
    {}

    Pair( Pair&& rhs )
    : pStrategy( std::move( rhs.pStrategy ) )
    , fUpdatePair( std::move( rhs.fUpdatePair ) )
    , eBase( rhs.eBase )
    //, bAllowTimer( rhs.bAllowTimer )
    {}
  };

  // TODO: use a std::pair for the key?
  using mapPair_t = std::map<std::string,Pair>;
  mapPair_t m_mapPair;

  boost::gregorian::date           m_startDateUTC;
  boost::posix_time::time_duration m_startTimeUTC;

  std::thread m_thread;
  boost::asio::io_context m_io;

  using work_guard_t = asio::executor_work_guard<boost::asio::io_context::executor_type>;
  using pWorkGuard_t = std::unique_ptr<work_guard_t>;
  pWorkGuard_t m_pWorkGuard;

  enum class ESoftwareReset { quiescent, looking, encountered };
  ESoftwareReset m_stateSoftwareReset;

  ou::Delegate<int> m_OnSimulationComplete;

  bool BuildProviders_Live( wxBoxSizer* );
  bool BuildProviders_Sim();

  void SetStreamStartDateTime();

  void PopulatePortfolioChart();
  void PopulateTreeRoot();
  void ConstructStrategyList();
  void LoadPortfolioCurrency(); // base currency portfolio

  void ConfirmProviders();
  pInstrument_t ConstructInstrumentBase( const std::string& sName, const std::string& sExchange );
  void EnhanceInstrument( pInstrument_t );

  pPosition_t ConstructPosition( pPortfolio_t pPortfolio, const std::string& sPositionPrefix, pWatch_t );
  void PopulateCurrency( ou::tf::Currency::ECurrency );
  void PopulateStrategy( pInstrument_t );

  void HandleSimConnected( int );
  void HandleMenuActionSimStart();
  void HandleMenuActionSimStop();
  void HandleMenuActionSimEmitStats();
  void HandleSimComplete();

  void HandleTimer( wxTimerEvent& event );
  void UpdatePanelCurrencyStats();

  void HandleMenuActionCloseAndDone();
  void HandleMenuActionSaveValues();
  void HandleMenuActionEmitSwingTrack();
  void HandleMenuActionEmitCubiCoef();

  void SaveState();
  void LoadState();

  virtual bool OnInit();
  void OnClose( wxCloseEvent& event );
  virtual int OnExit();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    ar & m_splitterData->GetSashPosition();
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
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppCurrencyTrader, 1)

DECLARE_APP(AppCurrencyTrader)

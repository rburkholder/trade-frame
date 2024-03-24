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

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>
#include <wx/timer.h>
#include <wx/splitter.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTrading/DBWrapper.h>

#include <TFIQFeed/Provider.h>
#include <TFInteractiveBrokers/IBTWS.h>
#include <TFSimulation/SimulationProvider.h>

#include <TFBitsNPieces/FrameWork02.hpp>

#include "Config.hpp"

class Strategy;
class FrameMain;

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
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

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

  wxSplitterWindow* m_splitterData;

  TreeItem* m_pTreeItemRoot; // root of custom tree items
  TreeItem* m_pTreeItemPortfolio;
  wxTreeCtrl* m_treeSymbols;

  wxTimer m_timerOneSecond;

  bool m_bProvidersConfirmed;

  pProvider_t       m_data;
  pProvider_t       m_exec;

  pProviderIB_t     m_tws;    // live - [ execution ]
  //pProviderSim_t    m_sim;    // simulation - [ data,execution ]
  pProviderIQFeed_t m_iqf;    // live - [ data ], simulation - [ execution ]

  //std::string m_sSimulationDateTime;  // used for l2 output streaming file name

  std::string m_sTSDataStreamStarted;
  int m_nTSDataStreamSequence;

  //std::unique_ptr<ou::tf::BuildInstrument> m_pBuildInstrument;

  std::unique_ptr<ou::tf::db> m_pdb;

  ou::ChartEntryIndicator m_ceUnRealized;
  ou::ChartEntryIndicator m_ceRealized;
  ou::ChartEntryIndicator m_ceCommissionsPaid;
  ou::ChartEntryIndicator m_ceTotal;

  ou::ChartDataView m_dvChart; // the data

  pPortfolio_t m_pPortfolioUSD; // base currency
  //pPortfolio_t m_pPortfolioForex;

  using pStrategy_t = std::unique_ptr<Strategy>;
  using mapStrategy_t = std::map<std::string,pStrategy_t>;
  mapStrategy_t m_mapStrategy;

  //ou::Delegate<int> m_OnSimulationComplete;

  void HandleOneSecondTimer( wxTimerEvent& event );

  void HandleMenuActionCloseAndDone();
  void HandleMenuActionSaveValues();

  void LoadPortfolioCurrency(); // base currency portfolio
  //void LoadPortfolioForex();    // strategy positions
  void ConfirmProviders();
  void BuildStrategy( pInstrument_t );

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

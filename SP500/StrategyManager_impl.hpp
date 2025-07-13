/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    StrategyManager_impl.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: June 28, 2025 21:20:51
 */

#pragma once

// need to keep Torch stuff separate from wxWidgets stuff.
// source includes negatively interact with each other.

#include <unordered_map>

#include <OUCommon/KeyWordMatch.h>

#include <OUCharting/ChartDataView.h>

#include <TFVuTrading/WinChartView.h>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

#include <TFIQFeed/Provider.h>
#include <TFInteractiveBrokers/IBTWS.h>
#include <TFSimulation/SimulationProvider.hpp>

#include "Model.hpp"
#include "Strategy.hpp"

namespace config {
  class Choices;
} // namespace config

class StrategyManager_impl {
public:

  using fTask_t = std::function<void()>;
  using fQueueTask_t = std::function<void( fTask_t&& )>;
  using fSetChartDataView_t = std::function<void( ou::tf::WinChartView::EState, ou::ChartDataView* )>;
  using fDone_t = std::function<void()>;

  StrategyManager_impl(
    const config::Choices&
  , fQueueTask_t&&
  , fSetChartDataView_t&&
  , fDone_t&&
  );
  ~StrategyManager_impl();

protected:
private:

  const config::Choices& m_choices;

  ou::ChartDataView m_cdv;
  ou::ChartDataView m_cdv_build;
  ou::ChartDataView m_cdv_predict; // might reuse the first if Clear() works properly

  fSetChartDataView_t m_fSetChartDataView;

  fQueueTask_t m_fQueueTask;
  fDone_t m_fDone;

  boost::gregorian::date           m_startDateUTC;
  boost::posix_time::time_duration m_startTimeUTC;

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;
  using pProviderIB_t = ou::tf::ib::TWS::pProvider_t;
  using pProviderIQF_t = ou::tf::iqfeed::Provider::pProvider_t;
  using pProviderSim_t = ou::tf::SimulationProvider::pProvider_t;

  pProvider_t m_data;
  pProvider_t m_exec;

  pProviderIQF_t m_iqf; // live - [ data ], simulation - [ execution ]
  pProviderIB_t  m_tws; // live - [ execution ]
  pProviderSim_t m_sim; // may not need this as iqf does sim

  std::string m_sSimulatorGroupDirectory;

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using mapHdf5Instrument_t = std::unordered_map<std::string,pInstrument_t>;
  mapHdf5Instrument_t m_mapHdf5Instrument;

  /*
  symbol_name_l1=II6A.Z  #S&P 500 ISSUES ADVANCES DTN - sum to 500
  symbol_name_l1=II6D.Z  #S&P 500 ISSUES DECLINES DTN - sum to 500
  symbol_name_l1=JT6T.Z  #S&P 500 TICKS NET              - useful
  symbol_name_l1=LI6N.Z  #EXCHG ONLY S&P 500 TICKS NET   - useful
  symbol_name_l1=TR6T.Z  #S&P 500 TICKS RATIO  - useful
  */

  enum class ESymbol { SPY, II6A, II6D, JT6T, LI6N, TR6T, UKNWN } m_eSymbol;
  ou::KeyWordMatch<ESymbol>* m_pkwmSymbol;

  struct SymbolInfo {
    const std::string sName;
    size_t ixChart;
    ou::tf::Trades trades;
    ou::tf::Quotes quotes;
    ou::ChartEntryIndicator indicatorAsk;
    ou::ChartEntryIndicator indicatorTrade;
    ou::ChartEntryIndicator indicatorBid;
    boost::posix_time::time_duration tdDelay;

    SymbolInfo( const std::string& sName_, size_t ixChart_ )
    : sName( sName_ )
    , ixChart( ixChart_ )
    , tdDelay( boost::posix_time::time_duration( 0, 0, 0 ) )
    {}

    SymbolInfo( const std::string& sName_, size_t ixChart_
    , boost::posix_time::time_duration tdDelay_
    )
    : sName( sName_ )
    , ixChart( ixChart_ )
    , tdDelay( tdDelay_ )
    {}
  };
  using mapSymbolInfo_t = std::unordered_map<ESymbol,SymbolInfo>;
  mapSymbolInfo_t m_mapSymbolInfo;

  using pStrategy_t = std::unique_ptr<Strategy>;
  pStrategy_t m_pStrategy;

  Model m_model;

  using fHandleLoadTreeHdf5Object_t = std::function<void(const std::string&, const std::string&)>;
  void IterateHDF5( ou::tf::HDF5DataManager&, fHandleLoadTreeHdf5Object_t&& );

  void InitStructures(
    ESymbol, const std::string& sName, size_t ixChart,
    boost::posix_time::time_duration = boost::posix_time::time_duration( 0, 0, 0 )
  );
  void LoadPanelFinancialChart();

  void RunSimulation();

  bool BuildProviders_Sim();
  bool ValidateSimFile( const std::string& );
  void HandleSimConnected( int );
  void HandleSimComplete_build();
  void CleanUp_build();
  void HandleSimComplete_predict();
  void CleanUp_predict();

  void HandleLoadTreeHdf5Object_View( ou::tf::HDF5DataManager&, const std::string&, const std::string& );
  void HandleLoadTreeHdf5Object_Sim(   ou::tf::HDF5DataManager&, const std::string&, const std::string& );

  void HandleLoadTreeHdf5Group( const std::string&, const std::string& );

  void RunStrategy( boost::gregorian::date, ou::ChartDataView&, Strategy::fForward_t&& );
  void RunStrategy_build();
  void RunStrategy_predict();

};

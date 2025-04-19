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
 * File:    AppSP500.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: March 30, 2025 13:49:15
 */

#pragma once

#include <unordered_map>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>

#include <OUCommon/KeyWordMatch.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFIQFeed/Provider.h>
#include <TFInteractiveBrokers/IBTWS.h>
#include <TFSimulation/SimulationProvider.h>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

#include <TFTrading/DBWrapper.h>

#include <TFVuTrading/TreeItem.hpp>
#include <TFVuTrading/WinChartView.h>

#include <TFBitsNPieces/FrameWork02.hpp>

#include "Config.hpp"
#include "Strategy.hpp"

class FrameMain;

class AppSP500:
  public wxApp
, public ou::tf::FrameWork02<AppSP500>
{
  friend class boost::serialization::access;
  friend ou::tf::FrameWork02<AppSP500>;
public:
protected:
private:

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;
  using pProviderIB_t = ou::tf::ib::TWS::pProvider_t;
  using pProviderIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;
  using pProviderSimulator_t = ou::tf::SimulationProvider::pProvider_t;

  config::Choices m_choices;

  FrameMain* m_pFrameMain;
  //ou::tf::PanelFinancialChart* m_pPanelFinancialChart; // refactor/inherit once functionality is known

  std::unique_ptr<ou::tf::db> m_pdb;

  using pHDF5DataManager_t = std::unique_ptr<ou::tf::HDF5DataManager>;
  pHDF5DataManager_t m_pdm;

  ou::tf::TreeItem* m_ptiRoot;

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

  ou::ChartDataView m_cdv;
  ou::tf::WinChartView* m_pwcv; // handles drawing the chart

  //std::string m_sSimulatorGroupDirectory;

  pProvider_t m_data;
  pProvider_t m_exec;

  pProviderIQFeed_t    m_iqf; // live - [ data ], simulation - [ execution ]
  pProviderIB_t        m_tws; // live - [ execution ]
  pProviderSimulator_t m_sim; // may not need this as iqf does sim

  using pStrategy_t = std::unique_ptr<Strategy>;
  pStrategy_t m_pStrategy;

  void OnFrameMainAutoMove( wxMoveEvent& );

  void InitStructures(
    ESymbol, const std::string& sName, size_t ixChart,
    boost::posix_time::time_duration = boost::posix_time::time_duration( 0, 0, 0 )
  );

  void LoadPanelFinancialChart();

  void IterateObjects();

  void HandleLoadTreeHdf5Group( const std::string& s1, const std::string& s2 );
  void HandleLoadTreeHdf5Object( const std::string& s1, const std::string& s2 );

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    //ar & *m_pPanelFinancialChart;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    if ( 2 <= version ) {
      //ar & *m_pPanelFinancialChart;
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppSP500, 3)

DECLARE_APP(AppSP500)

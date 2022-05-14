/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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

/*
 * File:    AppDoM.h
 * Author:  raymond@burkholder.net
 * Project: App Depth of Market
 * Created on October 12, 2021, 23:04
 */

#include <atomic>
#include <memory>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>

#include <TFTrading/Watch.h>

#include <TFIndicators/TSEMA.h>
#include <TFIndicators/Pivots.h>
#include <TFIndicators/TSSWStochastic.h>

#include <TFIQFeed/HistoryRequest.h>
#include <TFIQFeed/Level2/Symbols.hpp>
#include <TFIQFeed/Level2/FeatureSet.hpp>

#include <TFBitsNPieces/FrameWork01.h>

#include <TFVuTrading/PanelLogging.h>

#include "Config.h"
#include "PanelStatistics.hpp"

namespace ou {
namespace tf {
  class FrameControls;
namespace l2 {
  class PanelTrade;
  class PanelSideBySide;
} // namespace l2
} // namespace tf
} // namespace ou

class FrameMain;

class AppDoM:
  public wxApp,
  public ou::tf::FrameWork01<AppDoM>
{
  friend ou::tf::FrameWork01<AppDoM>;
  friend class boost::serialization::access;
public:
protected:
private:

  config::Options m_config;

  FrameMain* m_pFrameMain;
  ou::tf::FrameControls* m_pFrameControls;

  ou::tf::PanelLogging* m_pPanelLogging;
  ou::tf::l2::PanelTrade* m_pPanelTrade;
  ou::tf::l2::PanelSideBySide* m_pPanelSideBySide;

  int m_cntLoops;
  PanelStatistics::values_t m_valuesStatistics;
  PanelStatistics* m_pPanelStatistics;

  //wxMenu* m_pMenuLoadDays;

  ou::tf::iqfeed::HistoryRequest::pHistoryRequest_t m_pHistoryRequest;
  std::unique_ptr<ou::tf::iqfeed::l2::Symbols> m_pDispatch;

  ou::tf::iqfeed::l2::OrderBased m_OrderBased; // direct access
  ou::tf::iqfeed::l2::FeatureSet m_FeatureSet;

  ou::tf::Watch::pWatch_t m_pWatch;

  ou::tf::Bars m_barsHistory;
  ou::tf::PivotSet m_setPivots;

  struct Stochastic {

    using pTSSWStochastic_t = std::unique_ptr<ou::tf::TSSWStochastic>;
    using fStochastic_t = std::function<void(ptime,double,double,double)>;
    // datetime, percentage, price min, price max

    pTSSWStochastic_t m_pIndicatorStochastic;

    Stochastic(
      ou::tf::Quotes& quotes, int nPeriods, time_duration td
    , fStochastic_t&& fStochastic
    ) {
      m_pIndicatorStochastic = std::make_unique<ou::tf::TSSWStochastic>(
        quotes, nPeriods, td, std::move( fStochastic )
      );
    }

    Stochastic( const Stochastic& ) = delete;
    Stochastic( Stochastic&& rhs ) = delete;

    ~Stochastic() {
      m_pIndicatorStochastic.reset();
    }

  };

  using pStochastic_t = std::unique_ptr<Stochastic>;
  using vStochastic_t = std::vector<pStochastic_t>;
  vStochastic_t m_vStochastic;

  struct MA {

    std::string sName;
    ou::tf::hf::TSEMA<ou::tf::Quote> m_ema;

    MA( ou::tf::Quotes& quotes, size_t nPeriods, time_duration tdPeriod, const std::string& sName_ )
    : sName( sName_ ), m_ema( quotes, nPeriods, tdPeriod ) {}

    MA( MA&& rhs )
    : m_ema( std::move( rhs.m_ema ) ), sName( std::move( rhs.sName ) )
    {}

    double Latest() const { return m_ema.GetEMA(); }
  };

  using vMA_t = std::vector<MA>;
  vMA_t m_vMA;

  double m_dblLastAsk;
  double m_dblLastBid;

  std::atomic_uint32_t m_nMarketOrdersAsk;
  std::atomic_uint32_t m_nMarketOrdersBid;

  void EmitMarketMakerMaps();

  virtual bool OnInit();
  void OnClose( wxCloseEvent& event );
  virtual int OnExit();

  void OnData1Connected( int );
  void OnData1Disconnecting( int );
  void OnData1Disconnected( int );

  void StartDepthByMM();
  void StartDepthByOrder();

  void OnFundamentals( const ou::tf::Watch::Fundamentals& );
  void OnQuote( const ou::tf::Quote& );
  void OnTrade( const ou::tf::Trade& );

  void LoadDailyHistory();

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    ar & *m_pFrameControls;
}

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    if ( 2 <= version ) {
      ar & *m_pFrameControls;
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppDoM, 2)

DECLARE_APP(AppDoM)

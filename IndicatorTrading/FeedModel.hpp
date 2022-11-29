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
 * File:    FeedModel.hpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: 2022/11/21 13:42:06
 */

#pragma once

// provides the level I and level II data to the charts and indicators

#include <memory>
#include <vector>

#include <TFTrading/Watch.h>

#include <TFIQFeed/Level2/Symbols.hpp>
#include <TFIQFeed/Level2/FeatureSet.hpp>

#include "Stochastic.hpp"
#include "MovingAverage.hpp"

namespace ou {
namespace tf {
namespace l2 {
  class PanelTrade;
}
}
}

namespace config {
  class Options;
}

class InteractiveChart;

class FeedModel {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  FeedModel( pWatch_t, const config::Options& );  // Future(ByOrder)

  void Set( ou::tf::l2::PanelTrade* );
  void Set( InteractiveChart* );

  void AddToView( ou::ChartDataView& cdv, size_t price, size_t stoch );

  void Connect();
  void Disconnect();

  void FeatureSetDump();

protected:
private:

  bool m_bTriggerFeatureSetDump;

  double m_dblImbalanceMean, m_dblImbalanceSlope;

  pWatch_t m_pWatchUnderlying;

  std::atomic_uint32_t m_nMarketOrdersAsk; // pull from InteractiveChart
  std::atomic_uint32_t m_nMarketOrdersBid; // pull from InteractiveChart

  ou::tf::iqfeed::l2::OrderBased m_OrderBased; // direct access
  ou::tf::iqfeed::l2::FeatureSet m_FeatureSet;
  std::unique_ptr<ou::tf::iqfeed::l2::Symbols> m_pDispatch;

  ou::tf::l2::PanelTrade* m_pPanelTrade;
  InteractiveChart* m_pInteractiveChart;

  using pStochastic_t = std::unique_ptr<Stochastic>;
  using vStochastic_t = std::vector<pStochastic_t>;
  vStochastic_t m_vStochastic;

  using vMovingAverage_t = std::vector<MovingAverage>;
  vMovingAverage_t m_vMovingAverage;

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void StartDepthByOrder( size_t nLevels );

  void Imbalance( const ou::tf::Depth& );

};

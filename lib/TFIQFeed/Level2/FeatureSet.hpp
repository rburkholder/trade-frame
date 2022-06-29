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
 * File:    FeatureSet.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFIQFeed/Level2
 * Created: May 11, 2022 15:24
 */

 // based upon the paper:
 // Modeling high-frequency limit order book dynamics with support vector machines
 // October 24, 2013, Alec N.Kercheval, Yuan Zhang
 // page 16, table 2, Feature Vector Sets

#pragma once

#include <vector>

#include <TFIndicators/RunningStats.h>

#include "Symbols.hpp"
#include "FeatureSet_Level.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

class FeatureSet {
public:

  using price_t = ou::tf::Trade::price_t;
  using volume_t = ou::tf::Trade::volume_t;

  FeatureSet();
  ~FeatureSet();

  void Set( size_t nLevels );

  void HandleBookChangesAsk( ou::tf::iqfeed::l2::EOp, unsigned int, const ou::tf::Depth& );
  void HandleBookChangesBid( ou::tf::iqfeed::l2::EOp, unsigned int, const ou::tf::Depth& );

  // v7 Ask
  void Ask_IncLimit(  unsigned int, const ou::tf::Depth& );
  void Ask_IncMarket( unsigned int, const ou::tf::Depth& );
  void Ask_IncCancel( unsigned int, const ou::tf::Depth& );

  // v7 Bid
  void Bid_IncLimit(  unsigned int, const ou::tf::Depth& );
  void Bid_IncMarket( unsigned int, const ou::tf::Depth& );
  void Bid_IncCancel( unsigned int, const ou::tf::Depth& );

  void ImbalanceSummary( ou::tf::RunningStats::Stats& );

  void Emit() const;

protected:
private:

  size_t m_nLevels;

  using vLevels_t = std::vector<FeatureSet_Level>;
  vLevels_t m_vLevels;

};

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou

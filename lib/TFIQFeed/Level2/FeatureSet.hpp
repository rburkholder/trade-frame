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
#include <ostream>

#include "Symbols.hpp"
#include "FeatureSet_Level.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace linear {
  class Stats;
}
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

class FeatureSet {
public:

  using price_t = ou::tf::Trade::price_t;
  using volume_t = ou::tf::Trade::volume_t;

  FeatureSet();
  ~FeatureSet();

  // Initialization

  void Set( size_t nLevels );

  // Queries

  using vLevels_t = std::vector<FeatureSet_Level>;
  const vLevels_t& FVS() const { return m_vLevels; }

  void ImbalanceSummary( ou::tf::linear::Stats& ) const;

  // Assignment / Update

  void HandleBookChangesAsk( ou::tf::iqfeed::l2::EOp, unsigned int, const ou::tf::Depth& );
  void HandleBookChangesBid( ou::tf::iqfeed::l2::EOp, unsigned int, const ou::tf::Depth& );

  void Ask_IncLimit(  unsigned int, const ou::tf::Depth& ); // v7 ask
  void Ask_IncMarket( unsigned int, const ou::tf::Depth& );
  void Ask_IncCancel( unsigned int, const ou::tf::Depth& );

  void Bid_IncLimit(  unsigned int, const ou::tf::Depth& ); // v7 bid
  void Bid_IncMarket( unsigned int, const ou::tf::Depth& );
  void Bid_IncCancel( unsigned int, const ou::tf::Depth& );

  // Diagnostic

  bool IntegrityCheck() const;
  const std::string Header();
  std::ostream& operator<<( std::ostream& s ) const;

protected:
private:

  size_t m_nLevels;

  vLevels_t m_vLevels;

};

std::ostream& operator<<( std::ostream&, const FeatureSet& );

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou

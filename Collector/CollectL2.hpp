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
 * File:    CollectL2.hpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 6, 2025 06:18:07
 */

#pragma once

#include <TFTrading/Watch.h>

#include <TFIQFeed/Level2/Symbols.hpp>

#include "Collect.hpp"
#include "FillWrite.hpp"

namespace collect {

class L2: public Base {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  L2( const std::string& sPathPrefix, pWatch_t );
  ~L2();

  void Write() override; // incremental write
  size_t Count() const { return m_cntDepthsByOrder; }

protected:
private:

  size_t m_cntDepthsByOrder;

  pWatch_t m_pWatch;

  ou::tf::iqfeed::l2::OrderBased m_OrderBasedL2; // direct access
  std::unique_ptr<ou::tf::iqfeed::l2::Symbols> m_pDispatch;

  using fwDepthsByOrder_t = ou::tf::FillWrite<ou::tf::DepthsByOrder>;
  std::unique_ptr<fwDepthsByOrder_t> m_pfwDepthsByOrder;

  void HandleWatchDepthByOrder( const ou::tf::DepthByOrder& );
};

} // namespace collect

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
 * File:    Process.hpp
 * Author:  raymond@burkholder.net
 * Project: CollectFuL2
 * Created: October 20, 2022 21:07:40
 */

// inspiration from Dividend project

#pragma once

#include <array>
#include <atomic>

#include <TFIQFeed/Provider.h>

#include <TFIQFeed/Level2/Symbols.hpp>

#include <TFTrading/Watch.h>
#include <TFTrading/Instrument.h>

#include "Config.hpp"

namespace ou {
namespace tf {
  class ComposeInstrument;
} // namespace tf
} // namespace ou

class Process {
public:

  Process(
    const config::Choices&
  , const std::string& sTimeStamp
  );
  ~Process();

  void Write(); // incremental write of l2
  void Finish(); // stop and write l1 + remainder of l2
  size_t Count() const { return m_cntDepthsByOrder; }

protected:
private:

  const std::string m_sPathName;
  std::string m_sPathName_Depth;

  const config::Choices& m_choices;

  using pIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;
  pIQFeed_t m_piqfeed;

  std::unique_ptr<ou::tf::ComposeInstrument> m_pComposeInstrumentIQFeed;

  bool m_bHdf5AttributesSet;
  size_t m_cntDepthsByOrder;

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  pInstrument_t m_pInstrument;

  using pWatch_t = ou::tf::Watch::pWatch_t;
  pWatch_t m_pWatch;

  using rDepthsByOrder_t = std::array<ou::tf::DepthsByOrder,2>;
  rDepthsByOrder_t m_rDepthsByOrder; // one writes while one collects
  using ixDepthsByOrder_t = std::atomic<rDepthsByOrder_t::size_type>;
  ixDepthsByOrder_t m_ixDepthsByOrder_Filling;
  ixDepthsByOrder_t m_ixDepthsByOrder_Writing;

  ou::tf::iqfeed::l2::OrderBased m_OrderBased; // direct access
  std::unique_ptr<ou::tf::iqfeed::l2::Symbols> m_pDispatch;

  void StartIQFeed();
  void HandleIQFeedConnected( int );
  void ConstructUnderlying();
  void StartWatch();
  void StopWatch();
};

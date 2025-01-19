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
 * Project: Collector
 * Created: October 20, 2022 21:07:40
 */

// inspiration from Dividend project

#pragma once

#include <functional>
#include <unordered_map>

#include <TFTrading/Watch.h>

#include <TFIQFeed/Provider.h>

#include "Config.hpp"
#include "CollectL1.hpp"
#include "CollectL2.hpp"

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

  void Write(); // incremental write

protected:
private:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  using fWatch_t = std::function<void(pWatch_t)>;

  const std::string m_sPathName;

  const config::Choices& m_choices;

  using pIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;
  pIQFeed_t m_piqfeed;

  std::unique_ptr<ou::tf::ComposeInstrument> m_pComposeInstrumentIQFeed;

  using mapWatch_t = std::unordered_map<std::string, pWatch_t>;
  mapWatch_t m_mapWatch;

  using pCollectL1_t = std::unique_ptr<collect::L1>;
  using mapCollectL1_t = std::unordered_map<std::string, pCollectL1_t>;
  mapCollectL1_t m_mapCollectL1;

  using pCollectL2_t = std::unique_ptr<collect::L2>;
  using mapCollectL2_t = std::unordered_map<std::string, pCollectL2_t>;
  mapCollectL2_t m_mapCollectL2;

  void StartIQFeed();
  void HandleIQFeedConnected( int );
  void InitializeComposeInstrument();
  void ConstructWatch( const std::string&, fWatch_t&& );
  void ConstructCollectors();
  void ConstructCollectorL1( pWatch_t );
  void ConstructCollectorL2( pWatch_t );
};

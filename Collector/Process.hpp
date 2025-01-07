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

#include <unordered_map> // might use unordered_set instead

#include <TFTrading/Instrument.h>

#include <TFIQFeed/Provider.h>

#include "Config.hpp"
#include "CollectL1.hpp"

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

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  const std::string m_sPathName;

  const config::Choices& m_choices;

  using pIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;
  pIQFeed_t m_piqfeed;

  std::unique_ptr<ou::tf::ComposeInstrument> m_pComposeInstrumentIQFeed;

  using pCollectL1_t = std::unique_ptr<collect::L1>;
  using mapCollectL1_t = std::unordered_map<std::string, pCollectL1_t>;
  mapCollectL1_t m_mapCollectL1;

  void StartIQFeed();
  void HandleIQFeedConnected( int );
  void InitializeComposeInstrument();
  void ConstructCollectors();
  void ConstructCollector( pInstrument_t );
};

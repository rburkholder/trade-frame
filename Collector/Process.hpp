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

#include <thread>

#include <TFIQFeed/Provider.h>
#include <TFTrading/Instrument.h>

#include "Config.hpp"

namespace ou {
namespace tf {
  class BuildInstrument;
namespace iqfeed {
  class OptionChainQuery;
} // namespace iqfeed
} // namespace tf
} // namespace ou

class Process {
public:

  Process( const config::Choices& );
  ~Process();

  void Abort();
  void Wait();

protected:
private:

  bool m_bDone; // required for the nature of the condition variable
  std::mutex m_mutexWait;
  std::condition_variable m_cvWait;

  const config::Choices& m_choices;

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  pInstrument_t m_pInstrumentUnderlying;

  using pIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;
  pIQFeed_t m_piqfeed;

  std::shared_ptr<ou::tf::BuildInstrument> m_pBuildInstrumentIQFeed;

  using pOptionChainQuery_t = std::shared_ptr<ou::tf::iqfeed::OptionChainQuery>;
  pOptionChainQuery_t m_pOptionChainQuery; // need to disconnect

  void StartIQFeed();
  void HandleIQFeedConnected( int );
  void StartChainQuery();
  void HandleChainQueryConnected( int );
  void ConstructUnderlying();
  void StartWatch( pInstrument_t );
};
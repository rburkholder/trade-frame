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

#include <TFTrading/Watch.h>
#include <TFTrading/Instrument.h>

#include <TFIQFeed/Provider.h>

#include "Config.hpp"
#include "FillWrite.hpp"

namespace ou {
namespace tf {
  class ComposeInstrument;
  class HDF5Attributes;
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
  void Finish(); // write and stop

protected:
private:

  const std::string m_sPathName;

  const config::Choices& m_choices;

  using pIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;
  pIQFeed_t m_piqfeed;

  std::unique_ptr<ou::tf::ComposeInstrument> m_pComposeInstrumentIQFeed;

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  pWatch_t m_pWatch;

  using fwQuotes_t = ou::tf::FillWrite<ou::tf::Quotes>;
  std::unique_ptr<fwQuotes_t> m_pfwQuotes;

  using fwTrades_t = ou::tf::FillWrite<ou::tf::Trades>;
  std::unique_ptr<fwTrades_t> m_pfwTrades;

  void StartIQFeed();
  void HandleIQFeedConnected( int );
  void ConstructUnderlying();
  void HandleWatchTrade( const ou::tf::Trade& );
  void HandleWatchQuote( const ou::tf::Quote& );
  void SetAttributes( ou::tf::HDF5Attributes& );
  void StartWatch( pInstrument_t );
  void StopWatch();
};
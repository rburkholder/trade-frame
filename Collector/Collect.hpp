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
 * File:    Collect.hpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 5, 2025 17:11:37
 */

#pragma once

#include <TFTrading/Watch.h>

#include "FillWrite.hpp"

namespace ou {
namespace tf {
  class HDF5Attributes;
} // namespace tf
} // namespace ou

class Collect {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  Collect( const std::string& sPathPrefix, pWatch_t );
  ~Collect();

  void Write(); // incremental write

protected:
private:

  pWatch_t m_pWatch;

  using fwQuotes_t = ou::tf::FillWrite<ou::tf::Quotes>;
  std::unique_ptr<fwQuotes_t> m_pfwQuotes;

  using fwTrades_t = ou::tf::FillWrite<ou::tf::Trades>;
  std::unique_ptr<fwTrades_t> m_pfwTrades;

  void HandleWatchTrade( const ou::tf::Trade& );
  void HandleWatchQuote( const ou::tf::Quote& );
  void SetAttributes( ou::tf::HDF5Attributes& );
};
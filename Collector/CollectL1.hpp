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
 * File:    CollectL1.hpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 5, 2025 17:11:37
 */

#pragma once

#include <TFTrading/Watch.h>

#include "Collect.hpp"
#include "FillWrite.hpp"

namespace collect {

class L1: public Base {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  L1( const std::string& sPathPrefix, pWatch_t );
  ~L1();

  void Write() override; // incremental write

protected:
private:

  pWatch_t m_pWatch;

  using fwQuotes_t = ou::tf::FillWrite<ou::tf::Quotes>;
  std::unique_ptr<fwQuotes_t> m_pfwQuotes;

  using fwTrades_t = ou::tf::FillWrite<ou::tf::Trades>;
  std::unique_ptr<fwTrades_t> m_pfwTrades;

  void HandleWatchTrade( const ou::tf::Trade& );
  void HandleWatchQuote( const ou::tf::Quote& );
};

} // namespace collect

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
 * File:    Position.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFAlpaca
 * Created: June 13, 2022 13:17
 */

#pragma once

#include <string>
#include <vector>

namespace ou {
namespace tf {
namespace alpaca {
namespace position {

struct Current {
  std::string asset;
  std::string symbol;
  std::string exchange;
  std::string asset_class;
  bool asset_marginable;
  std::string avg_entry_price; // double
  std::string qty; // uint32_t
  std::string side;
  std::string market_value; // double
  std::string cost_basis; // double
  std::string unrealized_pl; // double
  std::string unrealized_plpc; // double
  std::string unrealized_intraday_pl; // double
  std::string unrealized_intraday_plpc; // double
  std::string current_price; // double
  std::string lastday_price; // double
  std::string change_today; // double
  uint32_t qty_available;  // uint32_t
};

using vCurrent_t = std::vector<Current>;
void Decode( const std::string&, Current& );
void Decode( const std::string&, vCurrent_t& );

} // namespace position
} // namespace alpaca
} // namespace tf
} // namespace ou

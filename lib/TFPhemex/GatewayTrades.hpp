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
 * File:    GatewayTrades.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFPhemex
 * Created: 2022/08/02 21:34:35
 */

#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace boost {
namespace json {
  class array;
  class value;
}
}

//namespace json = boost::json;

namespace ou {
namespace tf {
namespace phemex {
namespace gateway {
namespace trades {

// == Trades ==

struct trade {
  std::uint64_t time_stamp;
  std::string side;
  std::uint64_t price;
  std::uint64_t quantity;
};

using v_trade_t = std::vector<trade>;

} // namespace trades
} // namespace gateway
} // namespace phemex
} // namespace tf
} // namespace ou

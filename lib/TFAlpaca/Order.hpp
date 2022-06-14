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
 * File:    Order.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFAlpaca
 * Created: June 7, 2022 12:48
 */

#pragma once

#include <string>

namespace boost {
namespace json {
  class object;
  class value;
}
}

namespace ou {
namespace tf {
namespace alpaca {
namespace order {

struct Request {
  std::string symbol;  // required
  uint32_t quantity;  // required  - does this need to handle numbers with a decimal place for crypto?
  double notional;  // required, dollar amount to trade, not used if nQuantity is supplied, use for fractional quantities
  std::string side;    // required: 'buy' or 'sell', turn this into enumeration
  std::string type;   // required: 'market', 'limit', 'stop', 'stop_limit', 'trailing_stop', turn this into enumeration
  std::string time_in_force; // required: 'day', 'gtc', 'opg', 'cls', 'ioc', 'fok'
  double limit_price; // required, if 'limit' or 'stop_limit'
  double stop_price; // required, if 'stop' or 'stop_limit'
  double trail_price; // required, this or 'trail_percent' if 'trailing_stop'
  double trail_percent; // required, this or 'trail_price' if 'trailing_stop'
  bool extended_hours; // optional, default false, useful for 'day'/'limit'
  std::string client_order_id; // optional, unique, auto generated if not present
  std::string order_class; // optional, 'simple', 'bracket', 'oco', 'oto'
  // object take_profit;  // optional, advanced formatting, tbd
  // object stop_loss;    // optional, advanced formatting, tbd
};

struct Status {
  std::string asset_class;
  std::string asset_id;
  std::string cancel_requested_at;
  std::string canceled_at;
  std::string client_order_id;
  std::string created_at;
  std::string expired_at;
  bool extended_hours;
  std::string failed_at;
  std::string filled_at;
  std::string filled_avg_price;
  std::string filled_qty;
  std::string hwm;
  std::string id;
  std::string legs;
  std::string limit_price;
  std::string notional;
  std::string order_class;
  std::string order_type;
  std::string qty;
  std::string replaced_at;
  std::string replaced_by;
  std::string replaces;
  std::string side;
  std::string status;
  std::string stop_price;
  std::string submitted_at;
  std::string symbol;
  std::string time_in_force;
  std::string trail_percent;
  std::string trail_price;
  std::string type;
  std::string updated_at;
};

void Decode( const std::string&, Status& );
void Decode( boost::json::value const&, Status& );

} // namespace order
} // namespace alpaca
} // namespace tf
} // namespace ou

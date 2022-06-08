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
 * File:    Order.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFAlpaca
 * Created: June 7, 2022 12:48
 */

#include <boost/json.hpp>

#include "Order.hpp"

namespace json = boost::json;           // from <boost/json.hpp>

namespace ou {
namespace tf {
namespace alpaca {

namespace {

template<class T>
void extract( json::object const& obj, T& t, json::string_view key ) {
    t = json::value_to<T>( obj.at( key ) );
}

Message_Request tag_invoke( json::value_to_tag<Message_Request>, json::value const& jv ) {
  Message_Request msg;
  json::object const& obj = jv.as_object();
  extract( obj, msg.symbol, "symbol" );
  extract( obj, msg.quantity, "quantity" );
  extract( obj, msg.notional, "notional" );
  extract( obj, msg.time_in_force, "time_in_force" );
  extract( obj, msg.limit_price, "limit_price" );
  extract( obj, msg.stop_price, "stop_price" );
  extract( obj, msg.trail_price, "trail_price" );
  extract( obj, msg.trail_percent, "trail_percent" );
  extract( obj, msg.extended_hours, "extended_hours" );
  extract( obj, msg.client_order_id, "client_order_id" );
  extract( obj, msg.order_class, "order_class" );
  return msg;
}


} // namespace anonymous

} // namespace alpaca
} // namespace tf
} // namespace ou

/*
https://alpaca.markets/docs/trading/orders/#time-in-force
day: regular trading hours for the day, can be marked for extended hours
gtc: good till cancelled
opg: market on open, limit on open - market opening auction only
cls: market on close, limit on close - market closing auction only
ioc: immediate or cancel, full/partial/no-fill are valid result, api v2
fok: fill or kill, full fill only, or cancel, api v2

https://alpaca.markets/docs/trading/orders/#bracket-orders
simple: default order
bracket: three chained orders, required special nested JSON object
oco: one cancels other, submitted after a 'simple' in order to emulate complete bracket
oto: one triggers other, auto insertion of limit or stop after execution of original

*/
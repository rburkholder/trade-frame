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

#include <iostream>

#include <boost/json.hpp>

#include "Order.hpp"

namespace json = boost::json;

namespace ou {
namespace tf {
namespace alpaca {
namespace order {

namespace {

template<class T>
void extract( json::object const& obj, T& t, json::string_view key ) {
  if ( obj.at( key ).is_null() ) {
    if ( obj.at( key ).is_string() ) {}
    else {
      T init {};
      t = init;
    }
  }
  else {
    t = json::value_to<T>( obj.at( key ) );
  }
}

} // namespace anonymous

Request tag_invoke( json::value_to_tag<Request>, json::value const& jv ) {
  Request msg;
  json::object const& obj = jv.as_object();
  extract( obj, msg.symbol, "symbol" );
  extract( obj, msg.quantity, "qty" );
  extract( obj, msg.notional, "notional" );
  extract( obj, msg.side, "side" );
  extract( obj, msg.type, "type" );
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

Status tag_invoke( json::value_to_tag<Status>, json::value const& jv ) {
  Status msg;
  json::object const& obj = jv.as_object();
  extract( obj, msg.asset_class, "asset_class" );
  extract( obj, msg.asset_id, "asset_id" );
  extract( obj, msg.cancel_requested_at, "cancel_requested_at" );
  extract( obj, msg.canceled_at, "canceled_at" );
  extract( obj, msg.client_order_id, "client_order_id" );
  extract( obj, msg.created_at, "created_at" );
  extract( obj, msg.expired_at, "expired_at" );
  extract( obj, msg.extended_hours, "extended_hours" );
  extract( obj, msg.failed_at, "failed_at" );
  extract( obj, msg.filled_at, "filled_at" );
  extract( obj, msg.filled_avg_price, "filled_avg_price" );
  extract( obj, msg.filled_qty, "filled_qty" );
  extract( obj, msg.hwm, "hwm" );
  extract( obj, msg.id, "id" );
  extract( obj, msg.legs, "legs" );
  extract( obj, msg.limit_price, "limit_price" );
  extract( obj, msg.notional, "notional" );
  extract( obj, msg.order_class, "order_class" );
  extract( obj, msg.order_type, "order_type" );
  extract( obj, msg.qty, "qty" );
  extract( obj, msg.replaced_at, "replaced_at" );
  extract( obj, msg.replaced_by, "replaced_by" );
  extract( obj, msg.replaces, "replaces" );
  extract( obj, msg.side, "side" );
  extract( obj, msg.status, "status" );
  extract( obj, msg.stop_price, "stop_price" );
  extract( obj, msg.submitted_at, "submitted_at" );
  extract( obj, msg.symbol, "symbol" );
  extract( obj, msg.time_in_force, "time_in_force" );
  extract( obj, msg.trail_percent, "trail_percent" );
  extract( obj, msg.trail_price, "trail_price" );
  extract( obj, msg.type, "type" );
  extract( obj, msg.updated_at, "updated_at" );
  return msg;
}

OrderId tag_invoke( json::value_to_tag<OrderId>, json::value const& jv ) {
  OrderId msg;
  json::object const& obj = jv.as_object();
  extract( obj, msg.client_order_id, "client_order_id" );
  extract( obj, msg.id, "id" );
  return msg;
}

void Decode( boost::json::value const& jv, Status& status ) {
  status = json::value_to<Status>( jv );
}

void Decode( const std::string& sMessage, vOrderId_t& vOrderId ) {

  boost::system::error_code jec;
  json::value jv = json::parse( sMessage, jec );
  if ( jec.failed() ) {
    std::cout << "failed to parse current order id" << std::endl;
  }
  else {
    vOrderId = json::value_to<vOrderId_t>( jv );
  }
}


} // namespace order
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
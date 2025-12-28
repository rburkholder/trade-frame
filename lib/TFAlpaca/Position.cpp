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
 * File:    Position.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFAlpaca
 * Created: June 13, 2022 13:17
 */

#include <iostream>

#include <boost/json.hpp>

#include "Position.hpp"

namespace json = boost::json;

namespace ou {
namespace tf {
namespace alpaca {
namespace position {

namespace {
  template<class T>
  void extract( json::object const& obj, T& t, json::string_view key ) {
    t = json::value_to<T>( obj.at( key ) );
  }
} // namespace anonymous

Current tag_invoke( json::value_to_tag<Current>, json::value const& jv ) {
  Current current;
  json::object const& obj = jv.as_object();
  extract( obj, current.asset, "asset_id" );
  extract( obj, current.symbol, "symbol" );
  extract( obj, current.exchange, "exchange" );
  extract( obj, current.asset_class, "asset_class" );
  extract( obj, current.asset_marginable, "asset_marginable" );
  extract( obj, current.avg_entry_price, "avg_entry_price" );
  extract( obj, current.qty, "qty" );
  extract( obj, current.side, "side" );
  extract( obj, current.market_value, "market_value" );
  extract( obj, current.cost_basis, "cost_basis" );
  extract( obj, current.unrealized_pl, "unrealized_pl" );
  extract( obj, current.unrealized_plpc, "unrealized_plpc" );
  extract( obj, current.unrealized_intraday_pl, "unrealized_intraday_pl" );
  extract( obj, current.unrealized_intraday_plpc, "unrealized_intraday_plpc" );
  extract( obj, current.current_price, "current_price" );
  extract( obj, current.lastday_price, "lastday_price" );
  extract( obj, current.change_today, "change_today" );
  return current;
}

void Decode( const std::string& sMessage, Current& current ) {

  boost::system::error_code jec;
  json::value jv = json::parse( sMessage, jec );
  if ( jec.failed() ) {
    std::cout << "failed to parse current position" << std::endl;
  }
  else {
    current = json::value_to<Current>( jv );
  }
}

void Decode( const std::string& sMessage, vCurrent_t& vMessage ) {

  boost::system::error_code jec;
  json::value jv = json::parse( sMessage, jec );
  if ( jec.failed() ) {
    std::cout << "failed to parse current positions" << std::endl;
  }
  else {
    vMessage = json::value_to<vCurrent_t>( jv );
  }
}

} // namespace position
} // namespace alpaca
} // namespace tf
} // namespace ou

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
 * File:    Products.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFPhemex
 * Created: July 26, 2022 14:52:52
 */

#include <iostream>

#include <boost/json.hpp>

#include "Products.hpp"

namespace json = boost::json;

namespace ou {
namespace tf {
namespace phemex {
namespace products {

namespace {

template<class T>
void extract( json::object const& obj, json::string_view key, T& t ) {
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

Currency tag_invoke( json::value_to_tag<Currency>, json::value const& jv ) {
  Currency msg;
  json::object const& obj = jv.as_object();
  extract( obj, "currency", msg.currency );
  extract( obj, "name", msg.name );
  extract( obj, "code", msg.code );
  extract( obj, "valueScale", msg.valueScale );
  extract( obj, "minValueEv", msg.minValueEv );
  extract( obj, "maxValueEv", msg.maxValueEv );
  extract( obj, "needAddrTag", msg.needAddrTag );
  extract( obj, "status", msg.status );
  return msg;
}

void Decode( boost::json::value const& jv, Currency& currency ) {
  currency = json::value_to<Currency>( jv );
}

void Decode( const std::string& sMessage, vCurrency_t& vCurrency ) {

  json::error_code jec;
  json::value jv = json::parse( sMessage, jec );
  if ( jec.failed() ) {
    std::cout << "failed to parse current order id" << std::endl;
  }
  else {
    vCurrency = json::value_to<vCurrency_t>( jv );
  }
}

} // namespace products
} // namespace phemex
} // namespace tf
} // namespace ou

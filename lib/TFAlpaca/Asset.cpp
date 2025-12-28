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
 * File:    Asset.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFAlpaca
 * Created: June 5, 2022 16:08
 */

#include <boost/json.hpp>

#include "Asset.hpp"

namespace json = boost::json;

namespace ou {
namespace tf {
namespace alpaca {

template<class T>
void extract( json::object const& obj, T& t, json::string_view key ) {
    t = json::value_to<T>( obj.at( key ) );
}

ou::tf::alpaca::Asset::Message tag_invoke( json::value_to_tag<ou::tf::alpaca::Asset::Message>, json::value const& jv ) {
  ou::tf::alpaca::Asset::Message msg;
  json::object const& obj = jv.as_object();
  extract( obj, msg.id, "id" ); // uuid
  extract( obj, msg.class_, "class" );  // us_equity or crypto
  extract( obj, msg.exchange, "exchange" ); // AMEX, ARCA, BATS, NYSE, NASDAQ, NYSEARCA, OTC
  extract( obj, msg.symbol, "symbol" );
  extract( obj, msg.status, "status" ); // active or inactive
  extract( obj, msg.tradable, "tradable" );
  extract( obj, msg.marginable, "marginable" );
  extract( obj, msg.shortable, "shortable" );
  extract( obj, msg.easy_to_borrow, "easy_to_borrow" );
  extract( obj, msg.fractionable, "fractionable" );
  return msg;
}

Asset::Asset( const idSymbol_t& sSymbol, pInstrument_t pInstrument )
: ou::tf::Symbol<Asset>( pInstrument, sSymbol )
{
}

Asset::~Asset() {
}

void Asset::Decode( const std::string& sMessage, Message& message ) {

  boost::system::error_code jec;
  json::value jv = json::parse( sMessage, jec );
  if ( jec.failed() ) {
    std::cout << "failed to parse Asset::Message" << std::endl;
  }
  else {
    message = json::value_to<Message>( jv );
  }
}

void Asset::Decode( const std::string& sMessage, vMessage_t& vMessage ) {

  boost::system::error_code jec;
  json::value jv = json::parse( sMessage, jec );
  if ( jec.failed() ) {
    std::cout << "failed to parse vector of Asset::Message" << std::endl;
  }
  else {
    vMessage = json::value_to<vMessage_t>( jv );
  }
}

} // namespace alpaca
} // namespace tf
} // namespace ou

// https://alpaca.markets/docs/api-references/broker-api/trading/orders/
// https://alpaca.markets/docs/api-references/trading-api/orders/
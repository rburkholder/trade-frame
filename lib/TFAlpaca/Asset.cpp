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
 * File:    Symbol.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFAlpaca
 * Created: June 5, 2022 16:08
 */

#include <boost/json.hpp>

#include "Asset.hpp"

namespace json = boost::json;           // from <boost/json.hpp>

namespace {

// https://alpaca.markets/docs/api-references/trading-api/assets/
struct AssetMessage {
  std::string id;
  std::string class_;
  std::string exchange;
  std::string symbol;
  std::string status;
  bool        tradable;
  bool        marginable;
  bool        shortable;
  bool        easy_to_borrow;
  bool        fractionable;
};

template<class T>
void extract( json::object const& obj, T& t, json::string_view key ) {
    t = json::value_to<T>( obj.at( key ) );
}

AssetMessage tag_invoke( json::value_to_tag<AssetMessage>, json::value const& jv ) {
  AssetMessage msg;
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

} // namespace anonymous

namespace ou {
namespace tf {
namespace alpaca {

Asset::Asset( const idSymbol_t& sSymbol, pInstrument_t pInstrument )
: ou::tf::Symbol<Asset>( pInstrument, sSymbol )
{
}

Asset::~Asset() {
}


} // namespace alpaca
} // namespace tf
} // namespace ou

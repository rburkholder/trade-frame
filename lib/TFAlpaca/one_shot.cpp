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
 * File:    one_shot.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFAlpaca
 * Created: June 6, 2022 15:01
 */

#include <iostream>

#include <boost/beast/version.hpp>

#include <boost/json.hpp>

#include "one_shot.hpp"

namespace ou {
namespace tf {
namespace alpaca {
namespace session {

namespace { // anonymous

// Report a failure
void fail( beast::error_code ec, char const* what ) {
  std::cerr << what << ": " << ec.message() << "\n";
}

namespace json  = boost::json;      // from <boost/json.hpp>

// https://alpaca.markets/docs/api-references/trading-api/assets/
struct Asset {
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

Asset tag_invoke( json::value_to_tag<Asset>, json::value const& jv ) {
  Asset asset;
  json::object const& obj = jv.as_object();
  extract( obj, asset.id, "id" ); // uuid
  extract( obj, asset.class_, "class" );  // us_equity or crypto
  extract( obj, asset.exchange, "exchange" ); // AMEX, ARCA, BATS, NYSE, NASDAQ, NYSEARCA, OTC
  extract( obj, asset.symbol, "symbol" );
  extract( obj, asset.status, "status" ); // active or inactive
  extract( obj, asset.tradable, "tradable" );
  extract( obj, asset.marginable, "marginable" );
  extract( obj, asset.shortable, "shortable" );
  extract( obj, asset.easy_to_borrow, "easy_to_borrow" );
  extract( obj, asset.fractionable, "fractionable" );
  return asset;
}

} // namespace anonymous

namespace json  = boost::json;      // from <boost/json.hpp>

one_shot::one_shot(
  asio::any_io_executor ex,
  ssl::context& ssl_ctx)
: resolver_( ex )
, stream_( ex, ssl_ctx )
{}

// Start the asynchronous operation
void one_shot::run(
  const std::string& sHost,
  const std::string& sPort,
  const std::string& sTarget,
  int version,
  const std::string& sAlpacaKey,
  const std::string& sAlpacaSecret
) {
  // Set SNI Hostname (many hosts need this to handshake successfully)
  if( !SSL_set_tlsext_host_name( stream_.native_handle(), sHost.c_str() ) )
  {
      beast::error_code ec{ static_cast<int>( ::ERR_get_error()), asio::error::get_ssl_category() };
      std::cerr << ec.message() << "\n";
      return;
  }

  //std::cout
  //  << "request '"
  //  << json::serialize( jv )
  //  << "'"
  //  << std::endl;

  // Set up an HTTP GET request message
  request_.version( version );
  request_.method( http::verb::get );
  request_.target( sTarget );
  request_.set( http::field::host, sHost );
  request_.set( http::field::user_agent, BOOST_BEAST_VERSION_STRING );
  request_.set( "APCA-API-KEY-ID", sAlpacaKey );
  request_.set( "APCA-API-SECRET-KEY", sAlpacaSecret );
  //req_.body() = json::serialize( jv );
  //req_.prepare_payload();

  // Look up the domain name
  resolver_.async_resolve(
    sHost, sPort,
    beast::bind_front_handler(
      &one_shot::on_resolve,
      shared_from_this()
    )
  );
}

void one_shot::on_resolve(
  beast::error_code ec,
  tcp::resolver::results_type results
) {
  if ( ec )
    return fail(ec, "resolve");

  // Set a timeout on the operation
  beast::get_lowest_layer( stream_ ).expires_after( std::chrono::seconds( 15 ) );

  // Make the connection on the IP address we get from a lookup
  beast::get_lowest_layer( stream_ ).async_connect(
    results,
    beast::bind_front_handler(
      &one_shot::on_connect,
      shared_from_this() )
    );
}

void one_shot::on_connect( beast::error_code ec, tcp::resolver::results_type::endpoint_type ) {
  if ( ec )
    return fail(ec, "connect");

  // Perform the SSL handshake
  stream_.async_handshake(
    ssl::stream_base::client,
    beast::bind_front_handler(
      &one_shot::on_handshake,
      shared_from_this()
    )
  );
}

void one_shot::on_handshake( beast::error_code ec ) {

  if ( ec )
      return fail(ec, "handshake");

  // Set a timeout on the operation
  beast::get_lowest_layer( stream_ ).expires_after( std::chrono::seconds( 15 ) );

  // Send the HTTP request to the remote host
  http::async_write( stream_, request_,
    beast::bind_front_handler(
      &one_shot::on_write,
      shared_from_this()
    )
  );
}

void one_shot::on_write(
  beast::error_code ec,
  std::size_t bytes_transferred
) {
  boost::ignore_unused(bytes_transferred);

  if ( ec )
    return fail( ec, "write" );

  // Receive the HTTP response
  http::async_read(stream_, buffer_, response_,
    beast::bind_front_handler(
      &one_shot::on_read,
      shared_from_this()
    )
  );
}

void one_shot::on_read( beast::error_code ec, std::size_t bytes_transferred ) {

  boost::ignore_unused( bytes_transferred );

  if ( ec )
    return fail(ec, "read");

  // Write the message to standard out
  //std::cout << res_ << std::endl;

  //std::cout << res_.body() << std::endl;

  json::error_code jec;
  json::value jv = json::parse( response_.body(), jec );
  if ( jec.failed() ) {
    return fail( jec, "parse" );
  }

  //alpaca::Asset asset( json::value_to<alpaca::Asset>( jv ) ); // single asset
  using vAsset_t = std::vector<Asset>;
  vAsset_t vAsset = json::value_to<vAsset_t>( jv );

  for ( const vAsset_t::value_type& vt: vAsset ) {
    std::cout
      //<< vt.id << ","
      << vt.class_ << ","
      << vt.exchange << ","
      << vt.symbol << ","
      << "trade=" << vt.tradable << ","
      << "short=" << vt.shortable << ","
      << "margin=" << vt.marginable
      << std::endl;
  }

  std::cout << "found " << vAsset.size() << " assets" << std::endl;

  // Set a timeout on the operation
  beast::get_lowest_layer( stream_ ).expires_after( std::chrono::seconds( 15 ) );

  // Gracefully close the stream - can the stream be re-used?
  stream_.async_shutdown(
    beast::bind_front_handler(
      &one_shot::on_shutdown,
      shared_from_this()
    )
  );
}

void one_shot::on_shutdown( beast::error_code ec ) {
  if ( ec == asio::error::eof ) {
      // Rationale:
      // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
      ec = {};
  }
  if(ec)
    return fail( ec, "shutdown" );

  // If we get here then the connection is closed gracefully
}

} // namespace session
} // namespace alpaca
} // namespace tf
} // namespace ou

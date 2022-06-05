//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP SSL client, asynchronous
//
//------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <functional>

#include <boost/beast/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <boost/asio/strand.hpp>

#include <boost/json.hpp>

#include <TFAlpaca/root_certificates.hpp>

#include "Config.hpp"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace json = boost::json;           // from <boost/json.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

// Report a failure
void fail(beast::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

namespace alpaca {

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

} // namespace asset

// Performs an HTTP GET and prints the response
class session : public std::enable_shared_from_this<session>
{
    tcp::resolver resolver_;
    beast::ssl_stream<beast::tcp_stream> stream_;
    beast::flat_buffer buffer_; // (Must persist between reads)
    http::request<http::empty_body> req_;
    //http::request<http::string_body> req_;
    http::response<http::string_body> res_;

public:
  explicit session(
      net::any_io_executor ex,
      ssl::context& ctx)
  : resolver_(ex)
  , stream_(ex, ctx)
  {
  }

  // Start the asynchronous operation
  void run(
      const std::string& sHost,
      const std::string& sPort,
      const std::string& sTarget,
      int version,
      const std::string& sAlpacaKey,
      const std::string& sAlpacaSecret
  ) {
      // Set SNI Hostname (many hosts need this to handshake successfully)
      if(! SSL_set_tlsext_host_name(stream_.native_handle(), sHost.c_str()))
      {
          beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
          std::cerr << ec.message() << "\n";
          return;
      }

      //json::value jv = {
      //  { "status", "active" },
      //  { "asset_class", "crypto" }
      //};

      //std::cout
      //  << "request '"
      //  << json::serialize( jv )
      //  << "'"
      //  << std::endl;

      // Set up an HTTP GET request message
      req_.version( version );
      req_.method( http::verb::get );
      req_.target( sTarget );
      req_.set( http::field::host, sHost );
      req_.set( http::field::user_agent, BOOST_BEAST_VERSION_STRING );
      req_.set( "APCA-API-KEY-ID", sAlpacaKey );
      req_.set( "APCA-API-SECRET-KEY", sAlpacaSecret );
      //req_.body() = json::serialize( jv );
      //req_.prepare_payload();

      // Look up the domain name
      resolver_.async_resolve(
          sHost,
          sPort,
          beast::bind_front_handler(
              &session::on_resolve,
              shared_from_this()));
  }

  void on_resolve(
      beast::error_code ec,
      tcp::resolver::results_type results)
  {
      if(ec)
          return fail(ec, "resolve");

      // Set a timeout on the operation
      beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

      // Make the connection on the IP address we get from a lookup
      beast::get_lowest_layer(stream_).async_connect(
          results,
          beast::bind_front_handler(
              &session::on_connect,
              shared_from_this()));
  }

  void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
  {
      if(ec)
          return fail(ec, "connect");

      // Perform the SSL handshake
      stream_.async_handshake(
          ssl::stream_base::client,
          beast::bind_front_handler(
              &session::on_handshake,
              shared_from_this()));
  }

  void on_handshake(beast::error_code ec)
  {
      if(ec)
          return fail(ec, "handshake");

      // Set a timeout on the operation
      beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

      // Send the HTTP request to the remote host
      http::async_write(stream_, req_,
          beast::bind_front_handler(
              &session::on_write,
              shared_from_this()));
  }

  void on_write(
      beast::error_code ec,
      std::size_t bytes_transferred)
  {
      boost::ignore_unused(bytes_transferred);

      if(ec)
          return fail(ec, "write");

      // Receive the HTTP response
      http::async_read(stream_, buffer_, res_,
          beast::bind_front_handler(
              &session::on_read,
              shared_from_this()));
  }

  void on_read( beast::error_code ec, std::size_t bytes_transferred ) {

    boost::ignore_unused(bytes_transferred);

    if(ec)
        return fail(ec, "read");

    // Write the message to standard out
    //std::cout << res_ << std::endl;

    //std::cout << res_.body() << std::endl;

    json::error_code ec2;
    json::value jv = json::parse( res_.body(), ec2 );
    if ( ec2.failed() ) {
      return fail( ec2, "parse" );
    }

    //alpaca::Asset asset( json::value_to<alpaca::Asset>( jv ) ); // single asset
    using vAsset_t = std::vector<alpaca::Asset>;
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
    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    // Gracefully close the stream
    stream_.async_shutdown(
        beast::bind_front_handler(
            &session::on_shutdown,
            shared_from_this()));
  }

  void on_shutdown(beast::error_code ec) {
    if(ec == net::error::eof)
    {
        // Rationale:
        // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
        ec = {};
    }
    if(ec)
        return fail(ec, "shutdown");

    // If we get here then the connection is closed gracefully
  }
};

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    // Check command line arguments.
//    if(argc != 4 && argc != 5)
//    {
//        std::cerr <<
//            "Usage: http-client-async-ssl <host> <port> <target> [<HTTP version: 1.0 or 1.1(default)>]\n" <<
//            "Example:\n" <<
//            "    http-client-async-ssl www.example.com 443 /\n" <<
//            "    http-client-async-ssl www.example.com 443 / 1.0\n";
//        return EXIT_FAILURE;
//    }
    //auto const host = argv[1];
    //auto const port = argv[2];
    //auto const target = argv[3];

    const std::string sVersion( "1.0" );
    int version = argc == 5 && !std::strcmp("1.0", sVersion.c_str()) ? 10 : 11;

    config::Choices choices;
    config::Load( "alpaca.cfg", choices );

    const std::string sPort( "443" );
    //const std::string sTarget( "/v2/assets" );
    //const std::string sTarget( "/v2/assets/GLD" );
    const std::string sTarget( "/v2/assets?status=active&asset_class=crypto" );

    // The io_context is required for all I/O
    net::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::tlsv12_client};

    // This holds the root certificate used for verification
    load_root_certificates(ctx);

    // Verify the remote server's certificate
    ctx.set_verify_mode(ssl::verify_peer);

    // Launch the asynchronous operation
    // The session is constructed with a strand to
    // ensure that handlers do not execute concurrently.
    std::make_shared<session>(
        net::make_strand(ioc),
        ctx
        )->run(choices.m_sAlpacaDomain, sPort, sTarget, version, choices.m_sAlpacaKey, choices.m_sAlpacaSecret);

    // Run the I/O service. The call will return when
    // the get operation is complete.
    ioc.run();

    return EXIT_SUCCESS;
}
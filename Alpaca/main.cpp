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

#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>

#include <boost/asio/strand.hpp>

#include <boost/json.hpp>

#include <TFAlpaca/root_certificates.hpp>

#include "Config.hpp"

namespace json  = boost::json;      // from <boost/json.hpp>
namespace asio  = boost::asio;      // from <boost/asio.hpp>
namespace ssl   = asio::ssl;        // from <boost/asio/ssl.hpp>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http  = beast::http;      // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>

using tcp = boost::asio::ip::tcp;   // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

// Report a failure
void fail( beast::error_code ec, char const* what ) {
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

namespace session { // contains one_shot, and web_socket

// ====
// https://www.boost.org/doc/libs/1_79_0/libs/beast/example/http/client/async-ssl/http_client_async_ssl.cpp

// Performs an HTTP GET and prints the response
class one_shot : public std::enable_shared_from_this<one_shot>
{
    tcp::resolver resolver_;
    beast::ssl_stream<beast::tcp_stream> stream_;
    beast::flat_buffer buffer_; // (Must persist between reads)
    http::request<http::empty_body> request_;
    //http::request<http::string_body> req_;
    http::response<http::string_body> response_;

public:
  explicit one_shot(
    asio::any_io_executor ex,
    ssl::context& ssl_ctx)
  : resolver_( ex )
  , stream_( ex, ssl_ctx )
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
          sHost,
          sPort,
          beast::bind_front_handler(
              &one_shot::on_resolve,
              shared_from_this() ) );
  }

  void on_resolve(
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

  void on_connect( beast::error_code ec, tcp::resolver::results_type::endpoint_type ) {
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

  void on_handshake( beast::error_code ec ) {

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

  void on_write(
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

  void on_read( beast::error_code ec, std::size_t bytes_transferred ) {

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
    beast::get_lowest_layer( stream_ ).expires_after( std::chrono::seconds( 15 ) );

    // Gracefully close the stream - can the stream be re-used?
    stream_.async_shutdown(
      beast::bind_front_handler(
        &one_shot::on_shutdown,
        shared_from_this()
      )
    );
  }

  void on_shutdown( beast::error_code ec ) {
    if ( ec == asio::error::eof ) {
        // Rationale:
        // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
        ec = {};
    }
    if(ec)
      return fail( ec, "shutdown" );

    // If we get here then the connection is closed gracefully
  }
};

// ====
// https://www.boost.org/doc/libs/1_79_0/libs/beast/example/websocket/client/async-ssl/websocket_client_async_ssl.cpp

// Sends a WebSocket message and prints the response
class web_socket : public std::enable_shared_from_this<web_socket>
{
  tcp::resolver resolver_;
  websocket::stream<
      beast::ssl_stream<beast::tcp_stream>> ws_;
  beast::flat_buffer buffer_;
  std::string host_;

  std::string key_;
  std::string secret_;

public:

  // Resolver and socket require an io_context
  explicit web_socket( asio::io_context& ioc, ssl::context& ssl_ctx )
    : resolver_( asio::make_strand( ioc ) )
    , ws_( asio::make_strand(ioc), ssl_ctx )
  {}

  // Start the asynchronous operation
  void run(
    const std::string& host,
    const std::string& port,
    const std::string& sAlpacaKey,
    const std::string& sAlpacaSecret
  ) {
    // Save these for later
    host_ = host;

    key_ = sAlpacaKey;
    secret_ = sAlpacaSecret;

    // Look up the domain name
    resolver_.async_resolve(
      host,
      port,
      beast::bind_front_handler(
        &web_socket::on_resolve,
        shared_from_this()
      )
    );
  }

private:

  void on_resolve(
    beast::error_code ec,
    tcp::resolver::results_type results
  ) {
    if ( ec )
      return fail(ec, "resolve");
    else
      std::cout << "ws.on_resolve" << std::endl;

    // Set a timeout on the operation
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds( 15 ));

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(ws_).async_connect(
      results,
      beast::bind_front_handler(
        &web_socket::on_connect,
        shared_from_this()
      )
    );
  }

  void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep ) {

    if ( ec )
      return fail(ec, "connect");
    else
      std::cout << "ws.on_connect" << std::endl;

    // Set a timeout on the operation
    beast::get_lowest_layer( ws_ ).expires_after( std::chrono::seconds( 15 ) );

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if( !SSL_set_tlsext_host_name(
      ws_.next_layer().native_handle(),
      host_.c_str())
    ) {
      ec = beast::error_code(static_cast<int>(::ERR_get_error()),
          asio::error::get_ssl_category());
      return fail(ec, "connect");
    }

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    host_ += ':' + std::to_string(ep.port());

    // Perform the SSL handshake
    ws_.next_layer().async_handshake(
      ssl::stream_base::client,
      beast::bind_front_handler(
        &web_socket::on_ssl_handshake,
        shared_from_this()
      )
    );
  }

  void on_ssl_handshake( beast::error_code ec ) {

    if ( ec )
      return fail( ec, "ssl_handshake" );
    else
      std::cout << "ws.on_ssl_handshake" << std::endl;

    // Turn off the timeout on the tcp_stream, because
    // the websocket stream has its own timeout system.
    beast::get_lowest_layer(ws_).expires_never();

    // Set suggested timeout settings for the websocket
    ws_.set_option(
      websocket::stream_base::timeout::suggested(
        beast::role_type::client)
      );

    // Set a decorator to change the User-Agent of the handshake
    ws_.set_option(websocket::stream_base::decorator(
      [](websocket::request_type& request )
      {
          request.set(http::field::user_agent,
              std::string(BOOST_BEAST_VERSION_STRING) +
                  " websocket-client-async-ssl");
      })
    );

    // Perform the websocket handshake
    ws_.async_handshake(host_, "/stream",
      beast::bind_front_handler(
        &web_socket::on_handshake,
        shared_from_this()
      )
    );
  }

  void on_handshake(beast::error_code ec) {

    if ( ec )
      return fail( ec, "handshake" );
    else
      std::cout << "ws.on_handshake" << std::endl;

    // https://alpaca.markets/deprecated/docs/api-documentation/api-v2/streaming/
    // this does square brackets, undesirable
    //json::value jv = {
    //  { "action", "authenticate" },
    //  { "data" ,
    //    { "key_id", key_ },
    //    { "secret_key", secret_ }
    //  }
    //};

    json::object auth;
    auth[ "action" ] = "authenticate";
    auth[ "data" ] = { { "key_id", key_}, { "secret_key", secret_ } };

    std::cout
      << "message: '"
      << json::serialize( auth )
      << "'"
      << std::endl;

    // Send the message
    ws_.async_write(
//      asio::buffer( json::serialize( jv ) ),
      asio::buffer( json::serialize( auth ) ),
      beast::bind_front_handler(
        &web_socket::on_write,
        shared_from_this()
      )
    );
  }

  void on_write(
    beast::error_code ec,
    std::size_t bytes_transferred
  ) {
    boost::ignore_unused(bytes_transferred);

    if ( ec )
      return fail(ec, "write");
    else
      std::cout << "ws.on_write" << std::endl;

    // Read a message into our buffer
    ws_.async_read(
      buffer_,
      beast::bind_front_handler(
        &web_socket::on_read,
        shared_from_this()
      )
    );
  }

  void on_read(
      beast::error_code ec,
      std::size_t bytes_transferred
  ) {
    boost::ignore_unused( bytes_transferred );

    if ( ec )
      return fail( ec, "read" );
    else
      std::cout << "ws.on_read" << std::endl;

    // Close the WebSocket connection
    ws_.async_close(websocket::close_code::normal,
      beast::bind_front_handler(
        &web_socket::on_close,
        shared_from_this()));
  }

  void on_close(beast::error_code ec) {
    if( ec )
      return fail(ec, "close");

    // If we get here then the connection is closed gracefully

    // The make_printable() function helps print a ConstBufferSequence
    std::cout << beast::make_printable( buffer_.data() ) << std::endl;
  }
};

} // namespace session


//------------------------------------------------------------------------------

int main( int argc, char** argv )
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

    //const std::string sVersion( "1.0" );
    //int version = ( argc == 5 && !std::strcmp( "1.0", sVersion.c_str() ) ) ? 10 : 11;
    int version = 11;

    config::Choices choices;
    config::Load( "alpaca.cfg", choices );

    const std::string sPort( "443" );
    //const std::string sTarget( "/v2/assets" );
    //const std::string sTarget( "/v2/assets/GLD" );
    const std::string sTarget( "/v2/assets?status=active&asset_class=crypto" );

    // The SSL context is required, and holds certificates
    ssl::context ssl_ctx{ ssl::context::tlsv12_client };

    // This holds the root certificate used for verification
    load_root_certificates( ssl_ctx );

    // Verify the remote server's certificate
    ssl_ctx.set_verify_mode( ssl::verify_peer );

    // The io_context is required for all I/O
    asio::io_context ioc;

    // Launch the asynchronous operation
    // The session is constructed with a strand to
    // ensure that handlers do not execute concurrently.
    std::make_shared<session::one_shot>(
      asio::make_strand( ioc ),
      ssl_ctx
      )->run( choices.m_sAlpacaDomain, sPort, sTarget, version, choices.m_sAlpacaKey, choices.m_sAlpacaSecret );

    std::make_shared<session::web_socket>(
      ioc,
      ssl_ctx
      )->run( choices.m_sAlpacaDomain, sPort, choices.m_sAlpacaKey, choices.m_sAlpacaSecret );

    // Run the I/O service. The call will return when
    // the get operation is complete.
    ioc.run();

    return EXIT_SUCCESS;
}
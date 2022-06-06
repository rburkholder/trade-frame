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
 * File:    web_socket.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFAlpaca
 * Created: June 6, 2022 15:01
 */

#include <iostream>

#include <boost/asio/strand.hpp>

#include <boost/json.hpp>

#include "web_socket.hpp"

namespace json  = boost::json;      // from <boost/json.hpp>
namespace http  = beast::http;      // from <boost/beast/http.hpp>

namespace ou {
namespace tf {
namespace alpaca {
namespace session {

namespace {

// Report a failure
void fail( beast::error_code ec, char const* what ) {
  std::cerr << what << ": " << ec.message() << "\n";
}

} // namespace anonymous

// ====

// https://www.boost.org/doc/libs/1_79_0/libs/beast/example/websocket/client/async-ssl/websocket_client_async_ssl.cpp

// Resolver and socket require an io_context
web_socket::web_socket( asio::io_context& ioc, ssl::context& ssl_ctx )
  : m_resolver( asio::make_strand( ioc ) )
  , m_ws( asio::make_strand(ioc), ssl_ctx )
{}

// Start the asynchronous operation
void web_socket::connect(
  const std::string& host
, const std::string& port
, const std::string& sAlpacaKey
, const std::string& sAlpacaSecret
, fConnected_t&& fConnected
) {
  // Save these for later
  m_host = host;
  m_fConnected = std::move( fConnected );

  m_key = sAlpacaKey;
  m_secret = sAlpacaSecret;

  // Look up the domain name
  m_resolver.async_resolve(
    host,
    port,
    beast::bind_front_handler(
      &web_socket::on_resolve,
      shared_from_this()
    )
  );
}

void web_socket::on_resolve(
  beast::error_code ec,
  tcp::resolver::results_type results
) {
  if ( ec )
    return fail(ec, "resolve");
  else {
    std::cout << "ws.on_resolve" << std::endl;
  }

  // Set a timeout on the operation
  beast::get_lowest_layer( m_ws ).expires_after(std::chrono::seconds( 15 ));

  // Make the connection on the IP address we get from a lookup
  beast::get_lowest_layer( m_ws ).async_connect(
    results,
    beast::bind_front_handler(
      &web_socket::on_connect,
      shared_from_this()
    )
  );
}

void web_socket::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep ) {

  if ( ec )
    return fail(ec, "connect");
  else {
    std::cout << "ws.on_connect" << std::endl;
  }

  // Set a timeout on the operation
  beast::get_lowest_layer( m_ws ).expires_after( std::chrono::seconds( 15 ) );

  // Set SNI Hostname (many hosts need this to handshake successfully)
  if( !SSL_set_tlsext_host_name(
    m_ws.next_layer().native_handle(),
    m_host.c_str())
  ) {
    ec = beast::error_code(static_cast<int>(::ERR_get_error()),
        asio::error::get_ssl_category());
    return fail(ec, "connect");
  }

  // Update the host_ string. This will provide the value of the
  // Host HTTP header during the WebSocket handshake.
  // See https://tools.ietf.org/html/rfc7230#section-5.4
  m_host += ':' + std::to_string(ep.port());

  // Perform the SSL handshake
  m_ws.next_layer().async_handshake(
    ssl::stream_base::client,
    beast::bind_front_handler(
      &web_socket::on_ssl_handshake,
      shared_from_this()
    )
  );
}

void web_socket::on_ssl_handshake( beast::error_code ec ) {

  if ( ec )
    return fail( ec, "ssl_handshake" );
  else {
    std::cout << "ws.on_ssl_handshake" << std::endl;
  }

  // Turn off the timeout on the tcp_stream, because
  // the websocket stream has its own timeout system.
  beast::get_lowest_layer( m_ws ).expires_never();

  // Set suggested timeout settings for the websocket
  m_ws.set_option(
    websocket::stream_base::timeout::suggested(
      beast::role_type::client)
    );

  // Set a decorator to change the User-Agent of the handshake
  m_ws.set_option(websocket::stream_base::decorator(
    [](websocket::request_type& request )
    {
        request.set(http::field::user_agent,
            std::string(BOOST_BEAST_VERSION_STRING) +
                " websocket-client-async-ssl");
    })
  );

  // Perform the websocket handshake
  m_ws.async_handshake( m_host, "/stream",
    beast::bind_front_handler(
      &web_socket::on_handshake,
      shared_from_this()
    )
  );
}

void web_socket::on_handshake(beast::error_code ec) {

  if ( ec )
    return fail( ec, "handshake" );
  else {
    std::cout << "ws.on_handshake" << std::endl;
  }

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
  auth[ "data" ] = { { "key_id", m_key}, { "secret_key", m_secret } };

  // Send the message
  m_ws.async_write(
    asio::buffer( json::serialize( auth ) ),
    beast::bind_front_handler(
      &web_socket::on_write_auth,
      shared_from_this()
    )
  );
}

void web_socket::on_write_auth(
  beast::error_code ec,
  std::size_t bytes_transferred
) {
  boost::ignore_unused(bytes_transferred);

  if ( ec )
    return fail( ec, "write" );
  else {
    std::cout << "ws.on_write_auth" << std::endl;
  }

  // Read a message into our buffer
  m_ws.async_read(
    m_buffer,
    beast::bind_front_handler(
      &web_socket::on_read_auth,
      shared_from_this()
    )
  );
}

void web_socket::on_read_auth(
    beast::error_code ec,
    std::size_t bytes_transferred
) {
  boost::ignore_unused( bytes_transferred );

  if ( ec )
    return fail( ec, "read" );
  else {
    std::cout << "ws.on_read_auth" << std::endl;

    // The make_printable() function helps print a ConstBufferSequence
    std::cout << beast::make_printable( m_buffer.data() ) << std::endl;

    if ( m_fConnected ) m_fConnected( true );
  }

}

void web_socket::trade_updates( bool bEnable ) {

  json::object listen;
  listen[ "action" ] = "listen";
  if ( bEnable ) {
    listen[ "data" ].emplace_object()[ "streams" ] = { "trade_updates" };
  }
  else {
    listen[ "data" ].emplace_object()[ "streams" ].emplace_array();
  }

  std::cout << "listen: '" << listen << "'" << std::endl;

  // Send the message
  m_ws.async_write(
    asio::buffer( json::serialize( listen ) ),
    beast::bind_front_handler(
      &web_socket::on_write_listen,
      shared_from_this()
    )
  );
}

void web_socket::on_write_listen(
  beast::error_code ec,
  std::size_t bytes_transferred
) {
  boost::ignore_unused(bytes_transferred);

  if ( ec )
    return fail(ec, "write");
  else {
    std::cout << "ws.on_write_listen" << std::endl;
  }

  // Read a message into our buffer
  m_ws.async_read(
    m_buffer,
    beast::bind_front_handler(
      &web_socket::on_read_listen,
      shared_from_this()
    )
  );
}

void web_socket::on_read_listen(
    beast::error_code ec,
    std::size_t bytes_transferred
) {
  boost::ignore_unused( bytes_transferred );

  if ( ec )
    return fail( ec, "read" );
  else {
    std::cout << "ws.on_read_listen" << std::endl;

    //if ( m_fConnected ) m_fConnected( true );

    // The make_printable() function helps print a ConstBufferSequence
    std::cout << beast::make_printable( m_buffer.data() ) << std::endl;
  }

}


void web_socket::disconnect() {

  // Close the WebSocket connection
  m_ws.async_close( websocket::close_code::normal,
    beast::bind_front_handler(
      &web_socket::on_close,
      shared_from_this()
    )
  );
}

void web_socket::on_close(beast::error_code ec) {
  if( ec )
    return fail(ec, "close");
  else
    std::cout << "ws.on_close" << std::endl;

  // If we get here then the connection is closed gracefully

}

} // namespace session
} // namespace alpaca
} // namespace tf
} // namespace ou

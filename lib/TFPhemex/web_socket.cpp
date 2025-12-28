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
 * Project: lib/TFPhemex
 * Created: June 6, 2022 15:01
 */

#include <iostream>

#include <boost/json.hpp>

#include <boost/asio/strand.hpp>

#include  <boost/beast/websocket/stream.hpp>

#include "web_socket.hpp"

namespace json = boost::json;
namespace http = beast::http; // from <boost/beast/http.hpp>

namespace ou {
namespace tf {
namespace phemex {
namespace session {

namespace {

const static unsigned int nHeartBeatIntervalSeconds( 5 );

const static std::string sUserAgent( "ounl.tradeframe/1.0" );

// Report a failure
void fail( beast::error_code ec, char const* what ) {
  std::cerr << what << ": " << ec.message() << std::endl;
}

} // namespace anonymous

// ====

// Clients can use WS built-in ping message
// or the application level ping message to DataGW as heartbeat.
// The heartbeat interval is recommended to be set as 5 seconds,
// and actively reconnect to DataGW if don't receive messages in 3 heartbeat intervals.

// https://www.boost.org/doc/libs/1_79_0/libs/beast/example/websocket/client/async-ssl/websocket_client_async_ssl.cpp

// Resolver and socket require an io_context
web_socket::web_socket( asio::io_context& srvc, ssl::context& ssl_ctx )
: m_srvc( srvc )
, m_strand( srvc )
, m_timer( srvc )
, m_bSendHeartBeat( false )
, m_id( 0 )
, m_bConnected( false )
, m_resolver( asio::make_strand( srvc ) )
, m_ws( asio::make_strand( srvc ), ssl_ctx )
{
  std::cout << "phemex::web_socket construction" << std::endl; // ensuring proper timing of handling
}

web_socket::~web_socket() {
  std::cout << "phemex::web_socket destruction" << std::endl; // ensuring proper timing of handling
}

// Start the asynchronous operation - alpaca specific, needs to be replaced
void web_socket::connect(
  const std::string& host
, const std::string& port
, fConnected_t&& fConnected
, fDisconnected_t&& fDisconnected
, fMessage_t&& fMessage
) {
  m_host = host;

  m_fConnected = std::move( fConnected );
  m_fDisconnected = std::move( fDisconnected );
  m_fMessage = std::move( fMessage );

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
    return fail(ec, "ws.on_resolve");
  else {
    //std::cout << "ws.on_resolve" << std::endl;
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
    return fail(ec, "ws.on_connect 1");
  else {
    //std::cout << "ws.on_connect" << std::endl;
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
    return fail( ec, "ws.on_connect 2" );
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
    return fail( ec, "ws.on_ssl_handshake" );
  else {
    //std::cout << "ws.on_ssl_handshake" << std::endl;
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
  m_ws.set_option( websocket::stream_base::decorator(
    []( websocket::request_type& request )
    {
      request.set(http::field::user_agent,
        std::string( sUserAgent ) + " websocket-client-async-ssl");
    })
  );

  // Perform the websocket handshake
  m_ws.async_handshake(
    m_host, "/ws",
    beast::bind_front_handler(
      &web_socket::on_handshake,
      shared_from_this()
    )
  );
}

void web_socket::on_handshake( beast::error_code ec ) {

  if ( ec )
    return fail( ec, "ws.on_handshake" );
  else {
    //std::cout << "ws.on_handshake" << std::endl;
  }

  m_bConnected = true;
  if ( m_fConnected ) m_fConnected( true );

  // start up generic listener
  m_ws.async_read(
    m_buffer,
    beast::bind_front_handler(
      &web_socket::on_read_listen,
      shared_from_this()
    )
  );

  m_ws.control_callback(
    []( beast::websocket::frame_type kind, boost::string_view payload)
    {
        // Do something with the payload
        std::cout
          << "ws.handshake.control_callback "
          << (int)kind
          << " view " << payload
          << std::endl;
        boost::ignore_unused(kind, payload);
    });

  m_bSendHeartBeat = true;
  m_timer.expires_after( boost::asio::chrono::seconds( nHeartBeatIntervalSeconds ) );
  m_timer.async_wait( std::bind( &web_socket::on_timer, this, std::placeholders::_1 ) );

  // TODO:
  //  1) start heart beat
  //  2) perform phemex style authentication

  // this is alpaca specific
  //json::object auth;
  //auth[ "action" ] = "authenticate";
  //auth[ "data" ] = { { "key_id", m_key}, { "secret_key", m_secret } };

  // Send the message
  //m_ws.async_write(
  //  asio::buffer( json::serialize( auth ) ),
  //  beast::bind_front_handler(
  //    &web_socket::on_write_auth,
  //    shared_from_this()
  //  )
  //);
}

void web_socket::on_timer( const boost::system::error_code& ec ) {
  if ( ec ) {
    switch ( ec.value() ) {
      case boost::asio::error::operation_aborted:
        // TODO: clean up
        break;
      default:
        break;
    }
  }
  else {
    if ( m_bSendHeartBeat ) {
      json::object heart_beat;
      heart_beat[ "id" ] = (int)EMessageId::HeartBeat;
      heart_beat[ "method" ] = "server.ping";
      heart_beat[ "params" ] = json::array();

      auto sSerialized( json::serialize( heart_beat ) );

      std::cout << "ws.on_timer send " << sSerialized << std::endl;

      // Send the message, may need to fix this, may not need the strand
      asio::post(
        m_strand,
        [this,s=std::move(sSerialized)](){
          m_ws.async_write(
            asio::buffer( s ),
            beast::bind_front_handler(
              &web_socket::on_write_heart_beat,
              shared_from_this()
            )
          );
        } );

      m_timer.expires_after( boost::asio::chrono::seconds( nHeartBeatIntervalSeconds ) );
      m_timer.async_wait( std::bind( &web_socket::on_timer, this, std::placeholders::_1 ) );
    }
  }
}

void web_socket::on_write_heart_beat(
  beast::error_code ec,
  std::size_t bytes_transferred
) {
  boost::ignore_unused(bytes_transferred);

  if ( ec )
    return fail( ec, "ws.on_write_heart_beat" );
  else {
    //std::cout << "ws.on_write_heart_beat " << bytes_transferred << std::endl;
  }

  // Read a message into our buffer?
  //m_ws.async_read(
  //  m_buffer,
  //  beast::bind_front_handler(
  //    &web_socket::on_read_auth,
  //    shared_from_this()
  //  )
  //);
}

// unused at the moment
void web_socket::on_write_auth(
  beast::error_code ec,
  std::size_t bytes_transferred
) {
  boost::ignore_unused(bytes_transferred);

  if ( ec )
    return fail( ec, "ws.on_write_auth" );
  else {
    //std::cout << "ws.on_write_auth" << std::endl;
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

// unused at the moment
void web_socket::on_read_auth(
  beast::error_code ec,
  std::size_t bytes_transferred
) {
  boost::ignore_unused( bytes_transferred );

  if ( ec )
    return fail( ec, "ws.on_read_auth" );
  else {
    //std::cout << "ws.on_read_auth" << std::endl;

    // The make_printable() function helps print a ConstBufferSequence
    //std::cout << "ws.on_read_auth: " << beast::make_printable( m_buffer.data() ) << std::endl;

    std::string sMessage( beast::buffers_to_string( m_buffer.data() ) );
    //std::cout << "ws.on_read_auth: " << sMessage << std::endl;

    // TODO: will need to intercept id=1 for heart_beat

    m_bConnected = true;

    if ( m_fConnected ) m_fConnected( true );
    if ( m_fMessage ) m_fMessage( std::move( sMessage ) );
    m_buffer.clear();

    // wait for more reads
    m_ws.async_read(
      m_buffer,
      beast::bind_front_handler(
        &web_socket::on_read_listen,
        shared_from_this()
      )
    );
  }

}

// this is alpaca specific
/*
void web_socket::trade_updates( bool bEnable ) {

  json::object listen;
  listen[ "action" ] = "listen";
  if ( bEnable ) {
    listen[ "data" ].emplace_object()[ "streams" ] = "trade_updates";
  }
  else {
    listen[ "data" ].emplace_object()[ "streams" ].emplace_array();
  }

  //std::cout << "ws.listen: '" << listen << "'" << std::endl;

  // Send the message
  m_ws.async_write(
    asio::buffer( json::serialize( listen ) ),
    beast::bind_front_handler(
      &web_socket::on_write_listen,
      shared_from_this()
    )
  );
}
*/
// unused at the moment
void web_socket::on_write_listen(
  beast::error_code ec,
  std::size_t bytes_transferred
) {
  boost::ignore_unused(bytes_transferred);

  if ( ec )
    return fail(ec, "ws.on_write_listen");
  else {
    //std::cout << "ws.on_write_listen" << std::endl;
  }

  // already primed
  // Read a message into our buffer
  //m_ws.async_read(
  //  m_buffer,
  //  beast::bind_front_handler(
  //    &web_socket::on_read_listen,
  //    shared_from_this()
  //  )
  //);
}

void web_socket::on_read_listen(
  beast::error_code ec,
  std::size_t bytes_transferred
) {
  boost::ignore_unused( bytes_transferred );

  if ( ec )
    return fail( ec, "ws.on_read_listen" );
  else {
    //std::cout << "ws.on_read_listen" << std::endl;

    // The make_printable() function helps print a ConstBufferSequence
    //std::cout << "ws.on_read_listen: " << beast::make_printable( m_buffer.data() ) << std::endl;

    std::string sMessage( beast::buffers_to_string( m_buffer.data() ) );
    //std::cout << "ws.on_read_listen: " << sMessage << std::endl;

    if ( m_fMessage ) m_fMessage( std::move( sMessage ) );
    m_buffer.clear();

    if ( m_bConnected ) {
      // wait for more reads
      //std::cout << "ws.on_read_listen: start again" << std::endl;
      m_ws.async_read(
        m_buffer,
        beast::bind_front_handler(
          &web_socket::on_read_listen,
          shared_from_this()
        )
      );
    }
  }

}

void web_socket::disconnect() {

  m_bSendHeartBeat = false;
  m_timer.cancel();

  // Close the WebSocket connection
  m_ws.async_close(
    websocket::close_code::normal,
    beast::bind_front_handler(
      &web_socket::on_close,
      shared_from_this()
    )
  );
}

void web_socket::on_close(beast::error_code ec) {
  if( ec ) {
    return fail(ec, "ws.on_close");
  }
  else {
    std::cout << "ws.on_close" << std::endl;
  }

  m_bConnected = false;
  if ( m_fDisconnected ) m_fDisconnected();

  // If we get here then the connection is closed gracefully

}

void web_socket::StartTradeWatch( const std::string& sSymbol ) {

  json::object subscribe_trade;
  subscribe_trade[ "id" ] = (int)EMessageId::StartTradeWatch;
  subscribe_trade[ "method" ] = "trade.subscribe";
  subscribe_trade[ "params" ] = { sSymbol };

  auto sSerialized( json::serialize( subscribe_trade ) );

  std::cout << "ws.StartTradeWatch send " << sSerialized << std::endl;

  asio::post(
    m_strand,
    [this,s=std::move(sSerialized)](){
      m_ws.async_write(
        asio::buffer( s ),
        beast::bind_front_handler(
          &web_socket::on_write_subscribe_trade,
          shared_from_this()
        )
      );
    } );

}

void web_socket::on_write_subscribe_trade(
  beast::error_code ec,
  std::size_t bytes_transferred
) {
  boost::ignore_unused(bytes_transferred);

  if ( ec )
    return fail( ec, "ws.on_write_subscribe_trade" );
  else {
    //std::cout << "ws.on_write_subscribe_trade " << bytes_transferred << std::endl;
  }
}


void web_socket::StopTradeWatch( const std::string& sSymbol ) {

  json::object un_subscribe_trade;
  un_subscribe_trade[ "id" ] = (int)EMessageId::StopTradeWatch;
  un_subscribe_trade[ "method" ] = "trade.unsubscribe";
  un_subscribe_trade[ "params" ] = { sSymbol };

  const std::string sSerialized( json::serialize( un_subscribe_trade ) );

  std::cout << "ws.StopTradeWatch send " << sSerialized << std::endl;

  asio::post(
    m_strand,
    [this,s=std::move(sSerialized)](){
      m_ws.async_write(
        asio::buffer( s ),
        beast::bind_front_handler(
          &web_socket::on_write_un_subscribe_trade,
          shared_from_this()
        )
      );
    } );
}

void web_socket::on_write_un_subscribe_trade(
  beast::error_code ec,
  std::size_t bytes_transferred
) {
  boost::ignore_unused(bytes_transferred);

  if ( ec )
    return fail( ec, "ws.on_write_un_subscribe_trade" );
  else {
    //std::cout << "ws.on_write_un_subscribe_trade " << bytes_transferred << std::endl;
  }
}



} // namespace session
} // namespace phemex
} // namespace tf
} // namespace ou

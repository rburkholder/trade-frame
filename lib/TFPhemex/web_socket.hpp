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
 * File:    web_socket.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFPhemex
 * Created: June 6, 2022 15:01
 */

#pragma once

#include <memory>
#include <string>
#include <atomic>
#include <functional>

#include <boost/beast/ssl.hpp>
#include <boost/beast/core.hpp>

#include <boost/beast/websocket.hpp>

namespace asio  = boost::asio;      // from <boost/asio.hpp>
namespace ssl   = asio::ssl;        // from <boost/asio/ssl.hpp>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>

using tcp = boost::asio::ip::tcp;   // from <boost/asio/ip/tcp.hpp>

namespace ou {
namespace tf {
namespace phemex {
namespace session {

// https://www.boost.org/doc/libs/1_79_0/libs/beast/example/websocket/client/async-ssl/websocket_client_async_ssl.cpp

// Sends a WebSocket message and prints the response
class web_socket : public std::enable_shared_from_this<web_socket>
{
public:

  // Resolver and socket require an io_context
  explicit web_socket( asio::io_context&, ssl::context& );
  virtual ~web_socket();

  using fConnected_t = std::function<void(bool)>;
  using fDisconnected_t = std::function<void()>;
  using fMessage_t = std::function<void(std::string&&)>;

  // Start the asynchronous operation
  void connect(
    const std::string& host,
    const std::string& port,
    fConnected_t&&,
    fDisconnected_t&&,
    fMessage_t&&
  );
  void disconnect();

private:

  bool m_bConnected;
  std::atomic_uint64_t m_id; // used for incrementing message id in messages

  bool m_bSendHeartBeat;
  boost::asio::deadline_timer m_timer;

  tcp::resolver m_resolver;
  websocket::stream<
    beast::ssl_stream<beast::tcp_stream>> m_ws;
  beast::flat_buffer m_buffer;

  std::string m_host;

  fConnected_t m_fConnected;
  fDisconnected_t m_fDisconnected;
  fMessage_t m_fMessage;

  void on_resolve(
    beast::error_code ec,
    tcp::resolver::results_type results
  );

  void on_connect(beast::error_code, tcp::resolver::results_type::endpoint_type );

  void on_ssl_handshake( beast::error_code );
  void on_handshake( beast::error_code );

  void on_timer( const boost::system::error_code& );
  void on_write_heart_beat(
    beast::error_code,
    std::size_t bytes_transferred
  );

  // m_fConnected call back on completion - needs positive & negative calc
  void on_write_auth(
    beast::error_code,
    std::size_t bytes_transferred
  );

  void on_read_auth(
    beast::error_code,
    std::size_t bytes_transferred
  );

  // use callback for completion?
  void on_write_listen(
    beast::error_code,
    std::size_t bytes_transferred
  );

  void on_read_listen(
    beast::error_code,
    std::size_t bytes_transferred
  );

  void on_close( beast::error_code );

};

} // namespace session
} // namespace phemex
} // namespace tf
} // namespace ou

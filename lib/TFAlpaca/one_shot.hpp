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

#pragma once

#include <memory>
#include <string>

#include <boost/beast/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace asio  = boost::asio;      // from <boost/asio.hpp>
namespace ssl   = asio::ssl;        // from <boost/asio/ssl.hpp>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http  = beast::http;      // from <boost/beast/http.hpp>

using tcp = boost::asio::ip::tcp;   // from <boost/asio/ip/tcp.hpp>

namespace ou {
namespace tf {
namespace alpaca {
namespace session {

// https://www.boost.org/doc/libs/1_79_0/libs/beast/example/http/client/async-ssl/http_client_async_ssl.cpp
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
    ssl::context& ssl_ctx
  );

  // Start the asynchronous operation
  void run(
    const std::string& sHost,
    const std::string& sPort,
    const std::string& sTarget,
    int version,
    const std::string& sAlpacaKey,
    const std::string& sAlpacaSecret
  );

private:

  void on_resolve(
    beast::error_code ec,
    tcp::resolver::results_type results
  );

  void on_connect( beast::error_code ec, tcp::resolver::results_type::endpoint_type );

  void on_handshake( beast::error_code ec );

  void on_write(
    beast::error_code ec,
    std::size_t bytes_transferred
  );
  void on_read( beast::error_code ec, std::size_t bytes_transferred );

  void on_shutdown( beast::error_code ec );

};

} // namespace session
} // namespace alpaca
} // namespace tf
} // namespace ou

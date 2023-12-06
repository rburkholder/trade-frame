/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * Project: lib/Telegram
 * Created: February 17, 2023 10:02:25
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

namespace telegram {
namespace bot {
namespace session {

// https://www.boost.org/doc/libs/1_79_0/libs/beast/example/http/client/async-ssl/http_client_async_ssl.cpp
class one_shot : public std::enable_shared_from_this<one_shot> {
public:

  explicit one_shot(
    asio::any_io_executor ex,
    ssl::context& ssl_ctx
  );
  virtual ~one_shot();

  using fDone_t = std::function<void(bool,int,const std::string&)>; // false, not ok; true, fine

  void run(
    const std::string& sHost
  , const std::string& sPort
  , const std::string& sTarget
  , int version
  , const std::string& sTelegramToken
  );

  void get(
    const std::string& sHost
  , const std::string& sPort
  , const std::string& sTelegramToken
  , const std::string& sTarget
  , fDone_t&&
  );

  void get(
    const std::string& sHost
  , const std::string& sPort
  , const std::string& sTelegramToken
  , const std::string& sTarget
  , const std::string& sBody
  , fDone_t&&
  );

  void post(
    const std::string& sHost
  , const std::string& sPort
  , const std::string& sTelegramToken
  , const std::string& sTarget
  , const std::string& sBody
  , fDone_t&&
  );

  void patch(
    const std::string& sHost
  , const std::string& sPort
  , const std::string& sTelegramToken
  , const std::string& sTarget
  , const std::string& sBody
  , fDone_t&&
  );

  void delete_(
    const std::string& sHost
  , const std::string& sPort
  , const std::string& sTelegramToken
  , const std::string& sTarget
  , fDone_t&&
  );

private:

  tcp::resolver m_resolver;
  beast::flat_buffer m_buffer; // (Must persist between reads)
  beast::ssl_stream<beast::tcp_stream> m_stream;

  http::request<http::empty_body> m_request_empty; // one or the other
  http::request<http::string_body> m_request_body; // one or the other

  http::response<http::string_body> m_response;
  http::response_parser<http::string_body> m_parser;

  using fWriteRequest_t = std::function<void()>;
  fWriteRequest_t m_fWriteRequest;

  fDone_t m_fDone;

  void on_resolve( beast::error_code, tcp::resolver::results_type );

  void on_connect( beast::error_code, tcp::resolver::results_type::endpoint_type );

  void on_handshake( beast::error_code );

  void write_empty();
  void write_body();

  void on_write( beast::error_code, std::size_t bytes_transferred );
  void on_read( beast::error_code, std::size_t bytes_transferred );

  void on_shutdown( beast::error_code ec );

};

} // namespace session
} // namespace bot
} // namespace telegram

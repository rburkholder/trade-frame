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

// TODO: look at http return codes

#include <iostream>

#include <boost/beast/version.hpp>

#include "one_shot.hpp"

namespace telegram {
namespace bot {
namespace session {

namespace { // anonymous

const static int nVersion( 11 );
const static std::string sUserAgent( "ounl.tradeframe/1.0" );

// Report a failure
void fail( beast::error_code ec, char const* what ) {
  std::cerr << what << ": " << ec.message() << "\n";
}

} // namespace anonymous

one_shot::one_shot(
  asio::any_io_executor ex,
  ssl::context& ssl_ctx
)
: m_resolver( ex )
, m_stream( ex, ssl_ctx )
, m_fWriteRequest( nullptr )
, m_fDone( nullptr )
{
  //std::cout << "telegram_bot::one_shot construction" << std::endl; // ensuring proper timing of handling

  // Allow for an unlimited body size
  m_parser.body_limit( ( std::numeric_limits<std::uint64_t>::max )() );
  }

one_shot::~one_shot() {
  //std::cout << "telegram_bot::one_shot destruction" << std::endl;  // ensuring proper timing of handling
  //m_stream.shutdown();  // doesn't like this
  m_buffer.clear();
  m_response.clear();
  m_fWriteRequest = nullptr;
  m_fDone = nullptr;
}

void one_shot::run(
  const std::string& sHost
, const std::string& sPort
, const std::string& sTarget
, int version
, const std::string& sTelegramToken
) {
  // Set SNI Hostname (many hosts need this to handshake successfully)
  if( !SSL_set_tlsext_host_name( m_stream.native_handle(), sHost.c_str() ) )
  {
    beast::error_code ec{ static_cast<int>( ::ERR_get_error()), asio::error::get_ssl_category() };
    std::cerr << ec.message() << "\n";
    return;
  }

  // Set up an HTTP GET request message
  m_request_empty.version( version );
  m_request_empty.method( http::verb::get );
  m_request_empty.set( http::field::host, sHost );
  //request_.set( http::field::user_agent, BOOST_BEAST_VERSION_STRING );
  m_request_empty.set( http::field::user_agent, sUserAgent );

  m_request_empty.target( sTarget );
  //req_.body() = json::serialize( jv );
  //req_.prepare_payload();

  m_fWriteRequest = [this](){ write_empty(); };
  m_fDone = []( bool, const std::string& ){};

  // Look up the domain name
  m_resolver.async_resolve(
    sHost, sPort,
    beast::bind_front_handler(
      &one_shot::on_resolve,
      shared_from_this()
    )
  );
}

void one_shot::get(
  const std::string& sHost
, const std::string& sPort
, const std::string& sTelegramToken
, const std::string& sTarget
, fDone_t&& fDone
) {

  m_fDone = std::move( fDone );
  assert( m_fDone );

  // Set SNI Hostname (many hosts need this to handshake successfully)
  if( !SSL_set_tlsext_host_name( m_stream.native_handle(), sHost.c_str() ) )
  {
    beast::error_code ec{ static_cast<int>( ::ERR_get_error()), asio::error::get_ssl_category() };
    std::cerr << ec.message() << "\n";
    m_fDone( false, ec.message() );
    return;
  }

  // Set up an HTTP GET request message
  m_request_empty.version( nVersion );
  m_request_empty.method( http::verb::get );
  m_request_empty.set( http::field::host, sHost );
  m_request_empty.set( http::field::user_agent, sUserAgent );

  //m_request_empty.target( sTarget );
  const std::string s( "/bot" + sTelegramToken + "/" + sTarget );
  //std::cout << "get request: '" << s << "'" << std::endl;
  m_request_empty.target( s );

  m_fWriteRequest = [this](){ write_empty(); };

  // Look up the domain name
  m_resolver.async_resolve(
    sHost, sPort,
    beast::bind_front_handler(
      &one_shot::on_resolve,
      shared_from_this()
    )
  );
}

void one_shot::get(
  const std::string& sHost
, const std::string& sPort
, const std::string& sTelegramToken
, const std::string& sCommand
, const std::string& sBody
, fDone_t&& fDone
) {

  m_fDone = std::move( fDone );
  assert( m_fDone );

  // Set SNI Hostname (many hosts need this to handshake successfully)
  if( !SSL_set_tlsext_host_name( m_stream.native_handle(), sHost.c_str() ) )
  {
    beast::error_code ec{ static_cast<int>( ::ERR_get_error()), asio::error::get_ssl_category() };
    std::cerr << ec.message() << "\n";
    m_fDone( false, ec.message() );
    return;
  }

  // Set up an HTTP GET request message
  m_request_body.version( nVersion );
  m_request_body.method( http::verb::get );
  m_request_body.set( http::field::host, sHost );
  m_request_body.set( http::field::user_agent, sUserAgent );
  m_request_body.set( http::field::content_type, "application/json" );

  const std::string sTarget( "/bot" + sTelegramToken + "/" + sCommand );
  //std::cout << "get request: '" << s << "'" << std::endl;
  m_request_body.target( sTarget );

  m_request_body.body() = sBody;
  m_request_body.prepare_payload();

  m_fWriteRequest = [this](){ write_body(); };

  // Look up the domain name
  m_resolver.async_resolve(
    sHost, sPort,
    beast::bind_front_handler(
      &one_shot::on_resolve,
      shared_from_this()
    )
  );
}

void one_shot::post(
  const std::string& sHost
, const std::string& sPort
, const std::string& sTelegramToken
, const std::string& sCommand
, const std::string& sBody
, fDone_t&& fDone
) {

  m_fDone = std::move( fDone );
  assert( m_fDone );

  // Set SNI Hostname (many hosts need this to handshake successfully)
  if( !SSL_set_tlsext_host_name( m_stream.native_handle(), sHost.c_str() ) )
  {
    beast::error_code ec{ static_cast<int>( ::ERR_get_error()), asio::error::get_ssl_category() };
    std::cerr << ec.message() << "\n";
    m_fDone( false, ec.message() );
    return;
  }

  // Set up an HTTP GET request message
  m_request_body.version( nVersion );
  m_request_body.method( http::verb::post );
  m_request_body.set( http::field::host, sHost );
  m_request_body.set( http::field::user_agent, sUserAgent );
  m_request_body.set( http::field::content_type, "application/json" );

  const std::string sTarget( "/bot" + sTelegramToken + "/" + sCommand );
  //std::cout << "post target: '" << sTarget << "'" << std::endl;
  m_request_body.target( sTarget );

  m_request_body.body() = sBody;
  m_request_body.prepare_payload();

  //std::cout << m_request_body << std::endl;

  m_fWriteRequest = [this](){ write_body(); };

  // Look up the domain name
  m_resolver.async_resolve(
    sHost, sPort,
    beast::bind_front_handler(
      &one_shot::on_resolve,
      shared_from_this()
    )
  );
}

void one_shot::delete_(
  const std::string& sHost
, const std::string& sPort
, const std::string& sTelegramToken
, const std::string& sTarget
, fDone_t&& fDone
) {

  m_fDone = std::move( fDone );
  assert( m_fDone );

  // Set SNI Hostname (many hosts need this to handshake successfully)
  if( !SSL_set_tlsext_host_name( m_stream.native_handle(), sHost.c_str() ) )
  {
    beast::error_code ec{ static_cast<int>( ::ERR_get_error()), asio::error::get_ssl_category() };
    std::cerr << ec.message() << "\n";
    m_fDone( false, ec.message() );
    return;
  }

  // Set up an HTTP GET request message
  m_request_empty.version( nVersion );
  m_request_empty.method( http::verb::delete_ );
  m_request_empty.set( http::field::host, sHost );
  //request_.set( http::field::user_agent, BOOST_BEAST_VERSION_STRING );
  m_request_empty.set( http::field::user_agent, sUserAgent );

  m_request_empty.target( sTarget );

  m_fWriteRequest = [this](){ write_empty(); };

  // Look up the domain name
  m_resolver.async_resolve(
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
  if ( ec ) {
    fail( ec, "os.on_resolve");
    m_fDone( false, "os.on_resolve" );
  }
  else {
    // Set a timeout on the operation
    beast::get_lowest_layer( m_stream ).expires_after( std::chrono::seconds( 15 ) );

    //std::cout << "os.on_resolve" << std::endl;

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer( m_stream ).async_connect(
      results,
      beast::bind_front_handler(
        &one_shot::on_connect,
        shared_from_this() )
      );
  }
}

void one_shot::on_connect( beast::error_code ec, tcp::resolver::results_type::endpoint_type et ) {
  if ( ec ) {
    fail( ec, "os.on_connect" );
    m_fDone( false, "os.on_connect" );
  }
  else {

    //std::cout << "os.on_connect" << std::endl;

    // Perform the SSL handshake
    m_stream.async_handshake(
      ssl::stream_base::client,
      beast::bind_front_handler(
        &one_shot::on_handshake,
        shared_from_this()
      )
    );
  }
}

void one_shot::on_handshake( beast::error_code ec ) {

  if ( ec ) {
    fail( ec, "os.on_handshake" );
    m_fDone( false, "os.on_handshake" );
  }
  else {

    //std::cout << "os.ssl_handshake" << std::endl;

    // Set a timeout on the operation
    beast::get_lowest_layer( m_stream ).expires_after( std::chrono::seconds( 15 ) );

    assert( m_fWriteRequest );
    m_fWriteRequest();
  }
}

void one_shot::write_empty() {

  //std::cout << "os.write_empty" << std::endl;

  // Send the HTTP request to the remote host
  http::async_write( m_stream, m_request_empty,
    beast::bind_front_handler(
      &one_shot::on_write,
      shared_from_this()
    )
  );
}

void one_shot::write_body() {

  //std::cout << "os.write_body" << std::endl;

  // Send the HTTP request to the remote host
  http::async_write( m_stream, m_request_body,
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

  if ( ec ) {
    fail( ec, "os.on_write" );
    m_fDone( false, "os.on_write" );
  }
  else {

    //std::cout << "os.on_write" << std::endl;

    // Receive the HTTP response
    http::async_read(
//      m_stream, m_buffer, m_response,
      m_stream, m_buffer, m_parser,
      beast::bind_front_handler(
        &one_shot::on_read,
        shared_from_this()
      )
    );
  }
}

/*
   If more parser work required;

   initial inspiration:
   https://stackoverflow.com/questions/50348516/boost-beast-message-with-body-limit

   docs
   https://www.boost.org/doc/libs/1_79_0/libs/beast/doc/html/beast/using_http/parser_stream_operations.html

   https://www.vitaltrades.com/2018/12/28/reading-an-http-stream-using-c-boost-beast/ refers to the following:
   https://github.com/AndrewAMD/blog/tree/master/oanda_demo_stream

  handle chunks directly if things go horibbly wrong
   https://www.boost.org/doc/libs/1_79_0/libs/beast/doc/html/beast/ref/boost__beast__http__basic_parser.html
   https://www.boost.org/doc/libs/1_79_0/libs/beast/doc/html/beast/ref/boost__beast__http__async_read_some.html

*/

void one_shot::on_read( beast::error_code ec, std::size_t bytes_transferred ) {

  boost::ignore_unused( bytes_transferred );

  if ( ec ) {
    fail( ec, "os.on_read" );
    m_fDone( false, "os.on_read" );
  }
  else {

    //std::cout << "os.on_read" << std::endl;
    //std::cout << "get():" << m_parser.get() << std::endl;
    auto body = m_parser.get().body();
    //std::cout << "body():" << m_parser.get().body() << std::endl;

    //m_fDone( true, m_response.body() );
    m_fDone( true, body );
    // Set a timeout on the operation
    beast::get_lowest_layer( m_stream ).expires_after( std::chrono::seconds( 15 ) );

    // Gracefully close the stream - can the stream be re-used?
    m_stream.async_shutdown(
      beast::bind_front_handler(
        &one_shot::on_shutdown,
        shared_from_this()
      )
    );
  }
}

void one_shot::on_shutdown( beast::error_code ec ) {
  //std::cout << "os.on_shutdown" << std::endl;
  if ( ec == asio::error::eof ) {
      // Rationale:
      // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
      ec = {};
  }
  if ( ec ) {
    //return fail( ec, "os.shutdown" );
  }

  // If we get here then the connection is closed gracefully
}

} // namespace session
} // namespace bot
} // namespace telegram

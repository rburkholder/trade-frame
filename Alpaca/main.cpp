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

#include <boost/asio/strand.hpp>

#include <TFAlpaca/root_certificates.hpp>

#include <TFAlpaca/one_shot.hpp>
#include <TFAlpaca/web_socket.hpp>

#include "Config.hpp"

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
    auto os = std::make_shared<ou::tf::alpaca::session::one_shot>(
      asio::make_strand( ioc ),
      ssl_ctx
      );
    os->run(
      choices.m_sAlpacaDomain, sPort,
      sTarget,
      version,
      choices.m_sAlpacaKey, choices.m_sAlpacaSecret
    );

    auto ws = std::make_shared<ou::tf::alpaca::session::web_socket>(
      ioc,
      ssl_ctx
      );
    ws->connect(
      choices.m_sAlpacaDomain, sPort,
      choices.m_sAlpacaKey, choices.m_sAlpacaSecret,
      [ws](bool){
        ws->trade_updates( true );
        //ws->trade_updates( false );
        //ws->disconnect();
        }
    );

    // Run the I/O service. The call will return when
    // the get operation is complete.
    ioc.run();

    return EXIT_SUCCESS;
}
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
 * File:    Bot.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/Telegram
 * Created: February 17, 2023 18:22:02
 */

#include <iostream>
#include <functional>

#include <boost/asio/strand.hpp>

#include "one_shot.hpp"
// this needs to be factored out properly
#include "root_certificates.hpp"

#include "Bot.hpp"

namespace telegram {

Bot::Bot( const std::string& sToken )
: m_sToken( sToken )
, m_ssl_context( ssl::context::tlsv12_client )
{
  // This holds the root certificate used for verification
  // NOTE: this needs to be fixed, based upon comments in the include header
  ou::load_root_certificates( m_ssl_context );

  // Verify the remote server's certificate
  m_ssl_context.set_verify_mode( ssl::verify_peer );

  m_pWorkGuard = std::make_unique<work_guard_t>( asio::make_work_guard( m_io ) );
  m_thread = std::move( std::thread( [this](){ m_io.run(); } ) );
}

Bot::~Bot() {
  m_pWorkGuard.reset();
  if ( m_thread.joinable() ) {
    m_thread.join();
  }

}

void Bot::GetMe() {
  auto request = std::make_shared<bot::session::one_shot>( asio::make_strand( m_io ), m_ssl_context );
  request->get(
    "api.telegram.org"
  , "443"
  , m_sToken
  , "getMe"
  , [this]( bool bStatus, const std::string& message ){
      std::cout << message << std::endl;
    }
  );
}

} // namespace telegram

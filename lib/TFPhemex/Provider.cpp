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
 * File:    Provider.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFPhemex
 * Created: July 24, 2022 12:19
 */

#include "root_certificates.hpp"

#include "Provider.hpp"

namespace ou {
namespace tf {
namespace phemex {

Provider::Provider()
: ProviderInterface<Provider,Symbol>()
, m_ssl_context( ssl::context::tlsv12_client )
{
  m_sName = "phemex"; // this needs to match provider used in the database
  m_nID = keytypes::EProviderPhemex;
  //m_bProvidesBrokerInterface = true;
  m_bProvidesQuotes = true;
  m_bProvidesTrades = true;

  if ( 0 == GetThreadCount() ) {
    SetThreadCount( 1 ); // need at least one thread for websocket processing
  }

  // This holds the root certificate used for verification
  // NOTE: this needs to be fixed, based upon comments in the include header
  ou::load_root_certificates( m_ssl_context );

  // Verify the remote server's certificate
  m_ssl_context.set_verify_mode( ssl::verify_peer );

}

Provider::~Provider() {
  if ( EState::start != m_state ) {
    Disconnect();
  }
}

void Provider::Set( const std::string& sHost, const std::string& sKey, const std::string& sSecret ) {
  m_sHost = sHost;
  m_sPort = "443";
  m_sKeyId = sKey;
  m_sSecret = sSecret;
}

Provider::pSymbol_t Provider::NewCSymbol( pInstrument_t pInstrument ) {
  pSymbol_t pSymbol( new Symbol( pInstrument->GetInstrumentName( ID() ), pInstrument ) );
  inherited_t::AddCSymbol( pSymbol );
  return pSymbol;
}

} // namespace phemex
} // namespace tf
} // namespace ou

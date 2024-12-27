/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    InstrumentEngine.hpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/12/24 16:58:15
 */

#include "InstrumentEngine.hpp"

namespace ou {
namespace tf {
namespace engine {

Instrument::Instrument( pProvider_t pExec, pProvider_t pData )
: m_pExec( std::move( pExec ) ) // IB
, m_pData( std::move( pData ) ) // IQF
{

  switch ( m_pExec->ID() ) {
    case ou::tf::keytypes::EProviderIB:
      m_pIB = ou::tf::ib::TWS::Cast( m_pExec );
      break;
    case ou::tf::keytypes::EProviderIQF:
       m_pIQ = ou::tf::iqfeed::Provider::Cast( m_pExec );
      break;
    default:
      assert( false ); // need one of IB or IQF
  }

  switch ( m_pData->ID() ) {
    //case ou::tf::keytypes::EProviderIB:
    //  m_pIB = ou::tf::ib::TWS::Cast( m_pExec );
    //  break;
    case ou::tf::keytypes::EProviderIQF:
      m_pIQ = ou::tf::iqfeed::Provider::Cast( m_pData );
      break;
    default:
      assert( false ); // need the IQF provider
  }

  if ( m_pIB ) {
    m_pComposeInstrument = std::make_unique<ou::tf::ComposeInstrument>( m_pIQ, m_pIB, [](){} );
  }
  else {
    m_pComposeInstrument = std::make_unique<ou::tf::ComposeInstrument>( m_pIQ, [](){} );
  }

}

Instrument::~Instrument() {
  m_pComposeInstrument.reset();

  m_pIQ.reset();
  m_pData.reset();

  m_pIB.reset();
  m_pExec.reset();
}

void Instrument::Compose( const std::string& s, fInstrument_t&& f ) {
  assert( m_pComposeInstrument );
  m_pComposeInstrument->Compose( s, std::move( f ) );
}

Instrument::pWatch_t Instrument::MakeWatch( pInstrument_t p ) {
  return std::make_shared<ou::tf::Watch>( p, m_pIQ );
}

} // namespace engine
} // namespace tf
} // namespace ou
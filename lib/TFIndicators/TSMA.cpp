/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include <stdexcept>

#include "TSMA.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

TSMA::TSMA( CPrices& series, time_duration dt, unsigned int nInf, unsigned int nSup )
  : m_seriesSource( series ), m_dtTimeRange( dt ), m_nInf( nInf ), m_nSup( nSup ), m_dblRecentMA( 0.0 )
{
  // uses tau prime with 2 tau / ( nsup + ninf )
  assert( 1 <= nInf );
  assert( nInf <= nSup );
  throw std::runtime_error( "not implemented" );
}

TSMA::TSMA( CPrices& series, time_duration dt, unsigned int n )
  : m_seriesSource( series ), m_dtTimeRange( dt ), m_nInf( 1 ), m_nSup( n ), m_dblRecentMA( 0.0 )
{
  // uses tau prime with 2 tau / ( n + 1 )
  assert( 1 <= n );
}

TSMA::~TSMA(void) {
  if ( 0 < m_vEMA.size() ) {
    m_vEMA[ m_nSup ]->OnAppend.Remove( MakeDelegate( this, &TSMA::HandleUpdate ) );
    for ( unsigned int ix = m_nSup; 0 != ix; --ix ) {
      delete( m_vEMA[ ix ] );
      m_vEMA[ ix ] = 0;
    }
  }
}

void TSMA::Init( void ) {
  m_vEMA.resize( m_nSup + 1 ); // first vector element unused
  m_vEMA[ 0 ] = 0;
  m_vEMA[ 1 ] = new TSEMA<CPrice>( m_seriesSource, m_dtTimeRange ); 
  for ( unsigned int ix = 2; ix <= m_nSup; ++ix ) {
    m_vEMA[ ix ] = new TSEMA<CPrice>( *m_vEMA[ ix - 1 ], microseconds( 2 * m_dtTimeRange.total_microseconds() / ( ix + 1 ) ) );
  }
  m_vEMA[ m_nSup ]->OnAppend.Add( MakeDelegate( this, &TSMA::HandleUpdate ) );
}

void TSMA::HandleUpdate( const CPrice& price ) {
  double ma = 0.0;
  for ( unsigned int ix = 1; ix <= m_nSup; ++ix ) {
    ma += m_vEMA[ ix ]->GetEMA();
  }
  CPrices::Append( CPrice( price.DateTime(), ma / m_nSup ) );
}

} // namespace hf
} // namespace tf
} // namespace ou

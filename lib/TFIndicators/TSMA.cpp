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

#include "stdafx.h"

#include <stdexcept>

#include "TSMA.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

TSMA::TSMA( Prices& series, time_duration td, unsigned int nInf, unsigned int nSup )
  : m_seriesSource( series ), m_tdTimeRange( td ), m_nInf( nInf ), m_nSup( nSup ), m_dblRecentMA( 0.0 )
{
  // uses tau prime with 2 tau / ( nsup + ninf )
  assert( 1 <= nInf );
  assert( nInf <= nSup );
  throw std::runtime_error( "not implemented" );
}

TSMA::TSMA( Prices& series, time_duration td, unsigned int n )
  : m_seriesSource( series ), m_tdTimeRange( td ), m_nInf( 1 ), m_nSup( n ), m_dblRecentMA( 0.0 )
{
  // uses tau prime with 2 tau / ( n + 1 )
  assert( 1 <= n );
  Initialize();
}

TSMA::TSMA( const TSMA& rhs ) 
  : m_tdTimeRange( rhs.m_tdTimeRange ), m_nInf( rhs.m_nInf ), m_nSup( rhs.m_nSup ),
  m_dblRecentMA( rhs.m_dblRecentMA ), m_seriesSource( rhs.m_seriesSource )
{
  Initialize();
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

void TSMA::Initialize( void ) {
  boost::posix_time::time_duration td( microseconds( ( 2 * m_tdTimeRange.total_microseconds() ) / ( m_nSup + 1 ) ) );
  m_vEMA.resize( m_nSup + 1 ); // first vector element unused
  m_vEMA[ 0 ] = 0;
  //m_vEMA[ 1 ] = new TSEMA<Price>( m_seriesSource, m_dtTimeRange ); 
  m_vEMA[ 1 ] = new TSEMA<Price>( m_seriesSource, td ); 
  for ( unsigned int ix = 2; ix <= m_nSup; ++ix ) {
    //m_vEMA[ ix ] = new TSEMA<Price>( *m_vEMA[ ix - 1 ], microseconds( ( 2 * m_tdTimeRange.total_microseconds() ) / ( ix + 1 ) ) );
    m_vEMA[ ix ] = new TSEMA<Price>( *m_vEMA[ ix - 1 ], td );
    //m_vEMA[ ix ] = new TSEMA<Price>( m_seriesSource, microseconds( ( 2 * m_dtTimeRange.total_microseconds() ) / ( ix + 1 ) ) );
  }
  for ( unsigned int ix = 1; ix <= m_nSup; ++ix ) {
    m_vEMA[ ix ]->DisableAppend();
  }
  Prices::DisableAppend();
  m_vEMA[ m_nSup ]->OnAppend.Add( MakeDelegate( this, &TSMA::HandleUpdate ) );
}

void TSMA::HandleUpdate( const Price& price ) {
  double ma = 0.0;
  for ( unsigned int ix = 1; ix <= m_nSup; ++ix ) {
    ma += m_vEMA[ ix ]->GetEMA();
  }
  m_dblRecentMA = ma / m_nSup;
  Prices::Append( Price( price.DateTime(), m_dblRecentMA ) );
}

} // namespace hf
} // namespace tf
} // namespace ou

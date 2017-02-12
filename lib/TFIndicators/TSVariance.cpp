/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "TSVariance.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

TSVariance::TSVariance( Prices& series, time_duration td, unsigned int n, double p1, double p2 ) 
  : m_seriesSource( series ), m_tdTimeRange( td ), m_n( n ), m_p1( p1 ), m_p2( p2 ),
    m_ma2( m_dummy, td, n )
{
  assert( 0 < m_n );
  assert( 0.0 < m_p2 );
  Init();
}

TSVariance::TSVariance( const TSVariance& rhs ) 
  : m_tdTimeRange( rhs.m_tdTimeRange ), m_n( rhs.m_n ), m_p1( rhs.m_p1 ), m_p2( rhs.m_p2 ), m_z( rhs.m_z ),
  m_seriesSource( rhs.m_seriesSource ), m_ma2( m_dummy, m_tdTimeRange, m_n )
{
  Init();
}


TSVariance::~TSVariance(void) {
  m_seriesSource.OnAppend.Remove( MakeDelegate( this, &TSVariance::HandleUpdate ) );
  m_pma1->OnAppend.Remove( MakeDelegate( this, &TSVariance::HandleMA1Update ) );
  m_ma2.OnAppend.Remove( MakeDelegate( this, &TSVariance::HandleMA2Update ) );
  delete m_pma1;
}

void TSVariance::Init( void ) {
  Prices::DisableAppend();
  m_seriesSource.OnAppend.Add( MakeDelegate( this, &TSVariance::HandleUpdate ) );
  m_pma1 = new TSMA( m_seriesSource, m_tdTimeRange, m_n );
  m_pma1->DisableAppend();
  m_pma1->OnAppend.Add( MakeDelegate( this, &TSVariance::HandleMA1Update ) );
  m_dummy.DisableAppend();
  m_ma2.OnAppend.Add( MakeDelegate( this, &TSVariance::HandleMA2Update ) );
  m_ma2.DisableAppend();
}

void TSVariance::HandleUpdate( const Price& price ) {
//  std::cout << "Update: " << price.Value();
  m_z = price.Value();
}

void TSVariance::HandleMA1Update( const Price& price ) {
  double t = m_z - price.Value();
  if ( 1.0 == m_p1 ) {
    m_dummy.Append( Price( price.DateTime(), std::abs( t ) ) );
  }
  else {
    if ( 2.0 == m_p1 ) {
//      std::cout << " MA1: " << t << "," << t * t;
      m_dummy.Append( Price( price.DateTime(), t * t ) );
    }
    else {
      m_dummy.Append( Price( price.DateTime(), std::pow( std::abs( t ), m_p1 ) ) );
    }
  }
}

void TSVariance::HandleMA2Update( const Price& price ) {
  if ( 1.0 == m_p2 ) {
    Append( price );
  }
  else {
    if ( 2.0 == m_p2 ) {
      double tmp = std::sqrt( price.Value() );
//      std::cout << " MA2: " << tmp << std::endl;
      Append( Price( price.DateTime(), tmp ) );
    }
    else {
      Append( Price( price.DateTime(), std::pow( price.Value(), 1.0 / m_p2 ) ) );
    }
  }
}

} // namespace hf
} // namespace tf
} // namespace ou

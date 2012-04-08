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

#include <math.h>

#include "TSSWRealizedVolatility.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

TSSWRealizedVolatility::TSSWRealizedVolatility( CPrices& prices, time_duration tdWindowWidth, double p )
  : m_dblSum( 0.0 ), m_dblP( p ), m_n( 0 ), m_dt( not_a_date_time ), m_tdScaledWidth( hours( 365 * 24 ) + hours( 6 ) ),
    TimeSeriesSlidingWindow<TSSWRealizedVolatility, CPrice>( prices, tdWindowWidth, 0 )
{
  CalcScaleFactor();
}

TSSWRealizedVolatility::~TSSWRealizedVolatility( void ) {
}

void TSSWRealizedVolatility::Add( const CPrice& price ) {
  m_dt = price.DateTime();
  double val( price.Price() );
  ++m_n;
  if ( 1.0 == m_dblP ) {
    m_dblSum += val;
  }
  else {
    if ( 2.0 == m_dblP ) {
      m_dblSum += val * val;
    }
    else {
      m_dblSum += std::pow( std::abs( val ), m_dblP );
    }
  }
}

void TSSWRealizedVolatility::Expire( const CPrice& price ) {
  --m_n;
  double val( price.Price() );
  --m_n;
  if ( 1.0 == m_dblP ) {
    m_dblSum -= val;
  }
  else {
    if ( 2.0 == m_dblP ) {
      m_dblSum -= val * val;
    }
    else {
      m_dblSum -= std::pow( std::abs( val ), m_dblP );
    }
  }
}

void TSSWRealizedVolatility::PostUpdate( void ) {
  double result( 0.0 );
  if ( 1.0 == m_dblP ) {
    result = m_dblSum / m_n;
  }
  else {
    if ( 2.0 == m_dblP ) {
      result = std::sqrt( m_dblSum / m_n );
    }
    else {
      result = std::pow( m_dblSum / m_n, 1.0 / m_dblP );
    }
  }
  CPrices::Append( CPrice( m_dt, result * m_dblScaleFactor ) );
}

void TSSWRealizedVolatility::CalcScaleFactor( void ) {
  m_dblScaleFactor = 
    std::sqrt( 
    (double) m_tdScaledWidth.total_milliseconds() / 
    ( (double) TimeSeriesSlidingWindow<TSSWRealizedVolatility, CPrice>::WindowWidth().total_milliseconds() / m_n ) 
    );
}

} // namespace tf
} // namespace ou


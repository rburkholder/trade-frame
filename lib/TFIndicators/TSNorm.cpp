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

#include "TSNorm.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

TSNorm::TSNorm( Prices& series, time_duration dt, unsigned int n, double p ) 
  : m_seriesSource( series ), m_dtTimeRange( dt ), m_n( n ), m_p( p ), m_ma( *this, dt, n )
{
  m_seriesSource.OnAppend.Add( MakeDelegate( this, &TSNorm::HandleUpdate ) );
  m_ma.OnAppend.Add( MakeDelegate( this, &TSNorm::HandleMAUpdate ) );
}

TSNorm::TSNorm( const TSNorm& rhs ) 
  : m_dtTimeRange( rhs.m_dtTimeRange ), m_n( rhs.m_n ), m_p( rhs.m_p ), m_seriesSource( rhs.m_seriesSource ), 
  m_ma( *this, rhs.m_dtTimeRange, rhs.m_n )
{
  m_seriesSource.OnAppend.Add( MakeDelegate( this, &TSNorm::HandleUpdate ) );
  m_ma.OnAppend.Add( MakeDelegate( this, &TSNorm::HandleMAUpdate ) );
}

TSNorm::~TSNorm(void) {
  m_seriesSource.OnAppend.Remove( MakeDelegate( this, &TSNorm::HandleUpdate ) );
  m_ma.OnAppend.Remove( MakeDelegate( this, &TSNorm::HandleMAUpdate ) );
}

void TSNorm::HandleUpdate( const Price& price ) {
  if ( 1.0 == m_p ) {
    m_ma.Append( Price( price.DateTime(), std::abs( price.Value() ) ) );
  }
  else {
    if ( 2.0 == m_p ) {
      m_ma.Append( Price( price.DateTime(), price.Value() * price.Value() ) );
    }
    else {
      m_ma.Append( Price( price.DateTime(), std::pow( std::abs( price.Value() ), m_p ) ) ); 
    }
  }
}

void TSNorm::HandleMAUpdate( const Price& price ) {
  if ( 1.0 == m_p ) {
    Append( price );
  }
  else {
    if ( 2.0 == m_p ) {
      Append( Price( price.DateTime(), std::sqrt( price.Value() ) ) );
    }
    else {
      Append( Price( price.DateTime(), std::pow( price.Value(), 1.0 / m_p ) ) );
    }
  }
}

} // namespace hf
} // namespace tf
} // namespace ou

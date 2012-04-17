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

#include "TSNorm.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

TSNorm::TSNorm( CPrices& series, time_duration dt, unsigned int n, double p ) 
  : m_seriesSource( series ), m_dtTimeRange( dt ), m_n( n ), m_p( p ), m_ma( *this, dt, n )
{
  m_seriesSource.OnAppend.Add( MakeDelegate( this, &TSNorm::HandleUpdate ) );
  m_ma.OnAppend.Add( MakeDelegate( this, &TSNorm::HandleMAUpdate ) );
}

TSNorm::~TSNorm(void) {
  m_seriesSource.OnAppend.Remove( MakeDelegate( this, &TSNorm::HandleUpdate ) );
  m_ma.OnAppend.Remove( MakeDelegate( this, &TSNorm::HandleMAUpdate ) );
}

void TSNorm::HandleUpdate( const CPrice& price ) {
  if ( 1.0 == m_p ) {
    m_ma.Append( CPrice( price.DateTime(), std::abs( price.Price() ) ) );
  }
  else {
    if ( 2.0 == m_p ) {
      m_ma.Append( CPrice( price.DateTime(), price.Price() * price.Price() ) );
    }
    else {
      m_ma.Append( CPrice( price.DateTime(), std::pow( std::abs( price.Price() ), m_p ) ) ); 
    }
  }
}

void TSNorm::HandleMAUpdate( const CPrice& price ) {
  if ( 1.0 == m_p ) {
    Append( price );
  }
  else {
    if ( 2.0 == m_p ) {
      Append( CPrice( price.DateTime(), std::sqrt( price.Price() ) ) );
    }
    else {
      Append( CPrice( price.DateTime(), std::pow( price.Price(), 1.0 / m_p ) ) );
    }
  }
}

} // namespace hf
} // namespace tf
} // namespace ou

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
#include "TSDifferential.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

const double TSDifferential::m_gamma = 1.22208;
const double TSDifferential::m_beta = 0.65;
const double TSDifferential::m_alpha = 1.0 / ( TSDifferential::m_gamma * ( 8.0 * TSDifferential::m_beta - 3.0 ) );
  
TSDifferential::TSDifferential( CPrices& series, time_duration dt ) 
  : m_dtTimeRange( dt ), m_seriesSource( series ), m_bDerivative( false )
{
  Init();
}

TSDifferential::TSDifferential( CPrices& series, time_duration dt, double dblGammaDerivative, time_duration dtNormalization ) 
  : m_dtTimeRange( dt ), m_seriesSource( series ), m_bDerivative( true ),
    m_dtNormalization( dtNormalization ), m_dblGammaDerivative( dblGammaDerivative )
{
  assert( 0.0 != dblGammaDerivative );
  assert( 0 != dtNormalization.total_microseconds() );
  m_dblNormalization = (double) dt.total_microseconds() / (double) dtNormalization.total_microseconds();
  Init();
}

TSDifferential::~TSDifferential(void) {
  m_pema6->OnAppend.Remove( MakeDelegate( this, &TSDifferential::HandleTerm3Update ) );
  m_pema2->OnAppend.Remove( MakeDelegate( this, &TSDifferential::HandleTerm2Update ) );
  m_pema1->OnAppend.Remove( MakeDelegate( this, &TSDifferential::HandleTerm1Update ) );
  delete m_pema6;
  delete m_pema5;
  delete m_pema4;
  delete m_pema3;
  delete m_pema2;
  delete m_pema1;
}

void TSDifferential::Init( void ) {
  m_dtAlphaTau = microseconds( m_dtTimeRange.total_microseconds() * m_alpha );
  m_dtAlphaBetaTau = microseconds( m_dtTimeRange.total_microseconds() * m_alpha * m_beta );
  m_pema1 = new TSEMA<CPrice>( m_seriesSource, m_dtAlphaTau );
  m_pema1->OnAppend.Add( MakeDelegate( this, &TSDifferential::HandleTerm1Update ) );
  m_pema2 = new TSEMA<CPrice>( *m_pema1, m_dtAlphaTau );
  m_pema2->OnAppend.Add( MakeDelegate( this, &TSDifferential::HandleTerm2Update ) );
  m_pema3 = new TSEMA<CPrice>( m_seriesSource, m_dtAlphaBetaTau );
  m_pema4 = new TSEMA<CPrice>( *m_pema3, m_dtAlphaBetaTau );
  m_pema5 = new TSEMA<CPrice>( *m_pema4, m_dtAlphaBetaTau );
  m_pema6 = new TSEMA<CPrice>( *m_pema5, m_dtAlphaBetaTau );
  m_pema6->OnAppend.Add( MakeDelegate( this, &TSDifferential::HandleTerm3Update ) );
}

void TSDifferential::HandleTerm1Update( const CPrice& price ) {
  m_dblTerm1 = price.Price();
}

void TSDifferential::HandleTerm2Update( const CPrice& price ) {
  m_dblTerm2 = price.Price();
}

void TSDifferential::HandleTerm3Update( const CPrice& price ) {
  double differential = m_gamma * ( m_dblTerm1 + m_dblTerm2 - 2.0 * price.Price() );
  if ( m_bDerivative ) {
    double normalization = differential / m_dblNormalization;
    if ( 1.0 == m_dblGammaDerivative ) {
      Append( CPrice( price.DateTime(), normalization ) );
    }
    else {
      if( 0.5 == m_dblGammaDerivative ) {
        assert( 0.0 <= normalization );
        Append( CPrice( price.DateTime(), std::sqrt( normalization ) ) );
      }
      else {
        Append( CPrice( price.DateTime(), std::pow( normalization, m_dblGammaDerivative ) ) );
      }
    }
    
  }
  else {
    Append( CPrice( price.DateTime(), differential ) );
  }
  
}

} // namespace hf
} // namespace tf
} // namespace ou

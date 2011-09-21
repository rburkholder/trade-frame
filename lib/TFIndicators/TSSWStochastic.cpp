/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include "TSSWStochastic.h"


namespace ou { // One Unified
namespace tf { // TradeFrame

TSSWStochastic::TSSWStochastic(  CQuotes* quotes, long WindowSizeSeconds ) 
  : TimeSeriesSlidingWindow<TSSWStochastic, CQuote>( quotes, WindowSizeSeconds ),
    m_seconds( WindowSizeSeconds ), m_last( 0 ), m_k( 0 )
{
}

TSSWStochastic::~TSSWStochastic(void) {
}

void TSSWStochastic::Add( const CQuote& quote ) {
  double tmp = ( quote.Ask() + quote.Bid() ) / 2.0;
  if ( tmp != m_last ) {  // cut down on number of updates
    m_last = tmp;
    m_minmax.Add( m_last );
  }
}

void TSSWStochastic::Expire( const CQuote& quote ) {
  m_minmax.Remove( ( quote.Ask() + quote.Bid() ) / 2.0 );
}

void TSSWStochastic::PostUpdate( void ) {
  m_k = m_minmax.Max() == m_minmax.Min() ? 0 : ( ( m_last - m_minmax.Min() ) / ( m_minmax.Max() - m_minmax.Min() ) ) * 100.0;
}


} // namespace tf
} // namespace ou

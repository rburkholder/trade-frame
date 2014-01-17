/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include "TSSWStochastic.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

TSSWStochastic::TSSWStochastic( Quotes& quotes, time_duration tdWindowWidth ) 
  : TimeSeriesSlidingWindow<TSSWStochastic, Quote>( quotes, tdWindowWidth ),
    m_lastAdd( 0 ), m_lastExpire( 0 ), m_k( 0 )
{
}

TSSWStochastic::TSSWStochastic( const TSSWStochastic& rhs) 
  : TimeSeriesSlidingWindow<TSSWStochastic, Quote>( rhs ),
  m_lastAdd( rhs.m_lastAdd ), m_lastExpire( rhs.m_lastExpire ), m_k( rhs.m_k ),
  m_minmax( rhs.m_minmax )
{
}

TSSWStochastic::~TSSWStochastic(void) {
}

void TSSWStochastic::Add( const Quote& quote ) {
  double tmp = quote.Midpoint();
  if ( tmp != m_lastAdd ) {  // cut down on number of updates (can't use, needs to be replicated in Expire)
    m_lastAdd = tmp;
    m_minmax.Add( m_lastAdd );
  }
}

void TSSWStochastic::Expire( const Quote& quote ) {
  double tmp = quote.Midpoint();
  if ( tmp != m_lastExpire ) {  // cut down on number of updates (can't use, needs to be replicated in Expire)
    m_lastExpire = tmp;
    m_minmax.Remove( m_lastExpire );
  }
}

void TSSWStochastic::PostUpdate( void ) {
  m_k = m_minmax.Max() == m_minmax.Min() ? 0 : ( ( m_lastAdd - m_minmax.Min() ) / ( m_minmax.Max() - m_minmax.Min() ) ) * 100.0;
}

void TSSWStochastic::Reset( void ) {
  m_lastAdd = m_lastExpire = m_k = 0;
  m_minmax.Reset();
}


} // namespace tf
} // namespace ou

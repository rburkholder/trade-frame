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

#include "stdafx.h"

#include "TSSWStochastic.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

using base = RunningMinMax<TSSWStochastic,double>;

TSSWStochastic::TSSWStochastic( Quotes& quotes, time_duration tdWindowWidth ) 
  : TimeSeriesSlidingWindow<TSSWStochastic, Quote>( quotes, tdWindowWidth ),
    m_lastAdd( 0 ), m_lastExpire( 0 ), m_k( 0 )
{
}

TSSWStochastic::TSSWStochastic( const TSSWStochastic& rhs) 
  : TimeSeriesSlidingWindow<TSSWStochastic, Quote>( rhs ),
  m_lastAdd( rhs.m_lastAdd ), m_lastExpire( rhs.m_lastExpire ), m_k( rhs.m_k ),
  base( rhs )
{
}

TSSWStochastic::~TSSWStochastic(void) {
}

void TSSWStochastic::Add( const Quote& quote ) {
  double tmp = quote.Midpoint();
  if ( tmp != m_lastAdd ) {  // cut down on number of updates (can't use, needs to be replicated in Expire)
    m_lastAdd = tmp;
    base::Add( m_lastAdd );
  }
}

void TSSWStochastic::Expire( const Quote& quote ) {
  double tmp = quote.Midpoint();
  if ( tmp != m_lastExpire ) {  // cut down on number of updates (can't use, needs to be replicated in Expire)
    m_lastExpire = tmp;
    base::Remove( m_lastExpire );
  }
}

void TSSWStochastic::PostUpdate( void ) {
  double max( base::Max() );
  double min( base::Min() );
  m_k = max == min ? 0 : ( ( m_lastAdd - min ) / ( max - min ) ) * 100.0;
}

void TSSWStochastic::Reset( void ) {
  m_lastAdd = m_lastExpire = m_k = 0;
  base::Reset();
}


} // namespace tf
} // namespace ou

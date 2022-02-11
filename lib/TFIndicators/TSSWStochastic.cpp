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

#include "TSSWStochastic.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

using minmax = RunningMinMax<TSSWStochastic,double>;

TSSWStochastic::TSSWStochastic( Quotes& quotes, time_duration tdWindowWidth )
  : TimeSeriesSlidingWindow<TSSWStochastic, Quote>( quotes, tdWindowWidth ),
    m_lastAdd( 0 ), m_lastExpire( 0 ), m_k( 0 ), m_bAvailable( false )
{
}

TSSWStochastic::TSSWStochastic( Quotes& quotes, size_t nPeriods, time_duration tdPeriodWidth, fK_t&& fK )
  : TimeSeriesSlidingWindow<TSSWStochastic, Quote>( quotes, nPeriods, tdPeriodWidth ),
    m_lastAdd( 0 ), m_lastExpire( 0 ), m_k( 0 ), m_fK( std::move( fK ) ), m_bAvailable( false )
{
}

TSSWStochastic::~TSSWStochastic() {
}

double TSSWStochastic::Size() const {
  return ( minmax::Max() - minmax::Min() );
}

void TSSWStochastic::Add( const Quote& quote ) {
  if ( quote.IsNonZero() ) {
    double tmp = quote.Midpoint();
    if ( tmp != m_lastAdd ) {  // cut down on number of updates (can't use, needs to be replicated in Expire)
      m_lastAdd = tmp;
      minmax::Add( m_lastAdd );
    }
    m_dtLatest = quote.DateTime();
    m_bAvailable = true;
  }
  else {
    m_bAvailable = false;
  }
}

void TSSWStochastic::Expire( const Quote& quote ) {
  if ( quote.IsNonZero() ) {
    double tmp = quote.Midpoint();
    if ( tmp != m_lastExpire ) {  // cut down on number of updates (can't use, needs to be replicated in Add)
      m_lastExpire = tmp;
      minmax::Remove( m_lastExpire );
    }
    m_bAvailable = true;
  }
  // false
}

//void TSSWStochastic::PostUpdate() {
//  if ( m_bAvailable ) {
    //double max( minmax::Max() );
    //double min( minmax::Min() );
    //m_k = ( max == min ) ? 0 : ( ( ( m_lastAdd - min ) / ( max - min ) ) * 100.0 );
    //if ( m_fK ) m_fK( ou::tf::Price( m_dtLatest, m_k ) );
//  }
//}

void TSSWStochastic::UpdateOnAdd( double min, double max ) {
  if ( m_bAvailable ) {
    m_k = ( max == min ) ? 0 : ( ( ( m_lastAdd - min ) / ( max - min ) ) * 100.0 );
    if ( m_fK ) m_fK( ou::tf::Price( m_dtLatest, m_k ) );
  }
}

void TSSWStochastic::Reset() {
  m_lastAdd = m_lastExpire = m_k = 0;
  minmax::Reset();
}

} // namespace tf
} // namespace ou

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

TSSWStochastic::TSSWStochastic( CQuotes* quotes, long WindowSizeSeconds ) 
  : TimeSeriesSlidingWindow<TSSWStochastic, CQuote>( quotes, WindowSizeSeconds ),
    m_seconds( WindowSizeSeconds ), m_lastAdd( 0 ), m_lastExpire( 0 ), m_k( 0 )
{
}

TSSWStochastic::TSSWStochastic( const TSSWStochastic& stoch) 
  : TimeSeriesSlidingWindow<TSSWStochastic, CQuote>( stoch ),
  m_seconds( stoch.m_seconds ), m_lastAdd( stoch.m_lastAdd ), m_lastExpire( stoch.m_lastExpire ), m_k( stoch.m_k ),
  m_minmax( stoch.m_minmax )
{
}

TSSWStochastic::~TSSWStochastic(void) {
}

void TSSWStochastic::Add( const CQuote& quote ) {
  double tmp = ( quote.Ask() + quote.Bid() ) / 2.0;
  if ( tmp != m_lastAdd ) {  // cut down on number of updates (can't use, needs to be replicated in Expire)
    m_lastAdd = tmp;
    m_minmax.Add( m_lastAdd );
  }
}

void TSSWStochastic::Expire( const CQuote& quote ) {
  double tmp = ( quote.Ask() + quote.Bid() ) / 2.0;
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

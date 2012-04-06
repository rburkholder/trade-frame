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

#include <math.h>

#include "TSSWEfficiencyRatio.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

TSSWEfficiencyRatio::TSSWEfficiencyRatio( CTrades& trades, time_duration tdWindowWidth ) 
  : TimeSeriesSlidingWindow<TSSWEfficiencyRatio, CTrade>( trades, tdWindowWidth ),
    m_lastAdd( 0.0 ), m_lastExpire( 0.0 ), m_sum( 0.0 ), m_ratio( 0.0 ), m_total( 0.0 )
{
}

TSSWEfficiencyRatio::TSSWEfficiencyRatio( const TSSWEfficiencyRatio& ratio ) 
  : TimeSeriesSlidingWindow<TSSWEfficiencyRatio, CTrade>( ratio ),
    m_lastAdd( 0.0 ), m_lastExpire( 0.0 ), m_sum( 0.0 ), m_ratio( 0.0 ), m_total( 0.0 )
{
}

TSSWEfficiencyRatio::~TSSWEfficiencyRatio(void) {
}

void TSSWEfficiencyRatio::Add( const CTrade& trade ) {
  double tmp = trade.Trade();
  if ( 0.0 != m_lastAdd ) {
    double dif = fabs( tmp - m_lastAdd );
    m_sum += dif;
    m_total += dif;
  }
  else {
    m_lastExpire = tmp;  // prime the expire
  }
  m_lastAdd = tmp;
}

void TSSWEfficiencyRatio::Expire( const CTrade& trade ) {
  double tmp = trade.Trade();
  double dif = fabs( tmp - m_lastExpire );
  m_sum -= dif;
  m_lastExpire = tmp;  // not perfectly correct, but good enough, is next to last
}

void TSSWEfficiencyRatio::PostUpdate( void ) {
  if ( 0.0 != m_sum ) {
    m_ratio = ( m_lastAdd - m_lastExpire ) / m_sum;
  }
}

} // namespace tf
} // namespace ou

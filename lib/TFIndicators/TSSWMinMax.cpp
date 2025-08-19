/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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

/*
 * File:    TSSWMinMax.cpp
 * Author:  raymond@burkholder.net
 * Project: TFIndicators
 * Created: August 18, 2025 19:36:44
 */

#include "TSSWMinMax.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

using minmax = RunningMinMax<TSSWMinMax,double>;

TSSWMinMax::TSSWMinMax( Prices& prices, time_duration tdWindowWidth )
: TimeSeriesSlidingWindow<TSSWMinMax, Price>( prices, tdWindowWidth )
{
}

TSSWMinMax::TSSWMinMax( Prices& prices, size_t nPeriods, time_duration tdPeriodWidth )
: TimeSeriesSlidingWindow<TSSWMinMax, Price>( prices, nPeriods, tdPeriodWidth )
{
}

TSSWMinMax::~TSSWMinMax() {
}

double TSSWMinMax::Diff() const {
  return ( minmax::Max() - minmax::Min() );
}

void TSSWMinMax::Add( const Price& price ) {
  minmax::Add( price.Value() );
}

void TSSWMinMax::Expire( const Price& price ) {
  minmax::Remove( price.Value() );
}

//void TSSWMinMax::PostUpdate() {
//  if ( m_bAvailable ) {
    //double max( minmax::Max() );
    //double min( minmax::Min() );
    //m_k = ( max == min ) ? 0 : ( ( ( m_lastAdd - min ) / ( max - min ) ) * 100.0 );
    //if ( m_fK ) m_fK( ou::tf::Price( m_dtLatest, m_k ) );
//  }
//}

void TSSWMinMax::UpdateOnAdd( double min, double max ) {
}

void TSSWMinMax::Reset() {
  minmax::Reset();
}

} // namespace tf
} // namespace ou

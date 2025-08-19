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

#include "TSSWRunningTally.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

TSSWRunningTally::TSSWRunningTally( Prices& prices, time_duration tdWindowWidth )
  : TimeSeriesSlidingWindow<TSSWRunningTally, Price>( prices, tdWindowWidth ),
  m_net( 0.0 )
{
}

TSSWRunningTally::TSSWRunningTally( const TSSWRunningTally& rhs )
  : TimeSeriesSlidingWindow<TSSWRunningTally, Price>( rhs ),
  m_net( rhs.m_net )
{
}

TSSWRunningTally::~TSSWRunningTally() {
}

void TSSWRunningTally::Add( const Price& price ) {
  m_net += price.Value();
}

void TSSWRunningTally::Expire( const Price& price ) {
  m_net -= price.Value();
}

void TSSWRunningTally::PostUpdate() {
}

} // namespace tf
} // namespace ou


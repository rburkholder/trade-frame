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

#include "TSSWRunningTally.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

TSSWRunningTally::TSSWRunningTally( CPrices& prices, long WindowSizeSeconds ) 
  : TimeSeriesSlidingWindow<TSSWRunningTally, CPrice>( prices, WindowSizeSeconds ),
  m_net( 0.0 )
{
}

//TSSWRunningTally::TSSWRunningTally( const TSSWRunningTally& ) {
//}

TSSWRunningTally::~TSSWRunningTally(void) {
}

void TSSWRunningTally::Add( const CPrice& price ) {
  m_net += price.Price();
}

void TSSWRunningTally::Expire( const CPrice& price ) {
  m_net -= price.Price();
}

void TSSWRunningTally::PostUpdate( void ) {
}

} // namespace tf
} // namespace ou


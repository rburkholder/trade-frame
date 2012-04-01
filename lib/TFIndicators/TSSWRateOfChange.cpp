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

#include "TSSWRateOfChange.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

TSSWRateOfChange::TSSWRateOfChange( CPrices& prices, long WindowSizeSeconds ) 
  : TimeSeriesSlidingWindow<TSSWRateOfChange, CPrice>( prices, WindowSizeSeconds ),
  m_tail( 0.0 ), m_head( 0.0 )
{
}

TSSWRateOfChange::~TSSWRateOfChange(void) {
}

void TSSWRateOfChange::Add( const CPrice& price ) {
  m_head = price.Price();
}

void TSSWRateOfChange::Expire( const CPrice& price ) {
  m_tail = price.Price();
}

void TSSWRateOfChange::PostUpdate( void ) {
}

} // namespace tf
} // namespace ou


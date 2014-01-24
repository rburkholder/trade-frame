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

TSSWRateOfChange::TSSWRateOfChange( Prices& prices, time_duration tdWindowWidth ) 
  : TimeSeriesSlidingWindow<TSSWRateOfChange, Price>( prices, tdWindowWidth ),
  m_tail( 0.0 ), m_head( 0.0 )
{
}

TSSWRateOfChange::TSSWRateOfChange( const TSSWRateOfChange& rhs )
  : TimeSeriesSlidingWindow<TSSWRateOfChange, Price>( rhs ), 
  m_tail( rhs.m_tail ), m_head( rhs.m_head )
{
}

TSSWRateOfChange::~TSSWRateOfChange(void) {
}

void TSSWRateOfChange::Add( const Price& price ) {
  m_head = price.Value();
}

void TSSWRateOfChange::Expire( const Price& price ) {
  m_tail = price.Value();
}

void TSSWRateOfChange::PostUpdate( void ) {
}

} // namespace tf
} // namespace ou


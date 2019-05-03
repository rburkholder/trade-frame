/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:   TSSWDonchianChannel.cpp
 * Author: rpb
 * 
 * Created on May 3, 2019, 1:25 PM
 */

#include "TSSWDonchianChannel.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

TSSWDonchianChannel::TSSWDonchianChannel( Prices& prices, time_duration tdWindowWidth, size_t nWindowWidth )
: RunningMinMax(),
  TimeSeriesSlidingWindow<TSSWDonchianChannel, Price>( prices, tdWindowWidth, nWindowWidth )
{ }

TSSWDonchianChannel::~TSSWDonchianChannel( ) { }

//void TSSWDonchianChannel::UpdateMax( const double& ) {
//}

//void TSSWDonchianChannel::UpdateMin( const double& ) {
//}

void TSSWDonchianChannel::Add( const Price& price ) { // from TSSW
  minmax::Add( price.Value() );
}

void TSSWDonchianChannel::Expire( const Price& price ) { // from TSSW
  minmax::Remove( price.Value() );
}

//void TSSWDonchianChannel::PostUpdate() { // from TSSW
//}


} // namespace tf
} // namespace ou

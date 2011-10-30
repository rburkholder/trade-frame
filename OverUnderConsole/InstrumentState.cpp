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

#include "InstrumentState.h"


//================= InstrumentState ========================

// following are specific for gold futures
/*
InstrumentState::InstrumentState( void ):
  tdMarketOpen( time_duration( 19, 0, 0 ) ), // time relative to day  // pick up from symbol
  tdMarketOpenIdle( time_duration( 0, 0, 30 ) ),  // time relative to tdMarketOpen to allow initial collection of data
  tdCancelOrders( time_duration( 17, 50, 0 ) ),// time relative to day
  tdClosePositions( time_duration( 17, 51, 0 ) ),// time relative to day
  tdAfterMarket( time_duration( 18, 15, 0 ) ), // time relative to day
  tdMarketClosed( time_duration( 18, 15, 0 ) ), // time relative to day
  stochFast( &quotes, 60 ), stochMed( &quotes, 300 ), stochSlow( &quotes, 1800 ), // 1, 5, 30 min
  statsFast( &quotes, 60 ), statsMed( &quotes, 180 ), statsSlow( &quotes, 600 ), // 1, 3, 5 min
  bDaySession( true )
  {
    bMarketHoursCrossMidnight = tdMarketOpen > tdMarketClosed;
  }
  */

// following are specific for standard equities
InstrumentState::InstrumentState( void ):
  tdMarketOpen( time_duration( 10, 30, 0 ) ), // time relative to day  // pick up from symbol
  tdMarketOpenIdle( time_duration( 0, 0, 30 ) ),  // time relative to tdMarketOpen to allow initial collection of data
  tdCancelOrders( time_duration( 16, 50, 0 ) ),// time relative to day
  tdClosePositions( time_duration( 16, 51, 0 ) ),// time relative to day
  tdAfterMarket( time_duration( 17, 0, 0 ) ), // time relative to day
  tdMarketClosed( time_duration( 18, 0, 0 ) ), // time relative to day
  stochFast( &quotes, 60 ), stochMed( &quotes, 300 ), stochSlow( &quotes, 1800 ), // 1, 5, 30 min
  statsFast( &quotes, 60 ), statsMed( &quotes, 180 ), statsSlow( &quotes, 300 ), // 1, 3, 5 min
  bDaySession( true )
  {
    bMarketHoursCrossMidnight = tdMarketOpen > tdMarketClosed;
  }


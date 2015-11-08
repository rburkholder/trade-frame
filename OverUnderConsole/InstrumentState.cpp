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

// following are specific for gold futures
InstrumentStateFuturesGold::InstrumentStateFuturesGold( void ):
  InstrumentState( 
    time_duration( 19,  0,  0 ), // time relative to day  // pick up from symbol
    time_duration(  0,  0, 30 ), // time relative to tdMarketOpen to allow initial collection of data
    time_duration( 17, 50,  0 ), // time relative to day
    time_duration( 17, 51,  0 ), // time relative to day
    time_duration( 18, 15,  0 ), // time relative to day
    time_duration( 18, 15,  0 )  // time relative to day
	  )
  {
  }

// following are specific for standard US equities
InstrumentStateEquitiesUS::InstrumentStateEquitiesUS( void ):
  InstrumentState(
    time_duration( 10, 30,  0 ), // time relative to day  // pick up from symbol
    time_duration(  0,  2, 30 ), // time relative to tdMarketOpen to allow initial collection of data
    time_duration( 16, 54,  0 ), // time relative to day
    time_duration( 16, 55,  0 ), // time relative to day
    time_duration( 17,  0,  0 ), // time relative to day
    time_duration( 18,  0,  0 )  // time relative to day
    )
  {
  }

// following are specific for standard US equities
InstrumentState::InstrumentState( void ):
  stochFast( quotes, time_duration( 0, 0, 60 ) ), stochMed( quotes, time_duration( 0, 0, 300 ) ), stochSlow( quotes, time_duration( 0, 0, 1800 ) ), // 1, 5, 30 min
  statsFast( quotes, time_duration( 0, 0, 60 ) ), statsMed( quotes, time_duration( 0, 0, 180 ) ), statsSlow( quotes, time_duration( 0, 0, 300 ) ), // 1, 3, 5 min
  bDaySession( true )
  {
    bMarketHoursCrossMidnight = tdMarketOpen > tdMarketClosed;
  }

InstrumentState::InstrumentState(     
    time_duration tdMarketOpen_, 
    time_duration tdMarketOpenIdle_, 
    time_duration tdCancelOrders_,
    time_duration tdClosePositions_,
    time_duration tdAfterMarket_,
    time_duration tdMarketClosed_
    ): 
      tdMarketOpen( tdMarketOpen_ ),
      tdMarketOpenIdle( tdMarketOpenIdle_ ),
      tdCancelOrders( tdCancelOrders_ ),
      tdClosePositions( tdClosePositions_ ),
      tdAfterMarket( tdAfterMarket_ ),
      tdMarketClosed( tdMarketClosed_ ),
  stochFast( quotes, time_duration( 0, 0, 60 ) ), stochMed( quotes, time_duration( 0, 0, 300 ) ), stochSlow( quotes, time_duration( 0, 0, 1800 ) ), // 1, 5, 30 min
  statsFast( quotes, time_duration( 0, 0, 60 ) ), statsMed( quotes, time_duration( 0, 0, 180 ) ), statsSlow( quotes, time_duration( 0, 0, 300 ) ), // 1, 3, 5 min
  bDaySession( true )
{
  bMarketHoursCrossMidnight = tdMarketOpen > tdMarketClosed;
}
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
 * File:    Straddle.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 23, 2019, 10:50 AM
 */

#include "Straddle.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

using pInstrument_t = ou::tf::Instrument::pInstrument_t;
using pOption_t = Option::pOption_t;

Straddle::Straddle()
: Combo()
{
}

Straddle::Straddle( Straddle&& rhs )
: Combo( std::move( rhs ) )
{
}

Straddle::~Straddle() {
}

//void Strangle::Tick( bool bInTrend, double dblPriceUnderlying, ptime dt ) {
//  Combo::Tick( bInTrend, dblPriceUnderlying, dt ); // first or last in sequence?
//  if ( m_vLeg.empty() ) {
//    ChooseStrikes( dblPriceUnderlying );
//  }
//}

// TODO: need to fix this if other legs present.  Need to limit to the active legs.
//   maybe vector of inactive legs
// NOTE: if volatility drops, then losses occur on premium
//void Strangle::PlaceOrder( ou::tf::OrderSide::enumOrderSide side ) {
//  switch ( m_state ) {
//    case State::Positions: // doesn't confirm both put/call are available
//    case State::Watching:
//      for ( Leg& leg: m_vLeg ) {
//        leg.PlaceOrder( side, 1 );
//      }
//      m_state = State::Executing;
//      break;
//  }
//}

// TODO: functional, three/four sections:
//   choose option type, and strike price
//   check if different from existing trackers
//   construct options, and place into tracker
//   retrieve options after validation, and reset validator

// TODO: should be able to construct so leg1 + leg2 credit > 1.00

// TODO: need to fix this if other legs present.  Need to limit to the active legs.
//   maybe vector of inactive legs
// NOTE: if volatility drops, then losses occur on premium
void Straddle::PlaceOrder( ou::tf::OrderSide::enumOrderSide side ) {
  switch ( m_state ) {
    case State::Positions: // doesn't confirm both put/call are available
    case State::Watching:
      for ( Leg& leg: m_vLeg ) {
        leg.PlaceOrder( side, 1 );
      }
      m_state = State::Executing;
      break;
  }
}
Straddle::strike_pair_t Straddle::ChooseStrikes( const Chain& chain, double price ) const {

  double strikeAtmCall {};
  double strikeAtmPut {};

  strikeAtmCall = chain.Call_Atm( price );
  //assert( 0.0 <= ( strikeAtmCall - price ) );

  strikeAtmPut = chain.Put_Atm( price );
  //assert( 0.0 <= ( price - strikeAtmPut ) );

  if ( strikeAtmCall != strikeAtmPut ) {
    throw exception_strike_range_exceeded( "straddle" );
  }
  return strike_pair_t( strikeAtmCall, strikeAtmPut );
}

void Straddle::Init() {
}

} // namespace option
} // namespace tf
} // namespace ou

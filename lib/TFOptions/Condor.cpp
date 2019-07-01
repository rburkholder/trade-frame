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
 * File:    Condor.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 10, 2019, 6:24 PM
 */

#include "Condor.h"

// TODO: use VerticalSpread x 2?
//    need to use a vertical spread: bear call, enter for credit
//    need to use a vertical spread: bull put,  enter for credit

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

using pInstrument_t = ou::tf::Instrument::pInstrument_t;
using pOption_t = Option::pOption_t;

Condor::Condor()
: Combo()
{
}

Condor::Condor( const Condor& rhs )
: Combo( rhs )
{
}

Condor::Condor( const Condor&& rhs )
: Combo( std::move( rhs ) )
{
}

Condor::~Condor() {
}

void Condor::Tick( bool bInTrend, double dblPriceUnderlying, ptime dt ) {
  Combo::Tick( bInTrend, dblPriceUnderlying, dt ); // first or last in sequence?
//  if ( m_vLeg.empty() ) {
//    ChooseStrikes( dblPriceUnderlying );
//  }
}

// TODO: fix this, needs to be four strikes
Condor::strike_pair_t Condor::ChooseStrikes( const Chain& chain, double price ) const {

  double strikeOtmCall {};
  double strikeOtmPut {};

  strikeOtmCall = chain.Call_Otm( price );
  assert( 0.0 <= ( strikeOtmCall - price ) );
  if ( ( 0.20 * 0.50 ) > ( strikeOtmCall - price ) ) { // within edge of range
    strikeOtmCall = chain.Call_Otm( strikeOtmCall ); // choose a further out strike
  }
  strikeOtmPut = chain.Put_Otm( price );
  assert( 0.0 <= ( price - strikeOtmPut ) );
  if ( ( 0.20 * 0.50 ) > ( price - strikeOtmPut ) ) { // within edge of range
    strikeOtmPut = chain.Put_Otm( strikeOtmPut ); // choose a further out strike
  }
  assert( strikeOtmCall > strikeOtmPut );
  const double dblStrikeDelta = strikeOtmCall - strikeOtmPut;
  if ( m_dblMaxStrangleDelta > dblStrikeDelta ) {
    const double dblExclusionRange = 0.5 * ( ( 1.0 - 0.20 ) * dblStrikeDelta );  // enter in middle 20% only
    if (
      ( price < ( strikeOtmCall - dblExclusionRange ) ) &&
      ( price > ( strikeOtmPut  + dblExclusionRange ) )
    ) {
    }
    else{
      std::stringstream ss;
      ss << "Strangle::ChooseStrikes " << price << "," << dblExclusionRange << "," << strikeOtmCall << "," << strikeOtmPut;
      throw exception_strike_range_exceeded( ss.str().c_str() );
    }
  }
  return strike_pair_t( strikeOtmCall, strikeOtmPut );
}

// TODO: need to fix this if other legs present.  Need to limit to the active legs.
//   maybe vector of inactive legs
// NOTE: if volatility drops, then losses occur on premium
void Condor::PlaceOrder( ou::tf::OrderSide::enumOrderSide side ) {
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

// ==========
/*
ShortCondor::ShortCondor( )
: Condor()
{
}

ShortCondor::ShortCondor( ShortCondor&& rhs )
: Condor( std::move( rhs ) )
{
}

ShortCondor::~ShortCondor( ) { }

void ShortCondor::Tick( bool bInTrend, double dblPriceUnderlying, ptime dt ) {
  Condor::Tick( bInTrend, dblPriceUnderlying, dt ); // first or last in sequence?
//  if ( m_vLeg.empty() ) {
//    ChooseStrikes( dblPriceUnderlying );
//  }
}

// ==========

LongCondor::LongCondor( )
: Condor()
{
}

LongCondor::LongCondor( LongCondor&& rhs )
: Condor( std::move( rhs ) )
{
}

LongCondor::~LongCondor( ) { }

void LongCondor::Tick( bool bInTrend, double dblPriceUnderlying, ptime dt ) {
  Condor::Tick( bInTrend, dblPriceUnderlying, dt ); // first or last in sequence?
//  if ( m_vLeg.empty() ) {
//    ChooseStrikes( dblPriceUnderlying );
//  }
}

*/

} // namespace option
} // namespace tf
} // namespace ou


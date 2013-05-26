/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// started 2013-05-25

#include <algorithm>

#include "Margin.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options
namespace margin { // options

// === long equity

void Calc( RegTInitial& mr, const LongUnderlying& src ) {
  mr.margin = src.quantity * 0.25 * src.price;
}

void Calc( RegTMaintenance& mr, const LongUnderlying& src ) {
  mr.margin = src.quantity * 0.25 * src.price;
}

void Calc( RegTEndOfDay& mr, const LongUnderlying& src ) {
  mr.margin = src.quantity * 0.50 * src.price;
}

void Calc( CashOrRegTIra& mr, const LongUnderlying& src ) {
  mr.margin = src.quantity * src.price;
}

//==== short equity

void Calc( RegTInitial& mr, const ShortUnderlying& src ) {
  mr.margin = src.quantity * 0.30 * src.price;
}

void Calc( RegTMaintenance& mr, const ShortUnderlying& src ) {
  if ( 16.67 <= src.price ) mr.margin = src.quantity * 0.30 * src.price;
  else if ( 2.50 >= src.price ) mr.margin = src.quantity * 2.50;
  else if ( 5.00 <= src.price ) mr.margin = src.quantity * 5.00;
  else mr.margin = src.quantity * src.price; // between $2.50 and $16.67
}

void Calc( RegTEndOfDay& mr, const ShortUnderlying& src ) {
  mr.margin = src.quantity * 0.50 * src.price;
}

void Calc( CashOrRegTIra& mr, const ShortUnderlying& src ) {
  // 0
}

//==== long call or long put

void Calc( MarginRequirement& mr, const LongOption& src ) {
  mr.margin = src.quantity * src.price;
}

//==== naked short call

void Calc( RegT& mr, const Underlying& under, const ShortCall& call ) {
  double otm1 = call.pInstrument->GetStrike() - under.price;
  double otm2 = ( otm1 > 0.0 ) ? otm1 : 0.0;
  mr.margin = call.quantity * ( call.price + std::max<double>( 0.20 * under.price - otm2, 0.10 * under.price ) );
}

void Calc( CashOrRegTIra& mr, const Underlying& under, const ShortCall& src ) {
  // 0
}

//==== naked short put

void Calc( RegT& mr, const Underlying& under, const ShortPut& put ) {
  double otm1 = under.price - put.pInstrument->GetStrike();
  double otm2 = ( otm1 > 0.0 ) ? otm1 : 0.0;
  mr.margin = put.quantity * ( put.price + std::max<double>( 0.20 * under.price - otm2, 0.10 * put.pInstrument->GetStrike() ) );
}

void Calc( CashOrRegTIra& mr, const Underlying& under, const ShortPut& put ) {
  mr.margin = put.quantity * put.pInstrument->GetStrike();
}

//==== Covered Call

void Calc( RegT& mr, const LongUnderlying& under, const ShortCall& call ) {
  RegTInitial initial;
  Calc( initial, under );
  double itm1 = under.price - call.pInstrument->GetStrike();
  double itm2 = ( itm1 > 0.0 ) ? itm1 : 0.0;
  mr.margin = initial.margin + under.quantity * itm2;
}

void Calc( Cash& mr, const LongUnderlying& under, const ShortCall& call ) {
  mr.margin = under.quantity * under.price;
}

void Calc( RegTIra& mr, const LongUnderlying& under, const ShortCall& call ) {
  // 0
}

//==== Covered Put

void Calc( RegT& mr, const LongUnderlying& under, const ShortPut& put ) {
  RegTInitial initial;
  Calc( initial, under );
  double itm1 = put.pInstrument->GetStrike() - under.price;
  double itm2 = ( itm1 > 0.0 ) ? itm1 : 0.0;
  mr.margin = initial.margin + under.quantity * itm2;
}

void Calc( CashOrRegTIra& mr, const LongUnderlying& under, const ShortPut& put ) {
  // 0
}

//==== Call Spread
//  long position expires on or after the short position

void Calc( RegT& mr, const LongCall& longc, const ShortCall& shortc ) {
  assert( longc.quantity == shortc.quantity );
//  assert( longc.pInstrument->GetInstrumentName() == shortc.pInstrument->GetInstrumentName() );
  mr.margin = longc.quantity * std::max<double>( longc.pInstrument->GetStrike() - shortc.pInstrument->GetStrike(), 0.0 );
}

void Calc( Cash& mr, const LongCall& longc, const ShortCall& shortc ) {
  // 0, non 0 if european style cash settled
}

void Calc( RegTIra& mr, const LongCall& longc, const ShortCall& shortc ) {
  assert( longc.quantity == shortc.quantity );
//  assert( longc.pInstrument->GetInstrumentName() == shortc.pInstrument->GetInstrumentName() );
  mr.margin = longc.quantity * std::max<double>( longc.pInstrument->GetStrike() - shortc.pInstrument->GetStrike(), 0.0 );
}

//==== Put Spread
//  long position expires on or after the short position

void Calc( RegT& mr, const LongPut& longp, const ShortPut& shortp ) {
  assert( longp.quantity == shortp.quantity );
  mr.margin = longp.quantity * std::max<double>( shortp.pInstrument->GetStrike() - longp.pInstrument->GetStrike(), 0.0 );
}

void Calc( Cash& mr, const LongPut& longp, const ShortPut& shortp ) {
  // 0, non 0 if european style cash settled
}

void Calc( RegTIra& mr, const LongPut& longp, const ShortPut& shortp ) {
  assert( longp.quantity == shortp.quantity );
  mr.margin = longp.quantity * std::max<double>( shortp.pInstrument->GetStrike() - longp.pInstrument->GetStrike(), 0.0 );
}

// ===== Collar
// Long put and long underlying with short call. 
// Put and call must have same expiration date, same underlying (and same multiplier), 
// and put exercise price must be lower than call exercise price.

void Calc( RegTInitial& mr, const LongUnderlying& under, const ShortCall& call, const LongPut& put ) {
  RegTInitial initial;
  Calc( initial, under );
  double itm1 = under.price - call.pInstrument->GetStrike();
  double itm2 = ( itm1 > 0.0 ) ? itm1 : 0.0;
  mr.margin = initial.margin + call.quantity * itm2;
}

void Calc( RegTEndOfDay& mr, const LongUnderlying& under, const ShortCall& call, const LongPut& put ) {
  RegTInitial initial;
  Calc( initial, under );
  double itm1 = under.price - call.pInstrument->GetStrike();
  double itm2 = ( itm1 > 0.0 ) ? itm1 : 0.0;
  mr.margin = initial.margin + call.quantity * itm2;
}

void Calc( RegTMaintenance& mr, const LongUnderlying& under, const ShortCall& call, const LongPut& put ) {
  double otm1 = under.price - put.pInstrument->GetStrike();
  double otm2 = ( otm1 > 0.0 ) ? otm1 : 0.0;
  mr.margin = under.quantity * std::min<double>( 0.10 * put.pInstrument->GetStrike() + otm2, 0.25 * call.pInstrument->GetStrike() );
}

void Calc( CashOrRegTIra& mr, const LongUnderlying& under, const ShortCall& call, const LongPut& put ) {
    // 0
}

// === Long Call and Put

void Calc( MarginRequirement& mr, const LongCall& call, const LongPut& put ) {
  mr.margin = call.quantity * call.price + put.quantity * put.price;
}

// === short call and put

void Calc( RegT& mr, const ZeroUnderlying& under, const ShortCall& call, const ShortPut& put ) {
  RegTInitial rtcall, rtput;
  Calc( rtcall, under, call );
  Calc( rtput,  under, put  );
  if ( rtcall.margin  >= rtput.margin ) 
    mr.margin = rtcall.margin + put.quantity * put.price;
  else 
    mr.margin = rtput.margin + call.quantity * call.price;
}

void Calc( CashOrRegTIra& mr, const ZeroUnderlying& under, const ShortCall& call, const ShortPut& put ) {
    // 0
}

// === Long butterfly
// Two short options of the same series (class, multiplier, strike price, expiration) offset by 
// one long option of the same type (put or call) with a higher strike price and 
// one long option of the same type with a lower strike price. 
// All component options must have the same expiration, same underlying, and intervals between exercise prices must be equal.

void Calc( RegT& mr, const LongOption& opt1, const ShortOption& opt2, const LongOption& opt3 ) {
  mr.margin = opt1.quantity * opt1.price - opt2.quantity * opt2.price + opt3.quantity * opt3.price;
}

void Calc( Cash& mr, const LongOption& opt1, const ShortOption& opt2, const LongOption& opt3 ) {
  // 0, non 0 if european style cash settled
}

void Calc( RegTIra& mr, const LongOption& opt1, const ShortOption& opt2, const LongOption& opt3 ) {
  mr.margin = opt1.quantity * opt1.price - opt2.quantity * opt2.price + opt3.quantity * opt3.price;
}


// == Short Butterfly Put
// Two long put options of the same series offset by 
// one short put option with a higher strike price and 
// one short put option with a lower strike price. 
// All component options must have the same expiration, same underlying, and intervals between exercise prices must be equal.

void Calc( RegT& mr, const ShortPut& put1, const LongPut& put2, const ShortPut& put3 ) {
  double hi = std::max<double>( put1.pInstrument->GetStrike(), put3.pInstrument->GetStrike() );
  double lo = std::max<double>( put1.pInstrument->GetStrike(), put3.pInstrument->GetStrike() );
  double mid = put2.pInstrument->GetStrike();
  mr.margin = ( hi - mid ) + ( mid - lo );
}

void Calc( CashOrRegTIra& mr, const ShortPut& put1, const LongPut& put2, const ShortPut& put3 ) {
    // 0
}

// == Short Butterfly Call
// Two long call options of the same series offset by 
// one short call option with a higher strike price and 
// one short call option with a lower strike price. 
// All component options must have the same expiration, same underlying, and intervals between exercise prices must be equal.

void Calc( RegT& mr, const ShortCall& call1, const LongCall& call2, const ShortCall& call3 ) {
  double hi = std::max<double>( call1.pInstrument->GetStrike(), call3.pInstrument->GetStrike() );
  double lo = std::max<double>( call1.pInstrument->GetStrike(), call3.pInstrument->GetStrike() );
  double mid = call2.pInstrument->GetStrike();
  mr.margin = ( hi - mid ) + ( mid - lo );
}

void Calc( CashOrRegTIra& mr, const ShortPut& put1, const LongPut& put2, const ShortPut& put3 ) {
    // 0
}

// === Long Box Spread
// Long call and short put with the same exercise price ("buy side") coupled with a 
// long put and short call with the same exercise price ("sell side"). 
// Buy side exercise price is lower than the sell side exercise price. 
// All component options must have the same expiration, and underlying (multiplier).

// may need correction: Initial long box spread requirement must be maintained.

void Calc( RegT& mr, const LongCall& call1, const ShortPut& put1, const LongPut& put2, const ShortCall& call2 ) {
  mr.margin = call1.quantity * call1.price - ( put1.quantity * put1.price ) 
    + put2.quantity * put2.price - ( call2.quantity * call2.price );
}

void Calc( Cash& mr, const LongCall& call1, const ShortPut& put1, const LongPut& put2, const ShortCall& call2 ) {
    // 0
}

void Calc( RegTIra& mr, const LongCall& call1, const ShortPut& put1, const LongPut& put2, const ShortCall& call2 ) {
  mr.margin = call1.quantity * call1.price - ( put1.quantity * put1.price ) 
    + put2.quantity * put2.price - ( call2.quantity * call2.price );
}

// === Short Box Spread
// Long call and short put with the same exercise price ("buy side") coupled with a 
// long put and short call with the same exercise price ("sell side"). 
// Buy side exercise price is higher than the sell side exercise price. 
// All component options must have the same expiration, and underlying (multiplier).

void Calc( RegT& mr, const LongPut& lput, const ShortCall& scall, const ShortPut& sput, const LongCall& lcall ) {
  // european: Long Call Strike - Short Call Strike 
  // american
  double calc = lput.price + lcall.price - scall.price - sput.price;
  assert( 0 < calc );
  mr.margin = lput.quantity * std::max<double>( 1.02 * calc, lcall.pInstrument->GetStrike() - scall.pInstrument->GetStrike() );
}

void Calc( Cash& mr, const LongPut& lput, const ShortCall& scall, const ShortPut& sput, const LongCall& lcall ) {
    // 0
}

void Calc( RegTIra& mr, const LongPut& lput, const ShortCall& scall, const ShortPut& sput, const LongCall& lcall ) {
  // european: Long Call Strike - Short Call Strike 
  // american
  double calc = lput.price + lcall.price - scall.price - sput.price;
  assert( 0 < calc );
  mr.margin = lput.quantity * std::max<double>( 1.02 * calc, lcall.pInstrument->GetStrike() - scall.pInstrument->GetStrike() );
}



} // namespace margin
} // namespace option
} // namespace tf
} // namespace ou

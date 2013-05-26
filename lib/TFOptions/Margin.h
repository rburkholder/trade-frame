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

#pragma once

#include <assert.h> 

#include <TFTrading/Instrument.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options
namespace margin { // options

// need to add ability to do stock options, index options, world currency options, cash basket option
// need validation function for each style
// margin requirements not correct, need option contract multiplier

typedef ou::tf::Instrument::pInstrument_cref pInstrument_cref;

struct MarginRequirement { 
  double margin; 
  MarginRequirement( void ): margin( 0 ) {};
};
struct RegT: MarginRequirement {};
//struct RegTInitialOrMaintenance: RegT {};
struct RegTInitial: RegT {};
struct RegTMaintenance: RegT {};
struct RegTEndOfDay: RegT {};
struct CashOrRegTIra: MarginRequirement {};
struct Cash: CashOrRegTIra {};
struct RegTIra: CashOrRegTIra {};

struct Source {
  pInstrument_cref pInstrument;
  unsigned int quantity;
  double price;
};

struct Underlying: Source {};
struct ZeroUnderlying: Source {};
struct ShortUnderlying: Underlying {};
struct LongUnderlying: Underlying {};
struct Option: Source {};
struct LongOption: Option {};
struct ShortOption: Option {};
struct LongCall: LongOption {};
struct LongPut: LongOption {};
struct ShortCall: ShortOption {};
struct ShortPut: ShortOption {};

void Calc( RegTInitial&, const LongUnderlying& src );
void Calc( RegTMaintenance&, const LongUnderlying& src );
void Calc( RegTEndOfDay&, const LongUnderlying& src );
void Calc( CashOrRegTIra&, const LongUnderlying& src );

void Calc( RegTInitial&, const ShortUnderlying& src );
void Calc( RegTMaintenance&, const ShortUnderlying& src );
void Calc( RegTEndOfDay&, const ShortUnderlying& src );
void Calc( CashOrRegTIra&, const ShortUnderlying& src );

void Calc( MarginRequirement& mr, const LongOption& src );  // long call or put

void Calc( RegT& mr, const ZeroUnderlying& under, const ShortCall& call );  // naked short call
void Calc( CashOrRegTIra& mr, const ZeroUnderlying& under, const ShortCall& call );

void Calc( RegT& mr, const ZeroUnderlying& under, const ShortPut& put );  // naked short put
void Calc( CashOrRegTIra& mr, const ZeroUnderlying& under, const ShortPut& put );

void Calc( RegT& mr, const LongUnderlying& under, const ShortCall& call );  // covered call
void Calc( Cash& mr, const LongUnderlying& under, const ShortCall& call );
void Calc( RegTIra& mr, const LongUnderlying& under, const ShortCall& call );

void Calc( RegT& mr, const LongUnderlying& under, const ShortPut& put );  // covered put
void Calc( CashOrRegTIra& mr, const LongUnderlying& under, const ShortPut& put );

void Calc( RegT& mr, const LongCall& longc, const ShortCall& shortc ); // call spread
void Calc( Cash& mr, const LongCall& longc, const ShortCall& shortc );
void Calc( RegTIra& mr, const LongCall& longc, const ShortCall& shortc );

void Calc( RegT& mr, const LongPut& longp, const ShortPut& shortp ); // put spread
void Calc( Cash& mr, const LongPut& longp, const ShortPut& shortp );
void Calc( RegTIra& mr, const LongPut& longp, const ShortPut& shortp );

void Calc( RegTInitial& mr, const LongUnderlying& under, const ShortCall& call, const LongPut& put ); // collar
void Calc( RegTMaintenance& mr, const LongUnderlying& under, const ShortCall& call, const LongPut& put ); // collar
void Calc( RegTEndOfDay& mr, const LongUnderlying& under, const ShortCall& call, const LongPut& put ); // collar
void Calc( CashOrRegTIra& mr, const LongUnderlying& under, const ShortCall& call, const LongPut& put ); // collar

void Calc( MarginRequirement& mr, const LongCall& call, const LongPut& put );  // long call and long put

void Calc( RegT& mr, const ZeroUnderlying& under, const ShortCall& call, const ShortPut& put );  // short call and short put
void Calc( CashOrRegTIra& mr, const ZeroUnderlying& under, const ShortCall& call, const ShortPut& put );

void Calc( RegT& mr, const LongOption& opt1, const ShortOption& opt2, const LongOption& opt3 );  // long butterfly
void Calc( Cash& mr, const LongOption& opt1, const ShortOption& opt2, const LongOption& opt3 );  // long butterfly
void Calc( RegTIra& mr, const LongOption& opt1, const ShortOption& opt2, const LongOption& opt3 );  // long butterfly

void Calc( RegT& mr, const ShortPut& put1, const LongPut& put2, const ShortPut& put3 ); // short butterfly put
void Calc( CashOrRegTIra& mr, const ShortPut& put1, const LongPut& put2, const ShortPut& put3 );

void Calc( RegT& mr, const ShortCall& call1, const LongCall& call2, const ShortCall& call3 ); // short butterfly call
void Calc( CashOrRegTIra& mr, const ShortCall& call1, const LongCall& call2, const ShortCall& call3 );

void Calc( RegT& mr, const LongCall& call1, const ShortPut& put1, const LongPut& put2, const ShortCall& call2 ); // long box spread
void Calc( Cash& mr, const LongCall& call1, const ShortPut& put1, const LongPut& put2, const ShortCall& call2 );
void Calc( RegTIra& mr, const LongCall& call1, const ShortPut& put1, const LongPut& put2, const ShortCall& call2 );

void Calc( RegT& mr, const LongPut& lput, const ShortCall& scall, const ShortPut& sput, const LongCall& lcall );  // short box spread
void Calc( Cash& mr, const LongPut& lput, const ShortCall& scall, const ShortPut& sput, const LongCall& lcall );
void Calc( RegTIra& mr, const LongPut& lput, const ShortCall& scall, const ShortPut& sput, const LongCall& lcall );

} // namespace margin
} // namespace option
} // namespace tf
} // namespace ou

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

#include "StdAfx.h"

#include <algorithm>

#include <TFTrading/InstrumentManager.h>

#include "LiborFromIQFeed.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace libor { // libor
namespace local { // local

bool bInitialized = false;
bool bWatching = false;
pProvider_t pProvider;
vLibor_t vLibor;

void Initialize( pProvider_t pProvider ) {
  vLibor.push_back( structLibor( time_duration( hours(   0 * 24 ) ),  "ONLIB.X" ) ); // overnight
  vLibor.push_back( structLibor( time_duration( hours(   7 * 24 ) ),  "1WLIB.X" ) ); //  1 week
  vLibor.push_back( structLibor( time_duration( hours(  14 * 24 ) ),  "2WLIB.X" ) ); //  2 week
  vLibor.push_back( structLibor( time_duration( hours(  30 * 24 ) ),  "1MLIB.X" ) ); //  1 month
  vLibor.push_back( structLibor( time_duration( hours(  60 * 24 ) ),  "2MLIB.X" ) ); //  2 month
  vLibor.push_back( structLibor( time_duration( hours(  90 * 24 ) ),  "3MLIB.X" ) ); //  3 month
  vLibor.push_back( structLibor( time_duration( hours( 120 * 24 ) ),  "4MLIB.X" ) ); //  4 month
  vLibor.push_back( structLibor( time_duration( hours( 150 * 24 ) ),  "5MLIB.X" ) ); //  5 month
  vLibor.push_back( structLibor( time_duration( hours( 180 * 24 ) ),  "6MLIB.X" ) ); //  6 month
  vLibor.push_back( structLibor( time_duration( hours( 210 * 24 ) ),  "7MLIB.X" ) ); //  7 month
  vLibor.push_back( structLibor( time_duration( hours( 240 * 24 ) ),  "8MLIB.X" ) ); //  8 month
  vLibor.push_back( structLibor( time_duration( hours( 270 * 24 ) ),  "9MLIB.X" ) ); //  9 month
  vLibor.push_back( structLibor( time_duration( hours( 300 * 24 ) ), "10MLIB.X" ) ); // 10 month
  vLibor.push_back( structLibor( time_duration( hours( 330 * 24 ) ), "11MLIB.X" ) ); // 11 month
  vLibor.push_back( structLibor( time_duration( hours( 365 * 24 ) ),  "1YLIB.X" ) ); //  1 year 

  for ( vLibor_t::iterator iter = vLibor.begin(); vLibor.end() != iter; ++iter ) {
    ou::tf::InstrumentManager& mgr( ou::tf::InstrumentManager::Instance() );
    ou::tf::Instrument::pInstrument_t pInstrument;
    pInstrument = mgr.ConstructInstrument( iter->Symbol, "INDEX", ou::tf::InstrumentType::Index );
    iter->pWatch.reset( new Watch( pInstrument, pProvider ) );
  }
}

} // namespace local

void SetWatchOn( pProvider_t pProvider ) {
  assert( ou::tf::keytypes::EProviderIQF == pProvider->ID() );
  if ( !local::bInitialized ) {
    local::bInitialized = true;
    local::pProvider = pProvider;
    local::Initialize( pProvider );
  }
  if ( !local::bWatching ) {
    local::bWatching == true;
    for ( local::vLibor_t::iterator iter = local::vLibor.begin(); local::vLibor.end() != iter; ++ iter ) {
      iter->pWatch->StartWatch();
    }
  }
}

void SetWatchOff( void ) {
  if ( local::bWatching ) {
    local::bWatching == false;
    for ( local::vLibor_t::iterator iter = local::vLibor.begin(); local::vLibor.end() != iter; ++ iter ) {
      iter->pWatch->StopWatch();
    }
  }
}

double Value( time_duration td ) {
  assert( local::bWatching );
  assert( td >= local::vLibor[ 0 ].td );
  local::structLibor tmp( td, "" );
  local::vLibor_iter_t iter1 = std::lower_bound( local::vLibor.begin(), local::vLibor.end(), tmp, local::compLibor() );
  local::vLibor_iter_t iter2;
  if ( local::vLibor.end() == iter1 ) {
    // extrapolate beyond end
    --iter1;
    iter2 = iter1;
    -- iter1;
  }
  else {
    if ( iter1->td == td ) {
      // use the assigned value
      return iter1->pWatch->Price();
    }
    else {
      // interpolate
      iter2 = iter1;
      --iter1;
    }
  }
  double rate = iter1->pWatch->Price() + 
    ( ( iter2->pWatch->Price() - iter1->pWatch->Price() ) * 
    ( ( (double)( td - iter1->td ).total_seconds() ) / ( (double) ( iter2->td - iter1->td ).total_seconds() ) ) );
  return rate;
}

} // namespace libor
} // namespace tf
} // namespace ou

/*
  std::vector<int> v;
  v.push_back( 0 );
  v.push_back( 1 );
  v.push_back( 3 );
  v.push_back( 5 );

  std::vector<int>::iterator i1, i2, i3, i4;
  i1 = std::lower_bound( v.begin(), v.end(), 0 ); // value is 0
  i2 = std::lower_bound( v.begin(), v.end(), 2 ); // value is 3
  i3 = std::lower_bound( v.begin(), v.end(), 6 ); // end
  bool b = ( v.end() == i3 );
  i4 = std::lower_bound( v.begin(), v.end(), 3 ); // value is 3
*/

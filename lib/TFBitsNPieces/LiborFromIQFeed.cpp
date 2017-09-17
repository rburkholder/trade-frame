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

#include "stdafx.h"

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

  ou::tf::InstrumentManager& mgr( ou::tf::InstrumentManager::Instance() );
  ou::tf::Instrument::pInstrument_t pInstrument;
  for ( vLibor_t::iterator iter = vLibor.begin(); vLibor.end() != iter; ++iter ) {
    // need to check if it already exists
    if ( mgr.Exists( iter->Symbol, pInstrument ) ) {
    }
    else {
      pInstrument = mgr.ConstructInstrument( iter->Symbol, "INDEX", ou::tf::InstrumentType::Index );
    }
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
    local::bWatching = true;
    for ( local::vLibor_t::iterator iter = local::vLibor.begin(); local::vLibor.end() != iter; ++ iter ) {
      iter->pWatch->StartWatch();
    }
  }
}

void SetWatchOff( void ) {
  if ( local::bWatching ) {
    local::bWatching = false;
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
      return iter1->pWatch->LastTrade().Price();
    }
    else {
      // interpolate
      iter2 = iter1;
      --iter1;
    }
  }
  double rate = iter1->pWatch->LastTrade().Price() + 
    ( ( iter2->pWatch->LastTrade().Price() - iter1->pWatch->LastTrade().Price() ) * 
    ( ( (double)( td - iter1->td ).total_seconds() ) / ( (double) ( iter2->td - iter1->td ).total_seconds() ) ) );
  return rate;
}

void EmitYieldCurve( void ) {
  for ( local::vLibor_t::iterator iter = local::vLibor.begin(); local::vLibor.end() != iter; ++ iter ) {
    std::cout << iter->Symbol << " " << iter->pWatch->LastTrade().Price() << std::endl;
  }
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


/*

The short story is, use IRX.XO. The long story is longer, of course.

Theoretically, if you want to be precise, that depends on two things. A call is a delayed purchase, so you are holding cash and earning the risk-free rate on that cash prior to purchase. The benefit of delaying the purchase with the call goes up as the interest rate on your cash goes up, so the price of the call goes up. So length of time and location in the world where you are holding your cash would both be factors.

In the US, you might typically use T-bill rates that correspond to the time-to-expiry of the option.

In Europe, you might typically use German Bund rates.

However the LIBOR interbank over-night interest rates is also widely used for shorter terms.

https://quant.stackexchange.com/questions/15916/which-risk-free-rate-is-assumed-by-market-when-pricing-american-options

https://quant.stackexchange.com/questions/24414/how-to-use-the-black-scholes-formula-with-libor-rates

https://quant.stackexchange.com/questions/28287/risk-free-rate-vs-libor

Hull and White (authors of the canonical textbook on derivatives pricing)
http://www-2.rotman.utoronto.ca/~hull/DownloadablePublications/LIBORvsOIS.pdf
write in their abstract:
"Traditionally practitioners have used LIBOR and LIBOR-swap rates as proxies for risk-free rates
when valuing derivatives. This practice has been called into question by the credit crisis that
started in 2007. Many banks now consider that overnight indexed swap (OIS) rates should be
used as the risk-free rate when collateralized portfolios are valued and that LIBOR should be
used for this purpose when portfolios are not collateralized. This paper examines this practice
and concludes that OIS rates should be used in all situations."

Beyond theory, in practice, the risk-free rate is both (nowadays) small and changes very slowly. Since you usually want to match your markets pricing, you would typically either (1) just use a 30-day t-bill price; or (2) just calibrate against what an average of what current prices are saying they are using for the risk free rate by backsolving for it.

In even simpler practice, and for computing with IQFeed data, I would simply use the IRX.XO symbol:

IRX.XO is the 13-week T-bill (Treasury bill) yield index, published by the CBOE. Currently it is about 1% so its not going to make a huge impact on your greeks anyway, but you've got to use something, so use IRX.XO. It is readily available in the IQFeed.

---

For those using IRX.XO (e.g. for the ~13week timeframe), remember to divide by 1000 to get a percentage (annualized percentage rate).

*/

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
 * File:    PutBackSpread.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 23, 2019, 10:50 AM
 */

#include <array>

#include "LegDef.h"
#include "PutBackSpread.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options
namespace spread { // spread

namespace {

  static const size_t nStrikes( 2 );
  static const boost::gregorian::days nDaysToExpiry( 1 );

  using LegDef = ou::tf::option::LegDef;
  using rLegDef_t = std::array<LegDef,nStrikes>;

  static const rLegDef_t m_rLegDefLong = {
    LegDef( LegDef::EOrderSide::Buy,  1, LegDef::EOptionSide::Put ), // upper
    LegDef( LegDef::EOrderSide::Sell, 2, LegDef::EOptionSide::Put )  // lower
  };

  static const rLegDef_t m_rLegDefShort = {
    LegDef( LegDef::EOrderSide::Sell, 1, LegDef::EOptionSide::Put ), // upper
    LegDef( LegDef::EOrderSide::Buy,  2, LegDef::EOptionSide::Put )  // lower
  };

}

PutBackSpread::PutBackSpread( )
: Combo()
{
}

PutBackSpread::PutBackSpread( PutBackSpread&& rhs )
: Combo( std::move( rhs ) )
{
}

PutBackSpread::~PutBackSpread( ) { }

PutBackSpread::strike_pair_t PutBackSpread::ChooseStrikes( const Chain& chain, double price ) {

  double strikeOtmCallHigher {};
  double strikeOtmCallLower {};

//  strikeOtmCallLower = chains.Call_Otm( price );
//  double diffLower = strikeOtmCallLower - price;
//  assert( 0.0 <= ( diffLower ) );

//  strikeOtmCallHigher = chains.Put_Otm( strikeOtmCallLower );

//  assert( strikeOtmCallHigher > strikeOtmCallLower );

//  const double diffUpper = strikeOtmCallHigher - strikeOtmCallLower;
//  assert( 0.0 <= ( diffUpper ) ); // redundant

//  const double dblStrikeDelta = diffUpper;
//  if ( m_dblMaxStrikeDelta > dblStrikeDelta ) {
    // concerned if $0.50 vs $1.00?
//  }

  return strike_pair_t( strikeOtmCallLower, strikeOtmCallHigher );
}

} // namespace spread
} // namespace option
} // namespace tf
} // namespace ou


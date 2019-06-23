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

#include "PutBackSpread.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options
namespace spread { // spread

const Combo::leg_pair_t PutBackSpread::m_legDefLong(
  Combo::LegDef( Combo::EOptionSide::Put, Combo::EOrderSide::Buy,  1 ), // upper
  Combo::LegDef( Combo::EOptionSide::Put, Combo::EOrderSide::Sell, 2 )  // lower
);

const Combo::leg_pair_t PutBackSpread::m_legDefShort(
  Combo::LegDef( Combo::EOptionSide::Put, Combo::EOrderSide::Sell, 1 ), // upper
  Combo::LegDef( Combo::EOptionSide::Put, Combo::EOrderSide::Buy,  2 )  // lower
);

PutBackSpread::PutBackSpread( )
: Combo()
{
}

PutBackSpread::PutBackSpread( PutBackSpread&& rhs )
: Combo( std::move( rhs ) )
{
}

PutBackSpread::~PutBackSpread( ) { }

PutBackSpread::strike_pair_t PutBackSpread::ChooseStrikes( const IvAtm& chains, double price ) {

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


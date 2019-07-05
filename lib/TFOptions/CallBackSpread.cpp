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
 * File:    CallBackSpread.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 23, 2019, 10:50 AM
 */

#include "CallBackSpread.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options
namespace spread { // spread
/*
const Combo::leg_pair_t CallBackSpread::m_legDefLong(
  Combo::LegDef( Combo::EOptionSide::Call, Combo::EOrderSide::Sell, 2 ), // upper
  Combo::LegDef( Combo::EOptionSide::Call, Combo::EOrderSide::Buy,  1 )  // lower
);

const Combo::leg_pair_t CallBackSpread::m_legDefShort(
  Combo::LegDef( Combo::EOptionSide::Call, Combo::EOrderSide::Buy,  2 ), // upper
  Combo::LegDef( Combo::EOptionSide::Call, Combo::EOrderSide::Sell, 1 )  // lower
);
*/
CallBackSpread::CallBackSpread( )
: Combo()
{
}

CallBackSpread::CallBackSpread( CallBackSpread&& rhs )
: Combo( std::move( rhs ) )
{
}

CallBackSpread::~CallBackSpread( ) { }

CallBackSpread::strike_pair_t CallBackSpread::ChooseStrikes( const Chain& chain, double price ) {

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


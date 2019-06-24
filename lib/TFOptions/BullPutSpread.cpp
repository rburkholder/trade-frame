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
 * File:    BullPutSpread.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 16, 2019, 1:49 PM
 */

#include "BullPutSpread.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options
namespace spread { // spread

const Combo::leg_pair_t BullPut::m_legDefLong(
  Combo::LegDef( Combo::EOptionSide::Put, Combo::EOrderSide::Buy,  1 ), // upper
  Combo::LegDef( Combo::EOptionSide::Put, Combo::EOrderSide::Sell, 1 )  // lower
);

const Combo::leg_pair_t BullPut::m_legDefShort(
  Combo::LegDef( Combo::EOptionSide::Put, Combo::EOrderSide::Sell, 1 ), // upper
  Combo::LegDef( Combo::EOptionSide::Put, Combo::EOrderSide::Buy,  1 )  // lower
);

BullPut::BullPut( )
: Combo()
{
}

BullPut::BullPut( BullPut&& rhs )
: Combo( std::move( rhs ) )
{
}

BullPut::~BullPut( ) { }

BullPut::strike_pair_t BullPut::ChooseStrikes( const Chain& chain, double price ) {

  double strikeOtmPutHigher {};
  double strikeOtmPutLower {};

  strikeOtmPutHigher = chain.Put_Otm( price );
  double diffHigher = price - strikeOtmPutHigher;
  assert( 0.0 <= ( diffHigher ) );

  strikeOtmPutLower = chain.Put_Otm( strikeOtmPutHigher );

  assert( strikeOtmPutHigher > strikeOtmPutLower );

  const double diffLower = strikeOtmPutHigher - strikeOtmPutLower;
  assert( 0.0 <= ( diffLower ) ); // redundant

  const double dblStrikeDelta = diffLower;
  if ( m_dblMaxStrikeDelta > dblStrikeDelta ) {
    // concerned if $0.50 vs $1.00?
  }

  return strike_pair_t( strikeOtmPutHigher, strikeOtmPutLower );
}

} // namespace spread
} // namespace option
} // namespace tf
} // namespace ou


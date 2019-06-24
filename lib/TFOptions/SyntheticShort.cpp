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
 * File:    SyntheticShort.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 23, 2019, 10:49 AM
 */

#include "SyntheticShort.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options
namespace spread { // spread

const Combo::leg_pair_t SyntheticShort::m_legDefLong(
  Combo::LegDef( Combo::EOptionSide::Call, Combo::EOrderSide::Sell, 1 ),
  Combo::LegDef( Combo::EOptionSide::Put,  Combo::EOrderSide::Buy,  1 )
);

const Combo::leg_pair_t SyntheticShort::m_legDefShort(
  Combo::LegDef( Combo::EOptionSide::Call, Combo::EOrderSide::Buy,  1 ),
  Combo::LegDef( Combo::EOptionSide::Put,  Combo::EOrderSide::Sell, 1 )
);

SyntheticShort::SyntheticShort( )
: Combo()
{
}

SyntheticShort::SyntheticShort( SyntheticShort&& rhs )
: Combo( std::move( rhs ) )
{
}

SyntheticShort::~SyntheticShort( ) { }

SyntheticShort::strike_pair_t SyntheticShort::ChooseStrikes( const Chain& chain, double price ) {

  double strikeCall {};
  double strikeItmPut {};

  strikeItmPut = chain.Put_Itm( price );
  strikeCall = strikeItmPut;

  return strike_pair_t( strikeItmPut, strikeCall );
}

} // namespace spread
} // namespace option
} // namespace tf
} // namespace ou


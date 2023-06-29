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
 * File:    VerticalSpread.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 11, 2019, 8:03 PM
 */

#pragma once

#include "ComboTraits.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // option
namespace spread { // spread
namespace vertical { // vertical

// contains 'vertical bear call' and 'vertical bull put'
// chosen based upon ComboTraits::E20DayDirection

size_t LegCount();

void ChooseLegs( // throw Chain exceptions
  ComboTraits::E20DayDirection
, const mapChains_t& chains
, boost::gregorian::date
, const SpreadSpecs&
, double priceUnderlying
, const fLegSelected_t&&
);

void FillLegNote( size_t ix, ComboTraits::E20DayDirection, LegNote::values_t& );

std::string Name(
  ComboTraits::E20DayDirection
, const mapChains_t& chains
, boost::gregorian::date
, const SpreadSpecs&
, double price
, const std::string& sUnderlying
);

void AddLegOrder(
  const LegNote::Type
, pOrderCombo_t
, const ou::tf::OrderSide::EOrderSide
, uint32_t nOrderQuantity
, pPosition_t
);

void Bind( ComboTraits& traits );

} // namespace vertical
} // namespace spread
} // namespace option
} // namespace tf
} // namespace ou

/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    ComboTraits.hpp
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created: 2023/06/28 19:19:45
 */

#pragma once

#include <functional>

#include <boost/date_time/gregorian/greg_date.hpp>

#include <TFTrading/Position.h>
#include <TFTrading/Order_Combo.hpp>

#include <TFOptions/Chain.h>

#include "LegNote.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class SpreadSpecs;

using chain_t = ou::tf::option::Chain<ou::tf::option::chain::OptionName>;
using mapChains_t = std::map<boost::gregorian::date, chain_t>;
using fLegSelected_t = std::function<void( double /* strike */, boost::gregorian::date, const std::string&)>;
using pPosition_t = ou::tf::Position::pPosition_t;
using pOrderCombo_t = ou::tf::OrderCombo::pOrderCombo_t;

struct ComboTraits {

  enum class EMarketDirection { Select, Rising, Falling, NotApplicable };

  using fLegCount_t = std::function<size_t()>;

  using fChooseLegs_t = std::function<void(
    EMarketDirection
  , const mapChains_t& chains
  , boost::gregorian::date
  , const SpreadSpecs&
  , double priceUnderlying
  , const fLegSelected_t&&
  )>;

  using fFillLegNote_t = std::function<void(
    size_t ix, EMarketDirection, LegNote::values_t&
  )>;

  using fName_t = std::function<std::string(
    EMarketDirection
  , const mapChains_t& chains
  , boost::gregorian::date
  , const SpreadSpecs&
  , double price
  , const std::string& sUnderlying
  )>;

  using fAddLegOrder_t = std::function<void(
    const LegNote::Type
  , pOrderCombo_t
  , const ou::tf::OrderSide::EOrderSide
  , uint32_t nOrderQuantity
  , pPosition_t
  )>;

  ComboTraits()
  : fLegCount( nullptr ), fChooseLegs( nullptr )
  , fFillLegNote( nullptr ), fName( nullptr )
  , fAddLegOrder( nullptr )
  {}

  // ==

  fLegCount_t fLegCount;
  fChooseLegs_t fChooseLegs;
  fFillLegNote_t fFillLegNote;
  fName_t fName;
  fAddLegOrder_t fAddLegOrder;

};

} // namespace option
} // namespace tf
} // namespace ou
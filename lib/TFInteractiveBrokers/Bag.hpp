/************************************************************************
 * Copyright(c) 2026, One Unified. All rights reserved.                 *
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
 * File:    Bag.hpp
 * Author:  raymond@burkholder.net
 * Project: TFInteractiveBrokers
 * Created: 2026/01/18 16:00:43
 */

#pragma once

#include <string>

#include <TFTrading/Order.h>
#include <TFTrading/TradingEnumerations.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace ib { // Interactive Brokers

class Bag {
public:
  size_t order_id;
  uint32_t quantity;
  std::string name;
  ou::tf::OrderType::EOrderType type;
  ou::tf::OrderSide::EOrderSide side;
  double price;
  std::string tif; // DAY, GTC, ...
  bool outside_rth;

  using vOrder_t = std::vector<ou::tf::Order::pOrder_t>;
  vOrder_t vOrder;

  Bag()
  : order_id {}, quantity {}
  , type( ou::tf::OrderType::Unknown )
  , side( ou::tf::OrderSide::Unknown )
  , price {}
  , tif( "DAY" )
  , outside_rth( false )
  {}

  Bag( size_t quantity_, const std::string& name_
  , ou::tf::OrderType::EOrderType type_
  , ou::tf::OrderSide::EOrderSide side_
  )
  : quantity( quantity_ ), name( name_ ), type( type_ ), side( side_ )
  , tif( "DAY" )
  , outside_rth( false )
  {}

  Bag( size_t quantity_, const std::string& name_
  , ou::tf::OrderType::EOrderType type_
  , ou::tf::OrderSide::EOrderSide side_
  , double price_
  )
  : quantity( quantity_ ), name( name_ ), type( type_ ), side( side_ ), price( price_ )
  , tif( "DAY" )
  , outside_rth( false )
{}

protected:
private:
};

} // namespace ib
} // namespace tf
} // namespace ou

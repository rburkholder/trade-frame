/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    TradeLifeTime.h
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: March 9, 2022 16:38
 */

#pragma once

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
  struct PanelOrderButtons_Order;
} // namespace tf
} // namespace ou

class TradeLifeTime {
public:

  using pPosition_t = ou::tf::Position::pPosition_t;

  TradeLifeTime( pPosition_t, const ou::tf::PanelOrderButtons_Order& );
  virtual ~TradeLifeTime();

protected:

  using pOrder_t = ou::tf::Order::pOrder_t;

  enum class EPositionState { Looking, Entering, Entered, Buying, Long, Selling, Short };

  EPositionState m_statePosition;

  pPosition_t m_pPosition;

  pOrder_t m_pOrderEntry;
  pOrder_t m_pOrderProfit;
  pOrder_t m_pOrderStop;

  void HandleOrderCancelled( const ou::tf::Order& );
  void HandleOrderFilled( const ou::tf::Order& );

private:

};

// =====

class TradeWithABuy: public TradeLifeTime {
public:
  TradeWithABuy( pPosition_t, const ou::tf::PanelOrderButtons_Order& );
protected:
private:
  void HandleOrderCancelled( const ou::tf::Order& );
  void HandleOrderFilled( const ou::tf::Order& );
};

// =====

class TradeWithASell: public TradeLifeTime {
public:
  TradeWithASell( pPosition_t, const ou::tf::PanelOrderButtons_Order& );
protected:
private:
  void HandleOrderCancelled( const ou::tf::Order& );
  void HandleOrderFilled( const ou::tf::Order& );
};

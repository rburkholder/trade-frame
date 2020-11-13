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
 * File:    MonitorOrder.h
 * Author:  raymond@burkholder.net
 * Project: TFTrading
 * Created on May 25, 2019, 1:44 PM
 */

#ifndef MONITORORDER_H
#define MONITORORDER_H

#include "Order.h"
#include "Position.h"

namespace ou {
namespace tf {

// devoted to one Order at a time, runs as limit order
// NOTE: will require a tick between close/cancel and new order
class MonitorOrder {
public:

  using pPosition_t = Position::pPosition_t;

  MonitorOrder();
  MonitorOrder( pPosition_t& pPosition );
  MonitorOrder( const MonitorOrder& rhs ); // TODO: work on cleaning this up
  MonitorOrder( const MonitorOrder&& rhs );

  void SetPosition( pPosition_t pPosition );

  // can only work on one order at a time
  bool PlaceOrder( boost::uint32_t nOrderQuantity, ou::tf::OrderSide::enumOrderSide side );
  void CancelOrder();
  void ClosePosition();

  void Tick( ptime dt );

  bool IsOrderActive() const;

private:

  using pOrder_t = Order::pOrder_t;

  enum class State { NoPosition, NoOrder, Active, Filled, Cancelled };
  State m_state;
  size_t m_CountDownToAdjustment;
  pPosition_t m_pPosition;
  pOrder_t m_pOrder;

  void UpdateOrder( ptime dt );

  void OrderCancelled( const ou::tf::Order& order );
  void OrderFilled( const ou::tf::Order& order );
};

} // namespace ou
} // namespace tf

#endif /* MONITORORDER_H */

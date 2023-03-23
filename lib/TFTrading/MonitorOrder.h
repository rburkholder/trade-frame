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

#ifndef OU_TF_MONITORORDER_H
#define OU_TF_MONITORORDER_H

/*
  * manages the creation, submission, and adjustment of a limit order
  * restricted to one order at a time
  * NOTE: will require a tick between close/cancel and new order for state to change
*/

#include "Order.h"
#include "Position.h"

namespace ou {
namespace tf {

class MonitorOrder {
public:

  using pPosition_t = Position::pPosition_t;

  MonitorOrder();
  MonitorOrder( MonitorOrder&& );
  MonitorOrder( pPosition_t& );
  MonitorOrder& operator=( MonitorOrder&& );
  MonitorOrder( const MonitorOrder& ) = delete; // TODO: work on cleaning this up

  void SetPosition( pPosition_t );

  bool PlaceOrder( boost::uint32_t nOrderQuantity, ou::tf::OrderSide::EOrderSide side );
  void CancelOrder();
  void ClosePosition();

  void Tick( ptime dt ); // one second interval

  bool IsActive() const;
  bool IsDone() const;

private:

  using pOrder_t = Order::pOrder_t;

  enum class State {
    NoPosition, // virgin state
    Available, // has position, no order created yet
    Active, // exchange based cancel for GTD orders
    ManualCancel, // no order resubmit
    Filled,  // provides state machine step to restart
    Cancelled // provides state machine step to restart
    };

  State m_state;
  bool m_bEnableStatsAdd;
  size_t m_CountDownToAdjustment;
  pPosition_t m_pPosition;
  pOrder_t m_pOrder;

  double PriceInterval( double price ) const;
  double NormalizePrice( double price ) const;

  void UpdateOrder( ptime dt );

  void EnableStatsRemove();

  void OrderCancelled( const ou::tf::Order& order );
  void OrderFilled( const ou::tf::Order& order );
};

} // namespace ou
} // namespace tf

#endif /* OU_TF_MONITORORDER_H */

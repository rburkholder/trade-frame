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
 * File:    PriceLevelOrder.hpp (from AppDoM)
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: 2022/11/27 12:41:04
 */

#pragma once

#include <TFTrading/Order.h>

class PriceLevelOrder {
public:

  using pOrder_t = ou::tf::Order::pOrder_t;
  using fUpdateQuantity_t = std::function<void(uint32_t)>;

  PriceLevelOrder();
  PriceLevelOrder( pOrder_t pOrder );
  PriceLevelOrder( PriceLevelOrder&& rhs );
  PriceLevelOrder& operator=( pOrder_t pOrder );

  ~PriceLevelOrder();

  pOrder_t Order() { return m_pOrder; }

  void Set( fUpdateQuantity_t&& fUpdateQuantity );

  void SetEvents();
  void ClearEvents();

  void HandleOnPartialFill( const ou::tf::Order& order );
  void HandleOnOrderFilled( const ou::tf::Order& order );
  void HandleOnOrderCancelled( const ou::tf::Order& order );
  void HandleOnOrderExecution( const std::pair<const ou::tf::Order&, const ou::tf::Execution&>& pair );

protected:

  pOrder_t m_pOrder;

  fUpdateQuantity_t m_fUpdateQuantity;

private:


}; // PriceLevelOrder

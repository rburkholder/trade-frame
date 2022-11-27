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
 * File:    ControlExec.hpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: 2022/11/21 14:59:32
 */

#pragma once

// overall controller for handling interface events and distributing requests to the models
// initiates orders and updates

// TODO: add stop orders

#include <map>

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>

#include "PriceLevelOrder.hpp"

namespace ou {
namespace tf {
namespace l2 {
  class PanelTrade;
}
}
}

class InteractiveChart;

class ControlExec {
public:

  using pOrder_t = ou::tf::Order::pOrder_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  ControlExec( pPosition_t, unsigned int nDefaultOrder );

  void Set( ou::tf::l2::PanelTrade* );
  void Set( InteractiveChart* );

protected:
private:

  pPosition_t m_pPosition;

  ou::tf::l2::PanelTrade* m_pPanelTrade;
  InteractiveChart* m_pInteractiveChart;

  unsigned int m_nDefaultOrder;

  // TODO: allow multiple orders per level
  using mapOrders_t = std::map<double,PriceLevelOrder>;
  // note: the exchange will complain if there are orders on both sides
  mapOrders_t m_mapAskOrders;
  mapOrders_t m_mapBidOrders;

  PriceLevelOrder m_KillPriceLevelOrder; // temporary for unrolling lambda call

};
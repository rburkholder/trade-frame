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
 * File:    Order_Combo.hpp
 * Author:  raymond@burkholder.net
 * Project: TFTrading
 * Created: March 22, 2023 13:38:52
 */

#pragma once

#include <vector>
#include <functional>

#include "MonitorOrder.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class OrderCombo_TrackLeg {
public:

  using pPosition_t = Position::pPosition_t;

  using fLegDone_t = std::function<void()>;

  OrderCombo_TrackLeg( pPosition_t, uint32_t nQuantity_, ou::tf::OrderSide::EOrderSide, fLegDone_t&& );
  OrderCombo_TrackLeg( pPosition_t, fLegDone_t&& );

  void Submit();

  bool Tick( ptime dt ); // one second interval

  void Cancel();

protected:
private:

  enum class State { leg_add, leg_close, active, done } state;

  pPosition_t pPosition;
  uint32_t nQuantity;
  ou::tf::OrderSide::EOrderSide side;
  fLegDone_t fLegDone;

  MonitorOrder mo;

};

// ==================

class OrderCombo {
public:

  OrderCombo();
  ~OrderCombo();

  using pOrderCombo_t = std::shared_ptr<OrderCombo>;

  using pPosition_t = Position::pPosition_t;

  using fLegDone_t = OrderCombo_TrackLeg::fLegDone_t;
  using fComboDone_t = std::function<void()>;

  static pOrderCombo_t Factory() { return std::make_shared<OrderCombo>(); }

  void AddLeg( pPosition_t, uint32_t nOrderQuantity, ou::tf::OrderSide::EOrderSide, fLegDone_t&& );
  void CloseLeg( pPosition_t, fLegDone_t&& );

  void Submit( fComboDone_t&& );
  void Cancel( fComboDone_t&& );

  void Tick( ptime dt ); // one second interval

protected:
private:

  enum class EState { bare, loading, placing, cancel, monitoring, done } m_state;

  using vTrack_t = std::vector<OrderCombo_TrackLeg>;
  vTrack_t m_vTrack;

  fComboDone_t m_fComboDone; // TODO: needs a status:  filled, partial fill, all cancelled
};

} // namespace tf
} // namespace ou

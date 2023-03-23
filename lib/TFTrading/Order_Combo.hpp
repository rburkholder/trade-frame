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

//#include <map>
#include <vector>
#include <functional>

#include "MonitorOrder.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class OrderCombo {
public:

  using pOrder_t = Order::pOrder_t;
  using pPosition_t = Position::pPosition_t;

  using fLegDone_t = std::function<void()>;
  using fComboDone_t = std::function<void()>;

  OrderCombo();
  ~OrderCombo();

  void AddLeg( pPosition_t, uint32_t nOrderQuantity, ou::tf::OrderSide::EOrderSide, fLegDone_t&& );
  void CloseLeg( pPosition_t, fLegDone_t&& );

  void Submit( fComboDone_t&& );

  void Tick( ptime dt ); // one second interval

protected:
private:

  enum class EState { bare, loading, placing, monitoring, done } m_state;

  struct Track {

    enum class State { leg_add, leg_close, active, done } state;

    pPosition_t pPosition;
    uint32_t nQuantity;
    ou::tf::OrderSide::EOrderSide side;
    fLegDone_t fLegDone;

    MonitorOrder mo;

    Track( pPosition_t pPosition_, uint32_t nQuantity_, ou::tf::OrderSide::EOrderSide side_, fLegDone_t&& fLegDone_ )
    : state( State::leg_add )
    , pPosition( pPosition_ ), nQuantity( nQuantity_ ), side( side_ ), fLegDone( std::move( fLegDone_ ) )
    , mo( pPosition_ )
    {}

    Track( pPosition_t pPosition_, fLegDone_t&& fLegDone_ )
    : state( State::leg_close )
    , pPosition( pPosition_ ), nQuantity {}, fLegDone( std::move( fLegDone_ ) )
    , mo( pPosition_ )
    {}
  };

  using vTrack_t = std::vector<Track>;
  vTrack_t m_vTrack;

  fComboDone_t m_fComboDone;
};

} // namespace tf
} // namespace ou

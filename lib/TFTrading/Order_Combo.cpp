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
 * File:    Order_Combo.cpp
 * Author:  raymond@burkholder.net
 * Project: TFTrading
 * Created: March 22, 2023 13:38:52
 */

/*
  Goal:
    * collect orders for a combo, monitor for execution, notify on completion
    * https://interactivebrokers.github.io/tws-api/classIBApi_1_1Order.html
    * add WaitForSpreadStats to stabilize

  Note:
    * IB supports OPT, STK, FUT, not FOP
      * https://interactivebrokers.github.io/tws-api/basic_orders.html#combolimit
      * https://interactivebrokers.github.io/tws-api/order_submission.html#order_attach
    * therefore submit FuturesOptions as individual orders  for now
    * but give it a try sometime, once integration with Position is integrated

  For Later:
    * https://interactivebrokers.github.io/tws-api/oca.html#oca_types One Cancels All
*/

#include "Order_Combo.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

OrderCombo::OrderCombo()
: m_state( EState::bare )
{
}

OrderCombo::~OrderCombo() {
}

void OrderCombo::AddLeg( pPosition_t pPosition, uint32_t nOrderQuantity, ou::tf::OrderSide::EOrderSide side, fLegDone_t&& fLegDone ) {
  switch ( m_state ) {
    case EState::bare:
      m_state = EState::loading;
      // fall through
    case EState::loading: {
      Track& track( m_vTrack.emplace_back( std::move( Track( std::move( pPosition), nOrderQuantity, side, std::move( fLegDone ) ) ) ) );
      }
      break;
    case EState::placing:
    case EState::monitoring:
    case EState::done:
      assert( false );
      break;
  }
    }

void OrderCombo::Submit( fComboDone_t&& fComboDone ) {

  assert( EState::loading == m_state );
  m_state = EState::placing;

  m_fComboDone = std::move( fComboDone );

  for ( vTrack_t::value_type& entry: m_vTrack ) {
    switch ( entry.state ) {
      case Track::State::leg_add:
        entry.mo.PlaceOrder( entry.nQuantity, entry.side );
        break;
      case Track::State::leg_close:
        entry.mo.ClosePosition();
        break;
      default:
        assert( false );
    }
    entry.state = Track::State::active;
  }

  m_state = EState::monitoring;

}

void OrderCombo::Tick( ptime dt ) {
  switch ( m_state ) {
    case EState::monitoring: {
      bool bDone( true );
      for ( vTrack_t::value_type& entry: m_vTrack ) {
        entry.mo.Tick( dt );
        switch ( entry.state ) {
          case Track::State::leg_add:
          case Track::State::leg_close:
            break;
          case Track::State::active:
            if ( entry.mo.IsActive() ) {
              bDone = false;
            }
            else {
              entry.state = Track::State::done;
              entry.fLegDone();
            }
            break;
          case Track::State::done:
            break;
        } // switch( state )
      } // for( entry )
      if ( bDone ) {
        m_fComboDone();
      }
      }
      break;
    default:
      break;
  }

}


} // namespace tf
} // namespace ou

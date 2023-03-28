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
    * therefore submit FuturesOptions as individual orders for now
    * but give it a try sometime, once integration with Position is available

  For Later:
    * https://interactivebrokers.github.io/tws-api/oca.html#oca_types One Cancels All
*/

#include "Order_Combo.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

OrderCombo_TrackLeg::OrderCombo_TrackLeg( pPosition_t pPosition_, uint32_t nQuantity_, ou::tf::OrderSide::EOrderSide side_, fLegDone_t&& fLegDone_ )
: state( State::leg_add )
, pPosition( pPosition_ ), nQuantity( nQuantity_ ), side( side_ ), fLegDone( std::move( fLegDone_ ) )
, mo( pPosition_ )
{}

OrderCombo_TrackLeg::OrderCombo_TrackLeg( pPosition_t pPosition_, fLegDone_t&& fLegDone_ )
: state( State::leg_close )
, pPosition( pPosition_ ), nQuantity {}, fLegDone( std::move( fLegDone_ ) )
, mo( pPosition_ )
{}

void OrderCombo_TrackLeg::Submit() {

  switch ( state ) {
    case State::leg_add:
      mo.PlaceOrder( nQuantity, side );
      break;
    case OrderCombo_TrackLeg::State::leg_close:
      mo.ClosePosition();
      break;
    default:
      assert( false );
  }
  state = State::active;
}

bool OrderCombo_TrackLeg::Tick( ptime dt ) {

  bool bDone( true ); // done once legs have transitioned away from 'active'

  mo.Tick( dt );

  switch ( state ) {
    case State::leg_add:
    case State::leg_close:
      break;
    case State::active:
      if ( mo.IsActive() ) {
        bDone = false;
      }
      else {
        state = State::done;
        fLegDone();
      }
      break;
    case State::done:
      break;
  } // switch( state )
  return bDone;
}

void OrderCombo_TrackLeg::Cancel() {
  mo.CancelOrder();
}

// ==================

OrderCombo::OrderCombo()
: m_state( EState::bare )
, m_fComboDone( nullptr )
{
}

OrderCombo::~OrderCombo() {
  m_vTrack.clear();
  m_fComboDone = nullptr;
}

void OrderCombo::AddLeg( pPosition_t pPosition, uint32_t nOrderQuantity, ou::tf::OrderSide::EOrderSide side, fLegDone_t&& fLegDone ) {
  switch ( m_state ) {
    case EState::bare:
      m_state = EState::loading;
      // fall through
    case EState::loading: {
      // implicit place order
      OrderCombo_TrackLeg& track( m_vTrack.emplace_back( std::move( OrderCombo_TrackLeg( std::move( pPosition), nOrderQuantity, side, std::move( fLegDone ) ) ) ) );
      }
      break;
    case EState::placing:
    case EState::monitoring:
    case EState::done:
      assert( false );
      break;
  }
}

void OrderCombo::CloseLeg( pPosition_t pPosition, fLegDone_t&& fLegDone ) {
  switch ( m_state ) {
    case EState::bare:
      m_state = EState::loading;
      // fall through
    case EState::loading: {
      // implicit close leg
      OrderCombo_TrackLeg& track( m_vTrack.emplace_back( std::move( OrderCombo_TrackLeg( std::move( pPosition), std::move( fLegDone ) ) ) ) );
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
    entry.Submit();
  }

  m_state = EState::monitoring;

}

void OrderCombo::Tick( ptime dt ) {
  switch ( m_state ) {
    case EState::cancel: // need a tick or two to finish
    case EState::monitoring: {
      bool bDone( true );
      for ( vTrack_t::value_type& entry: m_vTrack ) {
        bDone &= entry.Tick( dt );
      }
      if ( bDone ) {
        m_fComboDone();
        m_state = EState::done;
      }
      }
      break;
    default:
      break;
  }
}

// may have a timing issue with the Active transition in MonitorOrder on each Tick
void OrderCombo::Cancel( fComboDone_t&& fComboDone ) {
  switch ( m_state ) {
    case EState::monitoring:
      m_state = EState::cancel;
      m_fComboDone = std::move( fComboDone ); // overwrite previous flavour -
      for ( vTrack_t::value_type& vt: m_vTrack ) {
        vt.Cancel(); // TODO: this needs to force the completion
      }
      break;
    default:
      std::cout << "OrderCombo::Cancel is out of state" << std::endl;
      break;
  }
}

} // namespace tf
} // namespace ou

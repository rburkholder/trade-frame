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
 * File:    MonitorOrder.cpp
 * Author:  raymond@burkholder.net
 * Project: TFTrading
 * Created on May 25, 2019, 1:44 PM
 */

#include "MonitorOrder.h"

namespace ou {
namespace tf {

MonitorOrder::MonitorOrder():
  m_nAdjustmentPeriods( 2 ),
  m_CountDownToAdjustment {}
  {}

MonitorOrder::MonitorOrder( pPosition_t& pPosition )
: m_nAdjustmentPeriods( 2 ),
  m_CountDownToAdjustment {},
  m_state( State::NoPosition ),
  m_pPosition( pPosition )
{}

MonitorOrder::MonitorOrder( const MonitorOrder&& rhs )
: m_nAdjustmentPeriods( rhs.m_nAdjustmentPeriods ),
  m_CountDownToAdjustment( rhs.m_CountDownToAdjustment ),
  m_state( rhs.m_state ),
  m_pPosition( std::move( rhs.m_pPosition ) ),
  m_pOrder( std::move( rhs.m_pOrder ) )
{}

void MonitorOrder::SetPosition( pPosition_t pPosition ) {
  m_pPosition = pPosition;
  m_state = State::NoOrder;
}

// can only work on one order at a time
bool MonitorOrder::PlaceOrder( boost::uint32_t nOrderQuantity, ou::tf::OrderSide::enumOrderSide side ) {
  bool bOk( false );
  switch ( m_state ) {
    case State::NoOrder:
    case State::Cancelled:  // can overwrite?
    case State::Filled:     // can overwrite?
      {
        double mid = m_pPosition->GetWatch()->LastQuote().Midpoint();
        double dblNormalizedPrice = m_pPosition->GetInstrument()->NormalizeOrderPrice( mid );
        m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, side, nOrderQuantity, dblNormalizedPrice );
        if ( m_pOrder ) {
          m_pOrder->SetSignalPrice( dblNormalizedPrice );
          m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &MonitorOrder::OrderFilled ) );
          m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &MonitorOrder::OrderCancelled ) );
          m_CountDownToAdjustment = m_nAdjustmentPeriods;
          m_state = State::Active;
          m_pPosition->PlaceOrder( m_pOrder );
          std::cout << m_pPosition->GetInstrument()->GetInstrumentName() << ": placed at " << dblNormalizedPrice << std::endl;
          bOk = true;
        }
      }
      break;
    case State::Active:
      std::cout << m_pPosition->GetInstrument()->GetInstrumentName() << ": active, cannot place order" << std::endl;
      break;
    case State::NoPosition:
      break;
  }
  return bOk;
}

void MonitorOrder::CancelOrder() {  // TODO: need to fix this, and take the Order out of UpdateOrder
  switch ( m_state ) {
    case State::Active:
      m_pPosition->CancelOrder( m_pOrder->GetOrderId() );
      break;
    case State::NoOrder:
    case State::Cancelled:
    case State::Filled:
    case State::NoPosition:
      break;
  }
}

void MonitorOrder::Tick() {
  switch ( m_state ) {
    case State::Active:
      UpdateOrder();
      break;
    case State::Cancelled:
    case State::Filled:
      m_pOrder.reset();
      m_state = State::NoOrder;
      break;
    case State::NoOrder:
    case State::NoPosition:
      break;
  }
}

bool MonitorOrder::IsOrderActive() const { return ( State::Active == m_state ); }

void MonitorOrder::UpdateOrder() { // true when order has been filled
  if ( 0 == m_pOrder->GetQuanRemaining() ) { // not sure if a cancel adjusts remaining
    // TODO: generate message? error on filled, but may be present on cancel
  }
  else {
    assert( 0 < m_CountDownToAdjustment );
    m_CountDownToAdjustment--;
    bool bUpdateOrder( false );
    if ( 0 == m_CountDownToAdjustment ) {
      switch ( m_pOrder->GetOrderSide() ) {
        case ou::tf::OrderSide::Buy:
          // TODO: maximum number of increments? aka don't chase too far?
          m_pOrder->SetPrice1( m_pOrder->GetPrice1() + 0.01 );
          bUpdateOrder = true;
          break;
        case ou::tf::OrderSide::Sell:
          if ( 0.03 < m_pOrder->GetPrice1() ) {
            m_pOrder->SetPrice1( m_pOrder->GetPrice1() - 0.01 );
            bUpdateOrder = true;
          }
          else {
            m_CountDownToAdjustment = m_nAdjustmentPeriods;
          }
          break;
      }
      // TODO: need to cancel both legs if spread is not < 0.10
      if ( bUpdateOrder ) {
        const ou::tf::Quote& quote( m_pPosition->GetWatch()->LastQuote() );
        double spread = quote.Spread();
        std::cout
          << m_pPosition->GetInstrument()->GetInstrumentName()
          << ": update order to " << m_pOrder->GetPrice1()
          //<< " on " << dblNormalizedPrice
          << " spread " << spread
          << std::endl;
        m_pPosition->UpdateOrder( m_pOrder );
        m_CountDownToAdjustment = m_nAdjustmentPeriods;
      }
    }
  }
}

void MonitorOrder::OrderCancelled( const ou::tf::Order& order ) { // TODO: delegate should have const removed?
  switch ( m_state ) {
    case State::Active:
      m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &MonitorOrder::OrderCancelled ) );
      m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &MonitorOrder::OrderFilled ) );
      m_state = State::Cancelled;
      break;
  }
}
void MonitorOrder::OrderFilled( const ou::tf::Order& order ) { // TODO: delegate should have const removed?
  switch ( m_state ) {
    case State::Active:
      m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &MonitorOrder::OrderCancelled ) );
      m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &MonitorOrder::OrderFilled ) );
      m_state = State::Filled;
      break;
  }
}

} // namespace ou
} // namespace tf
  
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

namespace {
  const size_t nAdjustmentPeriods( 3 );
}

MonitorOrder::MonitorOrder()
: m_CountDownToAdjustment {},
  m_state( State::NoPosition )
  {}

MonitorOrder::MonitorOrder( pPosition_t& pPosition )
: m_CountDownToAdjustment {},
  m_state( State::NoOrder ),
  m_pPosition( pPosition )
{}

// what checks to perform on m_state?  need to be in a good state for things to sync properly
//assert( !m_pPosition ); // let us see where this goes, may raise an issue with the constructor initialized with position
//assert( !m_pOrder ); // this causes issues if duplicated
MonitorOrder::MonitorOrder( MonitorOrder&& rhs )
: m_CountDownToAdjustment( rhs.m_CountDownToAdjustment ),
  m_state( rhs.m_state ),
  m_pPosition( std::move( rhs.m_pPosition ) ),
  m_pOrder( std::move( rhs.m_pOrder ) )
{}

MonitorOrder& MonitorOrder::operator=( const MonitorOrder&& rhs ) {
  if ( this != &rhs ) {
    m_CountDownToAdjustment = rhs.m_CountDownToAdjustment;
    m_state = rhs.m_state;
    m_pPosition = std::move( rhs.m_pPosition ),
    m_pOrder = std::move( rhs.m_pOrder );
  }
  return *this;
}

void MonitorOrder::SetPosition( pPosition_t pPosition ) {
  //assert( !m_pPosition );
  assert( !m_pOrder ); // no outstanding orders should exist
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
          m_state = State::Active;
          m_pOrder->SetSignalPrice( dblNormalizedPrice );
          m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &MonitorOrder::OrderFilled ) );
          m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &MonitorOrder::OrderCancelled ) );
          m_CountDownToAdjustment = nAdjustmentPeriods;
          m_pPosition->PlaceOrder( m_pOrder );
          std::cout
            << m_pOrder->GetOrderId() << " "
            << m_pOrder->GetInstrument()->GetInstrumentName() << ":"
            << m_pOrder->GetDateTimeOrderSubmitted().time_of_day() << " "
            << m_pOrder->GetOrderSideName()
            << " placed at " << dblNormalizedPrice
            << " monitored"
            << std::endl;
          bOk = true;
        }
        else {
          m_state = State::NoOrder;
          std::cout
            << "MonitorOrder::PlaceOrder: "
            << m_pPosition->GetInstrument()->GetInstrumentName() << " failed to construct order"
            << std::endl;
        }
      }
      break;
    case State::Active:
      std::cout << "MonitorOrder::PlaceOrder: " << m_pPosition->GetInstrument()->GetInstrumentName() << ": active, cannot place order" << std::endl;
      break;
    case State::NoPosition:
      std::cout << "MonitorOrder::PlaceOrder: " << m_pPosition->GetInstrument()->GetInstrumentName() << ": no position" << std::endl;
      break;
  }
  return bOk;
}

void MonitorOrder::ClosePosition() {
  if ( m_pPosition ) {
    const ou::tf::Position::TableRowDef& row( m_pPosition->GetRow() );
    if ( IsOrderActive() ) {
      std::cout << row.sName << ": error, monitor has active order, no close possible" << std::endl;
    }
    else {
      if ( 0 != row.nPositionPending ) {
        std::cout << row.sName << ": warning, has pending size of " << row.nPositionPending << " during close" << std::endl;
      }
      if ( 0 != row.nPositionActive ) {
        std::cout << row.sName << ": monitored closing position, side=" << row.eOrderSideActive << ", q=" << row.nPositionActive << std::endl;
        switch ( row.eOrderSideActive ) {
          case ou::tf::OrderSide::Buy:
            PlaceOrder( row.nPositionActive, ou::tf::OrderSide::Sell );
            break;
          case ou::tf::OrderSide::Sell:
            PlaceOrder( row.nPositionActive, ou::tf::OrderSide::Buy );
            break;
        }
      }
    }
  }
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

void MonitorOrder::Tick( ptime dt ) {
  switch ( m_state ) {
    case State::Active:
      UpdateOrder( dt );
      break;
    case State::Cancelled:
    case State::Filled:
      //m_pOrder.reset();
      m_state = State::NoOrder;
      // TODO: clear position?
      break;
    case State::NoOrder:
    case State::NoPosition:
      break;
  }
}

bool MonitorOrder::IsOrderActive() const { return ( State::Active == m_state ); }

void MonitorOrder::UpdateOrder( ptime dt ) { // true when order has been filled

  auto tod = dt.time_of_day();

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
            m_CountDownToAdjustment = nAdjustmentPeriods;
          }
          break;
      }
      // TODO: need to cancel both legs if spread is not < 0.10
      if ( bUpdateOrder ) {
        const ou::tf::Quote& quote( m_pPosition->GetWatch()->LastQuote() );
        double spread = quote.Spread();
        std::cout
          << tod << " "
          << m_pPosition->GetInstrument()->GetInstrumentName()
          << ": update order to " << m_pOrder->GetPrice1()
          //<< " on " << dblNormalizedPrice
          << " spread " << spread
          << std::endl;
        m_pPosition->UpdateOrder( m_pOrder );
        m_CountDownToAdjustment = nAdjustmentPeriods;
      }
    }
  }
}

void MonitorOrder::OrderCancelled( const ou::tf::Order& order ) { // TODO: delegate should have const removed?
  auto tod = order.GetDateTimeOrderFilled().time_of_day();
  switch ( m_state ) {
    case State::Active:
      assert( order.GetOrderId() == m_pOrder->GetOrderId() );
      m_state = State::Cancelled;
      m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &MonitorOrder::OrderCancelled ) );
      m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &MonitorOrder::OrderFilled ) );
      std::cout
        << tod << " "
        << order.GetOrderId() << " "
        << order.GetInstrument()->GetInstrumentName()
        << ": cancelled"
        << std::endl;
      m_pOrder.reset();
      break;
    default:
      std::cout
        << tod << " "
        << order.GetOrderId() << " "
        << order.GetInstrument()->GetInstrumentName()
        << ": cancelled has no matching state (" << (int)m_state << ")"
        << std::endl;
  }
}
void MonitorOrder::OrderFilled( const ou::tf::Order& order ) { // TODO: delegate should have const removed?
  auto tod = order.GetDateTimeOrderFilled().time_of_day();
  switch ( m_state ) {
    case State::Active:
      assert( order.GetOrderId() == m_pOrder->GetOrderId() );
      m_state = State::Filled;
      m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &MonitorOrder::OrderCancelled ) );
      m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &MonitorOrder::OrderFilled ) );
      std::cout
        << tod << " "
        << order.GetInstrument()->GetInstrumentName()
        << ": filled at " << m_pOrder->GetAverageFillPrice()
        << std::endl;
      m_pOrder.reset();
      break;
    default:
      std::cout
        << tod << " "
        << order.GetOrderId() << " "
        << order.GetInstrument()->GetInstrumentName()
        << ": fillled has no matching state (" << (int)m_state << ")"
        << std::endl;
  }

}

} // namespace ou
} // namespace tf

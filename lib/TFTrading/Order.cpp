/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <cassert>

#include <OUCommon/TimeSource.h>

#include "Order.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

Order::Order( // market order
  Instrument::pInstrument_cref pInstrument
, OrderType::EOrderType eOrderType
, OrderSide::EOrderSide eOrderSide
, boost::uint32_t nOrderQuantity
, idPosition_t idPosition
, ptime dtOrderSubmitted
  )
:
  m_row( idPosition, pInstrument->GetInstrumentName(), eOrderType, eOrderSide, nOrderQuantity, dtOrderSubmitted )
, m_pInstrument( pInstrument )
, m_dblPriceXQuantity( 0 ), m_dblIncrementalCommission( 0.0 )
, m_nNextExecutionId ( 0 )
{
  ConstructOrder();
}

Order::Order( // limit or stop
  Instrument::pInstrument_cref pInstrument
, OrderType::EOrderType eOrderType
, OrderSide::EOrderSide eOrderSide
, boost::uint32_t nOrderQuantity
, double dblPrice1
, idPosition_t idPosition
, ptime dtOrderSubmitted
  )
:
  m_row( idPosition, pInstrument->GetInstrumentName(), eOrderType, eOrderSide, nOrderQuantity, dblPrice1, dtOrderSubmitted )
, m_pInstrument( pInstrument )
, m_dblPriceXQuantity( 0 ), m_dblIncrementalCommission( 0.0 )
, m_nNextExecutionId ( 0 )
{
  ConstructOrder();
}

Order::Order( // limit and stop
  Instrument::pInstrument_cref pInstrument
, OrderType::EOrderType eOrderType
, OrderSide::EOrderSide eOrderSide
, boost::uint32_t nOrderQuantity
, double dblPrice1, double dblPrice2
, idPosition_t idPosition
, ptime dtOrderSubmitted
  )
:
  m_row( idPosition, pInstrument->GetInstrumentName(), eOrderType, eOrderSide, nOrderQuantity, dblPrice1, dblPrice2, dtOrderSubmitted )
, m_pInstrument( pInstrument )
, m_dblPriceXQuantity( 0 ),  m_dblIncrementalCommission( 0.0 )
, m_nNextExecutionId ( 0 )
{
  ConstructOrder();
}

Order::Order( const TableRowDef& row, pInstrument_t& pInstrument  )
: m_row( row ), m_pInstrument( pInstrument )
, m_dblPriceXQuantity( 0 ),  m_dblIncrementalCommission( 0.0 )
, m_nNextExecutionId ( 0 )
{}

Order::~Order() {
}

void Order::ConstructOrder() {
//  try {

  // need to do something with idInstrument, into and out of the database
  // will be dealt with by OrderManager

  m_row.dtOrderCreated = ou::TimeSource::LocalCommonInstance().Internal();
  //assert( nullptr != m_pInstrument ); // irrelevant as the name is dereferenced
  // order id needs to be set externally, as it is dependent upon external factors
//  m_nOrderId = m_persistedorderid.GetNextOrderId();
//  }
//  catch (...) {
//    bOrderIdOk = false;
//    std::cout << "CIBTWS::PlaceOrder: Couldn't get the next order key." << std::endl;
//  }
}

void Order::SetSendingToProvider() {
  assert( OrderStatus::Created == m_row.eOrderStatus );
  m_row.eOrderStatus = OrderStatus::SendingToProvider;
  m_row.dtOrderSubmitted = ou::TimeSource::LocalCommonInstance().Internal();
}

OrderStatus::EOrderStatus Order::ReportExecution(const Execution &exec) {
  // need to worry about fill after cancel, has multiple states:  canceling, fill during cancel, canceled
  assert( exec.GetOrderSide() == m_row.eOrderSide );
  bool bOverDone = false;
  if ( 0 == m_row.nQuantityRemaining ) {
    // yes this has happened, 2008/07/09 vmw
    std::cout << "Order " << m_row.idOrder << " overfilled with +" << exec.GetSize() << std::endl;
    m_row.eOrderStatus = OrderStatus::OverFilled;
    bOverDone = true;
  }
  else {
    assert( exec.GetSize() <= m_row.nQuantityRemaining );
    m_row.nQuantityRemaining -= exec.GetSize();
  }
  m_row.nQuantityFilled += exec.GetSize();
  if ( m_row.nQuantityFilled > m_row.nOrderQuantity ) {
    std:: cout << "Order " << m_row.idOrder << " overfilled with +" << exec.GetSize() << std::endl;
    bOverDone = true;
  }
//  if ( !bOverDone ) {  // position should actually reflect overages so things can be seen and handled
    m_dblPriceXQuantity += exec.GetPrice() * exec.GetSize();
    m_row.dblAverageFillPrice = m_dblPriceXQuantity / m_row.nQuantityFilled;
    if ( 0 == m_row.nQuantityRemaining ) {
      switch ( m_row.eOrderStatus ) {
      case OrderStatus::CancelSubmitted:
      case OrderStatus::Cancelled:
        m_row.eOrderStatus = OrderStatus::CancelledWithPartialFill;
        break;
      case OrderStatus::Filling:
      case OrderStatus::SendingToProvider:
        m_row.eOrderStatus = OrderStatus::Filled;
        break;
      default:
        m_row.eOrderStatus = OrderStatus::Filled;
        break;
      }
      m_row.dtOrderClosed = ou::TimeSource::LocalCommonInstance().Internal();
//      OnCommission( *this ); // commission is reported separately
      OnOrderFilled( *this );
    }
    else {
      switch ( m_row.eOrderStatus ) {
      case OrderStatus::SendingToProvider:
      case OrderStatus::Submitted:
      case OrderStatus::Filling:
      case OrderStatus::PreSubmission:
        m_row.eOrderStatus = OrderStatus::Filling;
        break;
      case OrderStatus::Cancelled:
      case OrderStatus::CancelSubmitted:
      case OrderStatus::FillingDuringCancel:
      case OrderStatus::CancelledWithPartialFill:
        m_row.eOrderStatus = OrderStatus::FillingDuringCancel;
        break;
      case OrderStatus::OverFilled:
        break;
      default:
        std::cout << "Order::ReportExecution " << static_cast<char>( m_row.eOrderStatus ) << std::endl;
        break;
      }
      OnPartialFill( *this );
    }
//  }
  OnExecution( std::pair<const Order&, const Execution&>( *this, exec ) );
  return m_row.eOrderStatus;
}

void Order::ActOnError(OrderError::EOrderError eError) {
  switch( eError ) {
    case OrderError::Cancelled:
      m_row.eOrderStatus = OrderStatus::Cancelled;
      break;
    case OrderError::Rejected:
    case OrderError::InstrumentNotFound:
      m_row.eOrderStatus = OrderStatus::Rejected;
      break;
    case OrderError::NotCancellable:
      break;
  }
}

void Order::SetCommission( double dblCommission ) {  // total accumulated commission to this point
  m_dblIncrementalCommission = dblCommission - m_row.dblCommission;
  m_row.dblCommission = dblCommission;
  OnCommission( *this );  // run on order completion
}

void Order::SetOrderId( idOrder_t id ) {
  assert( 0 != id );
  assert( m_row.idOrder == 0 );
  m_row.idOrder = id;
}

void Order::MarkAsCancelled() {
  switch ( m_row.eOrderStatus ) {
  case OrderStatus::Created:
    m_row.eOrderStatus = OrderStatus::Cancelled;
    break;
  case OrderStatus::SendingToProvider:
  case OrderStatus::Submitted:
    m_row.eOrderStatus = 0 == m_row.nQuantityFilled ? OrderStatus::Cancelled : OrderStatus::CancelledWithPartialFill;
    break;
  case OrderStatus::Filling:
    m_row.eOrderStatus = OrderStatus::CancelledWithPartialFill;
    break;
  case OrderStatus::FillingDuringCancel:  // these ones should not be reachable
  case OrderStatus::Cancelled:
  case OrderStatus::CancelledWithPartialFill:
  case OrderStatus::CancelSubmitted:
    m_row.eOrderStatus = OrderStatus::Cancelled;
    break;
  case OrderStatus::Filled:
    m_row.eOrderStatus = OrderStatus::Filled;  // this one re-affirms, as cancel was too late
    break;
  default:
    std::cout << "Order::MarkAsCancelled unhandled order status: " << m_row.eOrderStatus << std::endl;
    break;
  }
  OnOrderCancelled( *this );
}

} // namespace tf
} // namespace ou

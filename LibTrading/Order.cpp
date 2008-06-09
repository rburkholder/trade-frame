#include "StdAfx.h"
#include "Order.h"

COrder::COrder(void) {
}

COrder::COrder( 
    CInstrument *instrument,
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    ptime dtOrderSubmitted
    ) :
    m_pInstrument( instrument ), m_eOrderType( eOrderType ),
    m_eOrderSide( eOrderSide ), m_nOrderQuantity( nOrderQuantity ),
    m_dtOrderSubmitted( dtOrderSubmitted ),
    m_eOrderStatus( OrderStatus::Created ),
    m_nNextExecutionId ( 0 ),
    m_dblCommission( 0 ),
    m_dblPrice1( 0 ), m_dblPrice2( 0 ), m_bOutsideRTH( false )
{
  AssignOrderId();
}

COrder::COrder( 
    CInstrument *instrument,
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    double dblPrice1,
    ptime dtOrderSubmitted
    ) :
    m_pInstrument( instrument ), m_eOrderType( eOrderType ),
    m_eOrderSide( eOrderSide ), m_nOrderQuantity( nOrderQuantity ),
    m_dtOrderSubmitted( dtOrderSubmitted ), 
    m_eOrderStatus( OrderStatus::Created ),
    m_nNextExecutionId ( 0 ),
    m_dblCommission( 0 ),
    m_dblPrice1( dblPrice1 ), m_dblPrice2( 0 ), m_bOutsideRTH( false )
{
  AssignOrderId();
}

COrder::COrder( 
    CInstrument *instrument,
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    double dblPrice1, double dblPrice2,
    ptime dtOrderSubmitted
    ) :
    m_pInstrument( instrument ), m_eOrderType( eOrderType ),
    m_eOrderSide( eOrderSide ), m_nOrderQuantity( nOrderQuantity ),
    m_dtOrderSubmitted( dtOrderSubmitted ), 
    m_eOrderStatus( OrderStatus::Created ),
    m_nNextExecutionId ( 0 ),
    m_dblCommission( 0 ),
    m_dblPrice1( dblPrice1 ), m_dblPrice2( dblPrice2 ), m_bOutsideRTH( false )
{
  AssignOrderId();
}

COrder::~COrder(void) {
}

void COrder::AssignOrderId() {
//  try {
    m_nOrderId = m_persistedorderid.GetNextOrderId();
//  }
//  catch (...) {
//    bOrderIdOk = false;
//    std::cout << "CIBTWS::PlaceOrder: Couldn't get the next order key." << std::endl;
//  }
}

void COrder::SetSendingToProvider() {
  assert( OrderStatus::Created == m_eOrderStatus );
  m_eOrderStatus = OrderStatus::SendingToProvider;
}

OrderStatus::enumOrderStatus COrder::ReportExecution(const CExecution &exec) { 
  // need to worry about fill after cancel
  assert( exec.GetOrderSide() == m_eOrderSide );
  assert( exec.GetOrderId() == m_nOrderId );
  if ( 0 == m_nRemaining ) {
    std::cout << "Order " << m_nOrderId << " overfilled with +" << exec.GetSize() << std::endl;
    m_eOrderStatus = OrderStatus::OverFilled;
  }
  else {
    m_nRemaining -= exec.GetSize();
  }
  m_nFilled += exec.GetSize();
  m_dblPriceQuantity += exec.GetPrice() * exec.GetSize();
  m_dblAverageFillPrice = m_dblPriceQuantity / m_nFilled;
  if ( 0 == m_nRemaining ) {
    m_eOrderStatus = OrderStatus::Filled;
    OnOrderFilled( this );
  }
  else {
    switch ( m_eOrderStatus ) {
      case OrderStatus::SendingToProvider:
      case OrderStatus::Submitted:
      case OrderStatus::Filling:
      case OrderStatus::PreSubmission:
        m_eOrderStatus = OrderStatus::Filling;
        break;
      case OrderStatus::Cancelled:
      case OrderStatus::CancelSubmitted:
      case OrderStatus::FillingDuringCancel:
      case OrderStatus::CancelledWithPartialFill:
        m_eOrderStatus = OrderStatus::FillingDuringCancel;
        break;
      case OrderStatus::OverFilled:
        break;
      default:
        std::cout << "COrder::ReportExecution " << m_eOrderStatus << std::endl;
        break;
    }
  }
  return m_eOrderStatus;
}

void COrder::ActOnError(OrderErrors::enumOrderErrors eError) {
  switch( eError ) {
    case OrderErrors::Cancelled:
      m_eOrderStatus = OrderStatus::Cancelled;
      break;
    case OrderErrors::Rejected:
    case OrderErrors::InstrumentNotFound:
      m_eOrderStatus = OrderStatus::Rejected;
      break;
    case OrderErrors::NotCancellable:
      break;
  }
}
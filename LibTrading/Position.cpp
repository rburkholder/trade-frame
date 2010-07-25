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

#include "StdAfx.h"

#include "Position.h"
#include <LibTrading/OrderManager.h>

CPosition::CPosition( pInstrument_ref pInstrument, pProvider_ref pExecutionProvider, pProvider_ref pDataProvider ) 
: m_pExecutionProvider( pExecutionProvider ), m_pDataProvider( pDataProvider ), 
  m_pInstrument( pInstrument ), 
  m_nPositionPending( 0 ), m_nPositionActive( 0 ), 
  m_eOrderSidePending( OrderSide::Unknown ), m_eOrderSideActive( OrderSide::Unknown ),
  m_dblAveragePricePerShare( 0 ), m_dblConstructedValue( 0 ), m_dblMarketValue( 0 ),
  m_dblUnRealizedPL( 0 ), m_dblRealizedPL( 0 ),
  m_dblCommissionPaid( 0 )
{
}

CPosition::CPosition( pInstrument_ref pInstrument, pProvider_ref pExecutionProvider, pProvider_ref pDataProvider, const std::string& sNotes ) 
: m_pExecutionProvider( pExecutionProvider ), m_pDataProvider( pDataProvider ), 
  m_pInstrument( pInstrument ), 
  m_sNotes( sNotes ),
  m_nPositionPending( 0 ), m_nPositionActive( 0 ), 
  m_eOrderSidePending( OrderSide::Unknown ), m_eOrderSideActive( OrderSide::Unknown ),
  m_dblAveragePricePerShare( 0 ), m_dblConstructedValue( 0 ), m_dblMarketValue( 0 ),
  m_dblUnRealizedPL( 0 ), m_dblRealizedPL( 0 ),
  m_dblCommissionPaid( 0 )
{
}

CPosition::~CPosition(void) {
}

COrder::pOrder_t CPosition::PlaceOrder( // market
  OrderType::enumOrderType eOrderType,
  OrderSide::enumOrderSide eOrderSide,
  unsigned long nOrderQuantity
) {
  assert( OrderSide::Unknown != eOrderSide );
  assert( OrderType::Market == eOrderType );
  if ( OrderSide::Unknown != m_eOrderSidePending ) { // ensure new order matches existing orders
    if ( m_eOrderSidePending != eOrderSide ) {
      throw std::runtime_error( "CPosition::PlaceOrder1, new order does not match pending order type" );
    }
  }
  pOrder_t pOrder;
  pOrder = pOrder_t( new COrder( m_pInstrument, eOrderType, eOrderSide, nOrderQuantity ) );
  ProcessOrder( pOrder );
  return pOrder;
}

COrder::pOrder_t CPosition::PlaceOrder( // limit or stop
  OrderType::enumOrderType eOrderType,
  OrderSide::enumOrderSide eOrderSide,
  unsigned long nOrderQuantity,
  double dblPrice1
) {

  assert( OrderSide::Unknown != eOrderSide );
  assert( ( OrderType::Limit == eOrderType) || ( OrderType::Stop == eOrderType ) || ( OrderType::Trail == eOrderType ) );
  if ( OrderSide::Unknown != m_eOrderSidePending ) { // ensure new order matches existing orders
    if ( m_eOrderSidePending != eOrderSide ) {
      throw std::runtime_error( "CPosition::PlaceOrder2, new order does not match pending order type" );
    }
  }

  pOrder_t pOrder;
  pOrder = pOrder_t( new COrder( m_pInstrument, eOrderType, eOrderSide, nOrderQuantity, dblPrice1 ) );
  ProcessOrder( pOrder );
  return pOrder;
}

COrder::pOrder_t CPosition::PlaceOrder( // limit and stop
  OrderType::enumOrderType eOrderType,
  OrderSide::enumOrderSide eOrderSide,
  unsigned long nOrderQuantity,
  double dblPrice1,  
  double dblPrice2
) {

  assert( OrderSide::Unknown != eOrderSide );
  assert( ( OrderType::StopLimit == eOrderType) || ( OrderType::TrailLimit == eOrderType ) );
  if ( OrderSide::Unknown != m_eOrderSidePending ) { // ensure new order matches existing orders
    if ( m_eOrderSidePending != eOrderSide ) {
      throw std::runtime_error( "CPosition::PlaceOrder3, new order does not match pending order type" );
    }
  }

  pOrder_t pOrder;
  pOrder = pOrder_t( new COrder( m_pInstrument, eOrderType, eOrderSide, nOrderQuantity, dblPrice1, dblPrice2 ) );
  ProcessOrder( pOrder );
  return pOrder;
}

void CPosition::ProcessOrder( pOrder_t pOrder ) {
  m_eOrderSidePending = pOrder->GetOrderSide();
  m_nPositionPending += pOrder->GetQuantity();
  m_OpenOrders.push_back( pOrder );
  m_pExecutionProvider->PlaceOrder( pOrder );
}

void CPosition::CancelOrders( void ) {
  // may have a problem getting out of sync with broker if orders are cancelled by broker
  for ( std::vector<pOrder_t>::iterator iter = m_OpenOrders.begin(); iter != m_OpenOrders.end(); ++iter ) {
    COrderManager::Instance().CancelOrder( iter->get()->GetOrderId() );
    m_ClosedOrders.push_back( *iter );
  }
  m_OpenOrders.clear();
}

void CPosition::ClosePosition( void ) {
  // should outstanding orders be auto cancelled?
  // position is closed with a market order, can try to do limit in the futre, but need active market data
  switch ( m_eOrderSideActive ) {
    case OrderSide::Buy:
      PlaceOrder( OrderType::Market, OrderSide::Sell, m_nPositionActive );
      break;
    case OrderSide::Sell:
      PlaceOrder( OrderType::Market, OrderSide::Buy, m_nPositionActive );
      break;
    case OrderSide::Unknown:
      break;
  }
}

// before entry to this method, sanity check:  side on execution is same as side on order
void CPosition::HandleExecution( std::pair<const COrder&, const CExecution&>& status ) {

  // should be able to calculate profit/loss & position cost as exections are encountered
  // should be able to calculate position cost basis as position is updated (with and without commissions)
  // will need market feed in order to calculate profit/loss

  const COrder& order = status.first;
  const CExecution& exec = status.second;
  COrder::orderid_t orderId = order.GetOrderId();

  // update position, regardless of whether we see order open or closed
  switch ( m_eOrderSidePending ) {
    case OrderSide::Buy:
      switch ( exec.GetOrderSide() ) {
        case OrderSide::Buy:  // increase long
          m_nPositionActive += exec.GetSize();
          m_dblConstructedValue += exec.GetSize() * exec.GetPrice();
          break;
        case OrderSide::Sell:  // decrease long
          assert( m_nPositionActive >= exec.GetSize() );
          m_nPositionActive -= exec.GetSize();
          m_dblConstructedValue -= exec.GetSize() * exec.GetPrice();
          if ( 0 == m_nPositionActive ) m_dblConstructedValue = 0;
          break;
      }
      break;
    case OrderSide::Sell:
      switch ( exec.GetOrderSide() ) {
        case OrderSide::Buy:  // decrease short
          assert( m_nPositionActive >= exec.GetSize() );
          m_nPositionActive -= exec.GetSize();
          m_dblConstructedValue -= exec.GetSize() * exec.GetPrice();
          if ( 0 == m_nPositionActive ) m_dblConstructedValue = 0;
          break;
        case OrderSide::Sell:  // increase short
          m_nPositionActive += exec.GetSize();
          m_dblConstructedValue += exec.GetSize() * exec.GetPrice();
          break;
      }
      break;
    case OrderSide::Unknown:
      throw std::runtime_error( "CPosition::HandleExecution isn't prepped for execution" );
      break;
  }

  // check that we think that the order is still active
  bool bOrderFound = false;
  for ( std::vector<pOrder_t>::iterator iter = m_OpenOrders.begin(); iter != m_OpenOrders.end(); ++iter ) {
    if ( orderId == iter->get()->GetOrderId() ) {
      // update position based upon current position and what is executing
      //   decrease position when execution is opposite position
      //   increase position when execution is same as position
      m_nPositionPending -= exec.GetSize();
      if ( 0 == m_nPositionPending ) m_eOrderSidePending = OrderSide::Unknown;

      if ( 0 == order.GetQuanRemaining() ) {  // move from open to closed on order filled
        m_ClosedOrders.push_back( *iter );
        m_OpenOrders.erase( iter );
      }
      
      bOrderFound = true;
      break;
    }
  }
  if ( !bOrderFound ) {
    // need to handle the case where order was cancelled, but not in time to prevent execution
    throw std::runtime_error( "CPosition::HandleExecution doesn't have an Open Order" );
  }
  
}

// process execution to convert Pending to Active
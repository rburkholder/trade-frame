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

CPosition::CPosition( pProvider_ref pProvider, pInstrument_ref pInstrument ) 
: m_pProvider( pProvider ), m_pInstrument( pInstrument ), 
  m_nPositionPending( 0 ), m_nPositionActive( 0 ), 
  m_eOrderSidePending( OrderSide::Unknown ), m_eOrderSideActive( OrderSide::Unknown ),
  m_dblPrice( 0 ), m_dblAverageCost( 0 ), m_dblCommission( 0 )
{
}

CPosition::CPosition( pProvider_ref pProvider, pInstrument_ref pInstrument, const std::string& sNotes ) 
: m_pProvider( pProvider ), m_pInstrument( pInstrument ), 
  m_sNotes( sNotes ),
  m_nPositionPending( 0 ), m_nPositionActive( 0 ), 
  m_eOrderSidePending( OrderSide::Unknown ), m_eOrderSideActive( OrderSide::Unknown ),
  m_dblPrice( 0 ), m_dblAverageCost( 0 ), m_dblCommission( 0 )
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
  m_pProvider->PlaceOrder( pOrder );
}

void CPosition::CancelOrders( void ) {
}

void CPosition::ClosePosition( void ) {
}

// process execution to convert Pending to Active
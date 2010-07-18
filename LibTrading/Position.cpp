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

CPosition::CPosition( CInstrument::pInstrument_ref pInstrument ) 
: m_pInstrument( pInstrument ), 
  m_nPosition( 0 ), m_dblPrice( 0 ), m_dblAverageCost( 0 ), m_dblCommission( 0 )
{
}

CPosition::CPosition( CInstrument::pInstrument_ref pInstrument, const std::string& sNotes ) 
: m_pInstrument( pInstrument ), 
  m_sNotes( sNotes ),
  m_nPosition( 0 ), m_dblPrice( 0 ), m_dblAverageCost( 0 ), m_dblCommission( 0 )
{
}

CPosition::~CPosition(void) {
}

COrder::pOrder_ref CPosition::PlaceOrder( // market
  OrderType::enumOrderType eOrderType,
  OrderSide::enumOrderSide eOrderSide,
  unsigned long nOrderQuantity
  ) {
}
COrder::pOrder_ref CPosition::PlaceOrder( // limit or stop
  OrderType::enumOrderType eOrderType,
  OrderSide::enumOrderSide eOrderSide,
  unsigned long nOrderQuantity,
  double dblPrice1
  ) {
}
COrder::pOrder_ref CPosition::PlaceOrder( // limit and stop
  OrderType::enumOrderType eOrderType,
  OrderSide::enumOrderSide eOrderSide,
  unsigned long nOrderQuantity,
  double dblPrice1,  
  double dblPrice2
  ) {
}

void CPosition::CancelOrders( void ) {
  COrder::
}

void CPosition::ClosePosition( void ) {
}

/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include "OrdersOutstanding.h"

OrdersOutstanding::OrdersOutstanding( pPosition_t pPosition ) : m_pPosition( pPosition ) {
}

void OrdersOutstanding::AddOrderFilling( pOrder_t pOrder ) {
  m_mapOrdersFilling[ pOrder->GetOrderId() ] = pOrder;
  pOrder->OnOrderFilled.Add( MakeDelegate( this, &OrdersOutstanding::HandleOrderFilled ) );
  if ( 0 == pOrder->GetQuanRemaining() ) {
    HandleOrderFilled( *pOrder.get() );
  }
}

void OrdersOutstanding::HandleOrderFilled( const ou::tf::COrder& order ) {
  idOrder_t id = order.GetOrderId();
  mapOrdersFilling_t::iterator iter = m_mapOrdersFilling.find( id );
  if ( m_mapOrdersFilling.end() == iter ) {
    throw std::runtime_error( "can't find order" );
  }
  iter->second->OnOrderFilled.Remove( MakeDelegate( this, &OrdersOutstanding::HandleOrderFilled ) );
  double dblBasis = iter->second->GetAverageFillPrice();
  m_mapOrdersToMatch.insert( mapOrders_pair_t( dblBasis, structOrderMatching( dblBasis, iter->second ) ) );
  m_mapOrdersFilling.erase( iter );
}

void OrdersOutstanding::CancelAll( void ) {
  for ( mapOrders_iter_t iter = m_mapOrdersToMatch.begin(); m_mapOrdersToMatch.end() != iter; ++iter ) {
    if ( 0 != iter->second.pOrderClosing.use_count() ) {
      m_pPosition->CancelOrder( iter->second.pOrderClosing->GetOrderId() );  // what happens if filled during cancel?
      iter->second.pOrderClosing.reset();
    }
  }
}

void OrdersOutstandingLongs::HandleQuote( const ou::tf::CQuote& quote ) {
  double ask = quote.Ask();
  if ( 0.0 != ask ) {
    for ( mapOrders_iter_t iter = m_mapOrdersToMatch.begin(); m_mapOrdersToMatch.end() != iter; ++iter ) {
      if ( iter->first >= ask ) { // price is outside of profitable range
        if ( 0 == iter->second.pOrderClosing.use_count() ) { // nothing to do
        }
        else { // cancel existing order
          m_pPosition->CancelOrder( iter->second.pOrderClosing->GetOrderId() );  // what happens if filled during cancel?
          iter->second.pOrderClosing.reset();
        }
      }
      else { // price is inside profitable range
        if ( 0 == iter->second.pOrderClosing.use_count() ) { // create a limit order to attempt profit
          // may need to do some rounding when using larger quantities
          // use quantities from opening order, also will need to deal with fractional quantities on partial filled orders
          iter->second.pOrderClosing = m_pPosition->PlaceOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 1, iter->first + 0.10 );
        }
        else { // do nothing
        }
      }
    }
  }
}

void OrdersOutstandingShorts::HandleQuote( const ou::tf::CQuote& quote ) {
  double bid = quote.Bid();
  if ( 0.0 != bid ) {
    for ( mapOrders_iter_t iter = m_mapOrdersToMatch.begin(); m_mapOrdersToMatch.end() != iter; ++iter ) {
      if ( iter->first <= bid ) { // price is outside of profitable range
        if ( 0 == iter->second.pOrderClosing.use_count() ) { // nothing to do
        }
        else { // cancel existing order
          m_pPosition->CancelOrder( iter->second.pOrderClosing->GetOrderId() );  // what happens if filled during cancel?
          iter->second.pOrderClosing.reset();
        }
      }
      else { // price is inside profitable range
        if ( 0 == iter->second.pOrderClosing.use_count() ) { // create a limit order to attempt profit
          // may need to do some rounding when using larger quantities
          // use quantities from opening order, also will need to deal with fractional quantities on partial filled orders
          iter->second.pOrderClosing = m_pPosition->PlaceOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1, iter->first - 0.10 );
        }
        else { // do nothing
        }
      }
    }
  }
}


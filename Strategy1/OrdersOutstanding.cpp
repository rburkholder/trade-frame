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
  m_mapBaseOrdersFilling[ pOrder->GetOrderId() ] = pOrder;
  pOrder->OnOrderFilled.Add( MakeDelegate( this, &OrdersOutstanding::HandleBaseOrderFilled ) );
  if ( 0 == pOrder->GetQuanRemaining() ) {
    HandleBaseOrderFilled( *pOrder.get() );
  }
}

void OrdersOutstanding::HandleBaseOrderFilled( const ou::tf::COrder& order ) {
  idOrder_t id = order.GetOrderId();
  mapOrdersFilling_t::iterator iter = m_mapBaseOrdersFilling.find( id );
  if ( m_mapBaseOrdersFilling.end() == iter ) {
    throw std::runtime_error( "can't find order" );
  }
  iter->second->OnOrderFilled.Remove( MakeDelegate( this, &OrdersOutstanding::HandleBaseOrderFilled ) );
  double dblBasis = iter->second->GetAverageFillPrice();
  m_mapOrdersToMatch.insert( mapOrders_pair_t( dblBasis, structOrderMatching( dblBasis, iter->second ) ) );
  m_mapBaseOrdersFilling.erase( iter );
}

void OrdersOutstanding::CancelAll( void ) {
  for ( mapOrders_iter_t iter = m_mapOrdersToMatch.begin(); m_mapOrdersToMatch.end() != iter; ++iter ) {
    if ( 0 != iter->second.pOrderClosing.use_count() ) {
      m_pPosition->CancelOrder( iter->second.pOrderClosing->GetOrderId() );  // what happens if filled during cancel?
      iter->second.pOrderClosing.reset();
    }
  }
}

void OrdersOutstanding::HandleMatchingOrderCancelled( const ou::tf::COrder& order ) {
  ou::tf::COrder::idOrder_t id = order.GetOrderId();
  for ( mapOrders_iter_t iter = m_mapOrdersToMatch.begin(); m_mapOrdersToMatch.end() != iter; ++iter ) {
    // need to handle partial fill orders, do market order or manage partial fills based upon position?
    if ( 0 == iter->second.pOrderClosing.get() ) {  // map may have multiple orders, some of which don't have a matching order at the present
//      assert( false );
    }
    else {
      if ( id == iter->second.pOrderClosing->GetOrderId() ) {
        pOrder_t& pOrderClosing( iter->second.pOrderClosing );
        pOrderClosing->OnOrderFilled.Remove( MakeDelegate( this, &OrdersOutstanding::HandleMatchingOrderFilled ) );
        pOrderClosing->OnOrderCancelled.Remove( MakeDelegate( this, &OrdersOutstanding::HandleMatchingOrderCancelled ) );
        //m_mapOrdersToMatch.erase( iter );
        iter->second.pOrderClosing.reset();
        break;
      }
    }
  }
}

void OrdersOutstanding::HandleMatchingOrderFilled( const ou::tf::COrder& order ) {
  // use this to check when order filled before cancellation
  // is there an event for order cancellation?
  ou::tf::COrder::idOrder_t id = order.GetOrderId();
  for ( mapOrders_iter_t iter = m_mapOrdersToMatch.begin(); m_mapOrdersToMatch.end() != iter; ++iter ) {
    if ( 0 == iter->second.pOrderClosing.get() ) {  // we've already cancelled
    }
    else {
      if ( id == iter->second.pOrderClosing->GetOrderId() ) {
        pOrder_t& pOrderClosing( iter->second.pOrderClosing );
        pOrderClosing->OnOrderFilled.Remove( MakeDelegate( this, &OrdersOutstanding::HandleMatchingOrderFilled ) );
        pOrderClosing->OnOrderCancelled.Remove( MakeDelegate( this, &OrdersOutstanding::HandleMatchingOrderCancelled ) );
        m_mapOrdersToMatch.erase( iter );
        break;
      }
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
        else { // cancel existing order, but only do once, so look at status
          m_pPosition->CancelOrder( iter->second.pOrderClosing->GetOrderId() );  // what happens if filled during cancel?
          //iter->second.pOrderClosing.reset();  // this will be a problem if order is filled during cancellation
        }
      }
      else { // price is inside profitable range
        if ( 0 == iter->second.pOrderClosing.use_count() ) { // create a limit order to attempt profit
          // may need to do some rounding when using larger quantities
          // use quantities from opening order, also will need to deal with fractional quantities on partial filled orders
          pOrder_t& pOrderClosing( iter->second.pOrderClosing );
          pOrderClosing = m_pPosition->PlaceOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 1, iter->first + 0.20 );
          ou::tf::COrder::idOrder_t id = pOrderClosing->GetOrderId();
          pOrderClosing->OnOrderFilled.Add( MakeDelegate( this, &OrdersOutstanding::HandleMatchingOrderFilled ) );
          pOrderClosing->OnOrderCancelled.Add( MakeDelegate( this, &OrdersOutstanding::HandleMatchingOrderCancelled ) );
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
        else { // cancel existing order, but only do once, so look at status
          m_pPosition->CancelOrder( iter->second.pOrderClosing->GetOrderId() );  // what happens if filled during cancel?
          //iter->second.pOrderClosing.reset();  // this will be a problem if order is filled during cancellation
        }
      }
      else { // price is inside profitable range
        if ( 0 == iter->second.pOrderClosing.use_count() ) { // create a limit order to attempt profit
          // may need to do some rounding when using larger quantities
          // use quantities from opening order, also will need to deal with fractional quantities on partial filled orders
          pOrder_t& pOrderClosing( iter->second.pOrderClosing );
          pOrderClosing = m_pPosition->PlaceOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1, iter->first - 0.20 );
          ou::tf::COrder::idOrder_t id = pOrderClosing->GetOrderId();
          pOrderClosing->OnOrderFilled.Add( MakeDelegate( this, &OrdersOutstanding::HandleMatchingOrderFilled ) );
          pOrderClosing->OnOrderCancelled.Add( MakeDelegate( this, &OrdersOutstanding::HandleMatchingOrderCancelled ) );
        }
        else { // do nothing
        }
      }
    }
  }
}


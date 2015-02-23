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

#include "stdafx.h"

#include <math.h>

#include <boost/lexical_cast.hpp>

#include "OrdersOutstanding.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

OrdersOutstanding::OrdersOutstanding( pPosition_t pPosition ) : 
  m_pPosition( pPosition ), m_cntRoundTrips( 0 ),
  m_durRoundTripTime( 0, 0, 0 ), 
  m_durForceRoundTripClose( 0, 60, 0 ),  // try something from 5 minutes to 10 minutes
  m_durOrderOpenTimeOut( 0, 0, 30 ), 
  m_dblGlobalStop( 0.0 ),
  m_bCancelAndCloseInProgress( false )
{
}

void OrdersOutstanding::AddOrderFilling( structRoundTrip* pTrip ) {
  assert( !m_bCancelAndCloseInProgress );
  ou::tf::Order& order( *pTrip->pOrderEntry.get() );
  ou::tf::Order::idOrder_t idOrder = order.GetOrderId();
  assert( m_mapEntryOrdersFilling.end() == m_mapEntryOrdersFilling.find( idOrder ) );
  m_mapEntryOrdersFilling[ idOrder ] = pRoundTrip_t( pTrip );
  order.OnOrderFilled.Add( MakeDelegate( this, &OrdersOutstanding::HandleBaseOrderFilled ) );  // yes, this belongs here as it will be unconditionally removed later
  order.OnOrderCancelled.Add( MakeDelegate( this, &OrdersOutstanding::HandleBaseOrderCancelled ) );
  if ( 0 == order.GetQuanRemaining() ) {
    HandleBaseOrderFilled( order );
  }
}

void OrdersOutstanding::HandleBaseOrderCancelled( const ou::tf::Order& order ) {
  idOrder_t id = order.GetOrderId();
  mapOrdersFilling_t::iterator iter = m_mapEntryOrdersFilling.find( id );
  if ( m_mapEntryOrdersFilling.end() == iter ) {
    throw std::runtime_error( "can't find order" );
  }
  const_cast<ou::tf::Order&>( order ).OnOrderFilled.Remove( MakeDelegate( this, &OrdersOutstanding::HandleBaseOrderFilled ) );
  const_cast<ou::tf::Order&>( order ).OnOrderCancelled.Remove( MakeDelegate( this, &OrdersOutstanding::HandleBaseOrderCancelled ) );

  iter->second->eState = EStateCancelled;

  m_mapEntryOrdersFilling.erase( iter ); 
}

void OrdersOutstanding::HandleBaseOrderFilled( const ou::tf::Order& order ) {
  idOrder_t id = order.GetOrderId();
  mapOrdersFilling_t::iterator iter = m_mapEntryOrdersFilling.find( id );
  if ( m_mapEntryOrdersFilling.end() == iter ) {
    throw std::runtime_error( "can't find order" );
  }
  const_cast<ou::tf::Order&>( order ).OnOrderFilled.Remove( MakeDelegate( this, &OrdersOutstanding::HandleBaseOrderFilled ) );
  const_cast<ou::tf::Order&>( order ).OnOrderCancelled.Remove( MakeDelegate( this, &OrdersOutstanding::HandleBaseOrderCancelled ) );

  iter->second->eState = EStateOpen;

  double dblBasis = order.GetAverageFillPrice();
  iter->second->dblBasis = dblBasis;
  m_mapOrdersToMatch.insert( mapOrders_pair_t( dblBasis, iter->second ) );

  m_mapEntryOrdersFilling.erase( iter ); 
}

void OrdersOutstanding::CheckBaseOrder( const ou::tf::Quote& quote ) { // cancel after minimal pending time
  for ( mapOrdersFilling_iter_t iter = m_mapEntryOrdersFilling.begin(); m_mapEntryOrdersFilling.end() != iter; ++iter ) {
    if ( EStateOpenWaitingFill == iter->second->eState ) {
      if ( iter->second->pOrderEntry->GetDateTimeOrderSubmitted() + m_durOrderOpenTimeOut < quote.DateTime() ) {
        // close out order after time out
        m_pPosition->CancelOrder( iter->second->pOrderEntry->GetOrderId() );
        iter->second->eState = EStateOpenCancelling;
      }
    }
  }
}

void OrdersOutstanding::CancelAllButNEntryOrders( unsigned int n ) {
  size_t m = m_mapEntryOrdersFilling.size();
  mapOrdersFilling_iter_t iter = m_mapEntryOrdersFilling.begin();
  while ( m > n ) {
    if ( EStateOpenWaitingFill == iter->second->eState ) {
      iter->second->eState = EStateOpenCancelling;
      m_pPosition->CancelOrder( iter->first );
    }
    ++iter;
    --m;
  }
}

void OrdersOutstanding::CancelAllMatchingOrders( void ) {
  for ( mapOrders_iter_t iter = m_mapOrdersToMatch.begin(); m_mapOrdersToMatch.end() != iter; ++iter ) {
    if ( 0 != iter->second->pOrderExit.use_count() ) {
      m_pPosition->CancelOrder( iter->second->pOrderExit->GetOrderId() );  // what happens if filled during cancel?
//      iter->second.pOrderClosing.reset();
    }
  }
  if ( 0 == m_cntRoundTrips ) {
  }
  else {
    boost::uint64_t ave = m_durRoundTripTime.total_milliseconds() / m_cntRoundTrips;
  }
}

void OrdersOutstanding::CancelAndCloseAllOrders( void ) {
  m_bCancelAndCloseInProgress = true;
  m_stateCancelAndClose = CACStarted;
  for ( mapOrdersFilling_iter_t iter = m_mapEntryOrdersFilling.begin(); m_mapEntryOrdersFilling.end() != iter; ++iter ) {
    if ( EStateOpenWaitingFill == iter->second->eState ) {
      m_pPosition->CancelOrder( iter->second->pOrderEntry->GetOrderId() );
      iter->second->eState = EStateOpenCancelling;
    }
  }
  for ( mapOrders_iter_t iter = m_mapOrdersToMatch.begin(); m_mapOrdersToMatch.end() != iter; ++iter ) {
    if ( 0 != iter->second->pOrderExit.use_count() ) {
      m_pPosition->CancelOrder( iter->second->pOrderExit->GetOrderId() );
    }
  }
  m_stateCancelAndClose = CACWaitingForEntryCancels;
}

bool OrdersOutstanding::CancelAndCloseInProgress( void ) {
  if ( m_bCancelAndCloseInProgress ) {
    switch ( m_stateCancelAndClose ) {
    case CACWaitingForEntryCancels:
      if ( 0 == m_mapEntryOrdersFilling.size() ) {
        m_stateCancelAndClose = CACWaitingForExitCancels;
      }
      break;
    case CACWaitingForExitCancels: {
        bool bStillWaiting = false;
        for ( mapOrders_iter_t iter = m_mapOrdersToMatch.begin(); m_mapOrdersToMatch.end() != iter; ++iter ) {
          if ( 0 != iter->second->pOrderExit.use_count() ) {
            bStillWaiting = true;
          }
        }
        if ( !bStillWaiting ) {
          //ou::tf::Order::pOrder_t pOrder;
          for ( mapOrders_iter_t iter = m_mapOrdersToMatch.begin(); m_mapOrdersToMatch.end() != iter; ++iter ) {
            std::string sId( boost::lexical_cast<std::string>( iter->second->pOrderEntry->GetOrderId() ) );
            switch ( iter->second->pOrderEntry->GetOrderSide() ) {
            case ou::tf::OrderSide::Buy:
              PlaceOrder( iter->second->pOrderExit, "Cancel&Close " + sId, ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
              iter->second->eState = EStateClosing;
              break;
            case ou::tf::OrderSide::Sell:
              PlaceOrder( iter->second->pOrderExit, "Cancel&Close " + sId, ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
              iter->second->eState = EStateClosing;
              break;
            }
          }
          m_stateCancelAndClose = CACWaitingForMatchingCloses;
        }
      }
      break;
    case CACWaitingForMatchingCloses:
      if ( 0 == m_mapOrdersToMatch.size() ) {
        m_stateCancelAndClose = CACDone;
        m_bCancelAndCloseInProgress = false;
      }
      break;
    case CACStarted: // shouldn't reach this
      break;
    case CACDone:
      break;
    }
  }
  return m_bCancelAndCloseInProgress;
}

void OrdersOutstanding::HandleMatchingOrderCancelled( const ou::tf::Order& order ) {
  ou::tf::Order::idOrder_t id = order.GetOrderId();
  for ( mapOrders_iter_t iter = m_mapOrdersToMatch.begin(); m_mapOrdersToMatch.end() != iter; ++iter ) {
    // need to handle partial fill orders, do market order or manage partial fills based upon position?
    if ( 0 == iter->second->pOrderExit.get() ) {  // map may have multiple orders, some of which don't have a matching order at the present
//      assert( false );
    }
    else {
      ou::tf::Order& order( *iter->second->pOrderExit.get() );
      if ( id == order.GetOrderId() ) {
        order.OnOrderFilled.Remove( MakeDelegate( this, &OrdersOutstanding::HandleMatchingOrderFilled ) );
        order.OnOrderCancelled.Remove( MakeDelegate( this, &OrdersOutstanding::HandleMatchingOrderCancelled ) );
        //m_mapOrdersToMatch.erase( iter );
        iter->second->pOrderExit.reset();
        break;
      }
    }
  }
}

void OrdersOutstanding::HandleMatchingOrderFilled( const ou::tf::Order& order ) {
  // use this to check when order filled before cancellation
  ou::tf::Order::idOrder_t id = order.GetOrderId();
  for ( mapOrders_iter_t iter = m_mapOrdersToMatch.begin(); m_mapOrdersToMatch.end() != iter; ++iter ) {
    if ( 0 == iter->second->pOrderExit.get() ) {  // we've already cancelled
    }
    else {
      ou::tf::Order& order( *iter->second->pOrderExit.get() );
      if ( id == order.GetOrderId() ) {
        m_durRoundTripTime += order.GetDateTimeOrderFilled() - order.GetDateTimeOrderFilled();
        ++m_cntRoundTrips;
        order.OnOrderFilled.Remove( MakeDelegate( this, &OrdersOutstanding::HandleMatchingOrderFilled ) );
        order.OnOrderCancelled.Remove( MakeDelegate( this, &OrdersOutstanding::HandleMatchingOrderCancelled ) );
        m_vCompletedRoundTrips.push_back( iter->second );
        m_mapOrdersToMatch.erase( iter );
        break;
      }
    }
  }
}

void OrdersOutstandingLongs::HandleQuote( const ou::tf::Quote& quote ) {
  if ( !CancelAndCloseInProgress() ) {
    CheckBaseOrder( quote );
    double ask = quote.Ask();
    if ( 0.0 != ask ) {
      for ( mapOrders_iter_t iter = m_mapOrdersToMatch.begin(); m_mapOrdersToMatch.end() != iter; ++iter ) {
        std::string sId( boost::lexical_cast<std::string>( iter->second->pOrderEntry->GetOrderId() ) );
        if ( iter->first >= ask ) { // price is outside of profitable range
          if ( 0 == iter->second->pOrderExit.use_count() ) { 
            if ( iter->second->pOrderEntry->GetDateTimeOrderFilled() + m_durForceRoundTripClose < quote.DateTime() ) {
              // close out round trip
              PlaceOrder( iter->second->pOrderExit, "LongClose " + sId, ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
              iter->second->eState = EStateClosing;
            }
            else { // check stop
              if ( 0.0 != iter->second->dblStop ) {
                if ( iter->second->dblStop > ask ) {
                  PlaceOrder( iter->second->pOrderExit, "LongStop " + sId, ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
                  iter->second->eState = EStateClosing;
                }
              }
            }
          }
          else { // cancel existing order, but only do once, so look at status
            if ( EStateClosing != iter->second->eState ) {
                m_pPosition->CancelOrder( iter->second->pOrderExit->GetOrderId() );  // what happens if filled during cancel?
                //iter->second.pOrderClosing.reset();  // this will be a problem if order is filled during cancellation
            }
          }
        }
        else { // price is inside profitable range
          if ( 0 == iter->second->pOrderExit.use_count() ) { // create a limit order to attempt profit
            // may need to do some rounding when using larger quantities
            // use quantities from opening order, also will need to deal with fractional quantities on partial filled orders
            PlaceOrder( iter->second->pOrderExit, "LongProfit " + sId, ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 1, iter->second->dblTarget );
          }
          else { // do nothing
          }
        }
      }
    }
  }
}

void OrdersOutstandingShorts::HandleQuote( const ou::tf::Quote& quote ) {
  if ( !CancelAndCloseInProgress() ) {
    CheckBaseOrder( quote );
    double bid = quote.Bid();
    if ( 0.0 != bid ) {
      for ( mapOrders_iter_t iter = m_mapOrdersToMatch.begin(); m_mapOrdersToMatch.end() != iter; ++iter ) {
        std::string sId( boost::lexical_cast<std::string>( iter->second->pOrderEntry->GetOrderId() ) );
        if ( iter->first <= bid ) { // price is outside of profitable range
          if ( 0 == iter->second->pOrderExit.use_count() ) {
            if ( iter->second->pOrderEntry->GetDateTimeOrderFilled() + m_durForceRoundTripClose < quote.DateTime() ) {
              // close out round trip
              PlaceOrder( iter->second->pOrderExit, "ShortClose " + sId, ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
              iter->second->eState = EStateClosing;
            }
            else { // check stop
              if ( 0.0 != iter->second->dblStop ) {
                if ( iter->second->dblStop < bid ) {
                  PlaceOrder( iter->second->pOrderExit, "ShortStop " + sId, ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
                  iter->second->eState = EStateClosing;
                }
              }
            }
          }
          else { // cancel existing order, but only do once, so look at status
            if ( EStateClosing != iter->second->eState ) {
              m_pPosition->CancelOrder( iter->second->pOrderExit->GetOrderId() );  // what happens if filled during cancel?
              //iter->second.pOrderClosing.reset();  // this will be a problem if order is filled during cancellation
            }
          }
        }
        else { // price is inside profitable range
          if ( 0 == iter->second->pOrderExit.use_count() ) { // create a limit order to attempt profit
            // may need to do some rounding when using larger quantities
            // use quantities from opening order, also will need to deal with fractional quantities on partial filled orders
            PlaceOrder( iter->second->pOrderExit, "ShortProfit " + sId, ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1, iter->second->dblTarget );
          }
          else { // do nothing
          }
        }
      }
    }
  }
}

void OrdersOutstanding::PostMortemReport( void ) {
  double dif( 0.0 );
  long td( 0 );
  std::vector<long> vtd;
  for ( vCompletedRoundTrip_citer_t iter = m_vCompletedRoundTrips.begin(); m_vCompletedRoundTrips.end() != iter; ++iter ) {
    dif += iter->get()->pOrderExit->GetAverageFillPrice() - iter->get()->pOrderEntry->GetAverageFillPrice();
    time_duration t( iter->get()->pOrderExit->GetDateTimeOrderFilled() - iter->get()->pOrderEntry->GetDateTimeOrderFilled() );
    long lt = t.total_seconds();
    td += lt;
    vtd.push_back( lt );
  }
  if ( 0 != m_vCompletedRoundTrips.size() ) {
    size_t mn( td / m_vCompletedRoundTrips.size() );
    long long sum( 0 );
    for ( std::vector<long>::iterator iter = vtd.begin(); vtd.end() != iter; ++iter ) {
      size_t num( mn - *iter );
      sum += ( num * num );
    }
    double sq = sqrt( (double) sum );

    std::stringstream ss;
    ss << "round trips=" << m_vCompletedRoundTrips.size() 
      << ", avg dur=" << mn << ", stddev=" << sq / ( m_vCompletedRoundTrips.size() - 1 )
      << ", avg dif=" << dif / m_vCompletedRoundTrips.size() 
      << std::endl;
  }
}

void OrdersOutstanding::PlaceOrder( pOrder_t& pOrder, const std::string& sDescription, ou::tf::OrderType::enumOrderType type, ou::tf::OrderSide::enumOrderSide side, boost::uint32_t nOrderQuantity ) {
  pOrder = m_pPosition->ConstructOrder(type, side, nOrderQuantity );
  pOrder->SetDescription( sDescription );
  PlaceOrder( pOrder );
}

void OrdersOutstanding::PlaceOrder( pOrder_t& pOrder, const std::string& sDescription, ou::tf::OrderType::enumOrderType type, ou::tf::OrderSide::enumOrderSide side, boost::uint32_t nOrderQuantity, double dblPrice1  ) {
  pOrder = m_pPosition->ConstructOrder(type, side, nOrderQuantity, dblPrice1 );
  pOrder->SetDescription( sDescription );
  PlaceOrder( pOrder );
}

void OrdersOutstanding::PlaceOrder( pOrder_t& pOrder, const std::string& sDescription, ou::tf::OrderType::enumOrderType type, ou::tf::OrderSide::enumOrderSide side, boost::uint32_t nOrderQuantity, double dblPrice1, double dblPrice2 ) {
  pOrder = m_pPosition->ConstructOrder(type, side, nOrderQuantity, dblPrice1, dblPrice2 );
  pOrder->SetDescription( sDescription );
  PlaceOrder( pOrder );
}

void OrdersOutstanding::PlaceOrder( pOrder_t& pOrder ) {
  ou::tf::Order& order( *pOrder.get() );
  order.OnOrderFilled.Add( MakeDelegate( this, &OrdersOutstanding::HandleMatchingOrderFilled ) );
  order.OnOrderCancelled.Add( MakeDelegate( this, &OrdersOutstanding::HandleMatchingOrderCancelled ) );
  m_pPosition->PlaceOrder( pOrder );
}

} // namespace tf
} // namespace ou


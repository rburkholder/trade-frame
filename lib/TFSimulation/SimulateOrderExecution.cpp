/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <OUCommon/TimeSource.h>

#include "SimulateOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

int SimulateOrderExecution::m_nExecId( 1000 );

SimulateOrderExecution::SimulateOrderExecution()
: m_dtQueueDelay( milliseconds( 500 ) ), m_dblCommission( 1.00 )//, m_ea( EAQuotes )
{
}

SimulateOrderExecution::~SimulateOrderExecution() {
}

void SimulateOrderExecution::NewTrade( const Trade& trade ) {
  ProcessLimitOrders( trade );
}

void SimulateOrderExecution::NewQuote( const Quote& quote ) {
  ProcessOrderQueues( quote );
  m_lastQuote = quote;
}

void SimulateOrderExecution::SubmitOrder( pOrder_t pOrder ) {
  m_lOrderDelay.push_back( pOrder );
}

void SimulateOrderExecution::CancelOrder( Order::idOrder_t nOrderId ) {
  structCancelOrder co( ou::TimeSource::LocalCommonInstance().Internal(), nOrderId );
  m_lCancelDelay.push_back( co );
}

void SimulateOrderExecution::CalculateCommission( Order* pOrder, Trade::tradesize_t quan ) {
  // Order or CInstrument should have commission calculation?
  if ( 0 != quan ) {
    if ( NULL != OnCommission ) {
      double dblCommission( 0 );
      switch ( pOrder->GetInstrument()->GetInstrumentType() ) {
        case InstrumentType::ETF:
        case InstrumentType::Stock:
          dblCommission = 0.005 * (double) quan;
          if ( 1.00 > dblCommission ) dblCommission = 1.00;
          break;
        case InstrumentType::Option:
          dblCommission = 0.95 * (double) quan;
          break;
        case InstrumentType::Future:
          dblCommission = 2.50 * (double) quan;  // GC futures have this commission
          break;
        case InstrumentType::Currency:
          break;
      }
      OnCommission( pOrder->GetOrderId(), dblCommission );
    }
  }
}

void SimulateOrderExecution::ProcessOrderQueues( const Quote &quote ) {

  if ( !quote.IsValid() ) {
    return;
  }

  ProcessCancelQueue( quote );

  ProcessDelayQueue( quote );

  ProcessStopOrders( quote ); // places orders into market orders queue

  bool bProcessed;
  bProcessed = ProcessMarketOrders( quote );
  if ( !bProcessed ) {
    bProcessed = ProcessLimitOrders( quote );
  }

}

void SimulateOrderExecution::ProcessStopOrders( const Quote& quote ) {
  // not yet implemented
}

bool SimulateOrderExecution::ProcessMarketOrders( const Quote& quote ) {

  pOrder_t pOrderFrontOfQueue;  // change this so we reference the order directly, makes things a bit faster
  bool bProcessed = false;

  // process market orders
  if ( !m_lOrderMarket.empty() ) {

    pOrderFrontOfQueue = m_lOrderMarket.front();
    bProcessed = true;

    boost::uint32_t nOrderQuanRemaining = pOrderFrontOfQueue->GetQuanRemaining();
    assert( 0 != nOrderQuanRemaining );

    // figure out price of execution
    Trade::tradesize_t quanAvail;
    double dblPrice;
    OrderSide::EOrderSide orderSide = pOrderFrontOfQueue->GetOrderSide();
    switch ( orderSide ) {
      case OrderSide::Buy:
        quanAvail = std::min<Trade::tradesize_t>( nOrderQuanRemaining, quote.AskSize() );
        dblPrice = quote.Ask();
        break;
      case OrderSide::Sell:
        quanAvail = std::min<Trade::tradesize_t>( nOrderQuanRemaining, quote.BidSize() );
        dblPrice = quote.Bid();
        break;
      default:
        throw std::runtime_error( "SimulateOrderExecution::ProcessMarketOrders unknown order side" );
        break;
    }

    // execute order
    if ( 0 != OnOrderFill ) {
      std::string id;
      int nId( m_nExecId );  // before it gets incremented in next function
      GetExecId( &id );
      // using id in first parameter may or may not work
      Execution exec( nId, pOrderFrontOfQueue->GetOrderId(), dblPrice, quanAvail, orderSide, "SIMMkt", id );
      OnOrderFill( pOrderFrontOfQueue->GetOrderId(), exec );
    }
    else {
      int i = 1;  // we have a problem as nOrderQuanRemaining won't be updated for the next pass through on partial orders
      throw std::runtime_error( "no onorderfill to keep housekeeping in place" );
    }

    nOrderQuanRemaining -= quanAvail;

    // when order done, commission and toss away
    // what happens on cancelled orders and partial fills?
    if ( 0 == nOrderQuanRemaining ) {
      CalculateCommission( pOrderFrontOfQueue.get(), pOrderFrontOfQueue->GetQuanFilled() );
      m_lOrderMarket.pop_front();
    }
  }
  return bProcessed;
}

bool SimulateOrderExecution::ProcessLimitOrders( const Quote& quote ) {

  pOrder_t pOrderFrontOfQueue; // change this so we reference the order directly, makes things a bit faster
  bool bProcessed = false;
  boost::uint32_t nOrderQuanRemaining = 0;

  // todo: what about self's own crossing orders, could fill with out qoute

  if ( !m_mapAsks.empty() ) {
    if ( quote.Bid() >= m_mapAsks.begin()->first ) {
      bProcessed = true;
      pOrderFrontOfQueue = m_mapAsks.begin()->second;
      nOrderQuanRemaining = pOrderFrontOfQueue->GetQuanRemaining();
      assert( 0 != nOrderQuanRemaining );
      Trade::tradesize_t quanAvail = std::min<Trade::tradesize_t>( nOrderQuanRemaining, quote.BidSize() );
      if ( 0 != OnOrderFill ) {
        std::string id;
        GetExecId( &id );
        Execution exec( quote.Bid(), quanAvail, OrderSide::Sell, "SIMLmtSell", id );
        OnOrderFill( pOrderFrontOfQueue->GetOrderId(), exec );
        nOrderQuanRemaining -= quanAvail;
        if ( 0 == nOrderQuanRemaining ) {
          CalculateCommission( pOrderFrontOfQueue.get(), pOrderFrontOfQueue->GetQuanFilled() );
          m_mapAsks.erase( m_mapAsks.begin() );
        }
      }
    }
  }
  if ( !m_mapBids.empty() && !bProcessed) {
    if ( quote.Ask() <= m_mapBids.rbegin()->first ) {
      bProcessed = true;
      pOrderFrontOfQueue = m_mapBids.rbegin()->second;
      nOrderQuanRemaining = pOrderFrontOfQueue->GetQuanRemaining();
      assert( 0 != nOrderQuanRemaining );
      Trade::tradesize_t quanAvail = std::min<Trade::tradesize_t>( nOrderQuanRemaining, quote.AskSize() );
      if ( 0 != OnOrderFill ) {
        std::string id;
        GetExecId( &id );
        Execution exec( quote.Ask(), quanAvail, OrderSide::Buy, "SIMLmtBuy", id );
        OnOrderFill( pOrderFrontOfQueue->GetOrderId(), exec );
        nOrderQuanRemaining -= quanAvail;
        if ( 0 == nOrderQuanRemaining ) {
          CalculateCommission( pOrderFrontOfQueue.get(), pOrderFrontOfQueue->GetQuanFilled() );
          m_mapBids.erase( --m_mapBids.rbegin().base() );
        }
      }
    }
  }

  return bProcessed;
}

bool SimulateOrderExecution::ProcessLimitOrders( const Trade& trade ) {
  //bool bAllow( true );
  double bid( trade.Price() );
  double ask( trade.Price() );
  if ( !m_mapAsks.empty() ) {
    if ( m_lastQuote.Ask() <= m_mapAsks.begin()->first ) {
      ask = m_lastQuote.Ask();
    }
  }
  if ( !m_mapBids.empty() ) {
    if ( m_lastQuote.Bid() >= m_mapBids.rbegin()->first ) {
      bid = m_lastQuote.Bid();
    }
  }
  Quote quote( trade.DateTime(), bid, trade.Volume(), ask, trade.Volume() );
  return ProcessLimitOrders( quote );
}

void SimulateOrderExecution::ProcessDelayQueue( const Quote& quote ) {

  pOrder_t pOrderFrontOfQueue;  // change this so we reference the order directly, makes things a bit faster

  // process the delay list
  while ( !m_lOrderDelay.empty() ) {
    if ( ( m_lOrderDelay.front()->GetDateTimeOrderSubmitted() + m_dtQueueDelay ) >= quote.DateTime() ) {
      break;
    }
    else {
      pOrderFrontOfQueue = m_lOrderDelay.front();
      m_lOrderDelay.pop_front();
      switch ( pOrderFrontOfQueue->GetOrderType() ) {
        case OrderType::Market:
          // place into market order book
          if ( 0 == m_lOrderMarket.size() ) {
            m_lOrderMarket.push_back( pOrderFrontOfQueue );
          }
          else {
            if ( pOrderFrontOfQueue->GetOrderSide() == m_lOrderMarket.front()->GetOrderSide() ) {
              m_lOrderMarket.push_back( pOrderFrontOfQueue );
            }
            else {
              // can't have market orders in two different directions
              if ( 0 != OnOrderCancelled ) OnOrderCancelled( pOrderFrontOfQueue->GetOrderId() );
            }
          }

          break;
        case OrderType::Limit:
          // place into limit book
          assert( 0 < pOrderFrontOfQueue->GetPrice1() );
          switch ( pOrderFrontOfQueue->GetOrderSide() ) {
            case OrderSide::Buy:
              m_mapBids.insert( mapOrderBook_pair_t( pOrderFrontOfQueue->GetPrice1(), pOrderFrontOfQueue ) );
              break;
            case OrderSide::Sell:
              m_mapAsks.insert( mapOrderBook_pair_t( pOrderFrontOfQueue->GetPrice1(), pOrderFrontOfQueue ) );
              break;
            default:
              break;
          }
          break;
        case OrderType::Stop:
          // place into stop book
          assert( 0 < pOrderFrontOfQueue->GetPrice1() );
          switch ( pOrderFrontOfQueue->GetOrderSide() ) {
            case OrderSide::Buy:
              m_mapBuyStops.insert( mapOrderBook_pair_t( pOrderFrontOfQueue->GetPrice1(), pOrderFrontOfQueue ) );
              break;
            case OrderSide::Sell:
              m_mapSellStops.insert( mapOrderBook_pair_t( pOrderFrontOfQueue->GetPrice1(), pOrderFrontOfQueue ) );
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
    }
  }

}

void SimulateOrderExecution::ProcessCancelQueue( const Quote& quote ) {

  // process cancels list
  while ( !m_lCancelDelay.empty() ) {
    if ( ( m_lCancelDelay.front().dtCancellation + m_dtQueueDelay ) >= quote.DateTime() ) {
      break;  // havn't waited long enough to simulate cancel submission
    }
    else {
      structCancelOrder& co = m_lCancelDelay.front();  // capture the information
      bool bOrderFound = false;

      // need a fusion array based upon orders so can zero in on order without looping through all the structures

      // check the delay queue
      for ( lOrderQueue_iter_t iter = m_lOrderDelay.begin(); iter != m_lOrderDelay.end(); ++iter ) {
        if ( co.nOrderId == (*iter)->GetOrderId() ) {
          m_lOrderDelay.erase( iter );
          bOrderFound = true;
          break;
        }
      }

      // check the market order queue
      if ( !bOrderFound ) {
        for ( lOrderQueue_iter_t iter = m_lOrderMarket.begin(); iter != m_lOrderMarket.end(); ++iter ) {
          if ( co.nOrderId == (*iter)->GetOrderId() ) {
            if ( co.nOrderId == m_lOrderMarket.front()->GetOrderId() ) { // check order front of queue
              boost::uint32_t nOrderQuanProcessed = (*iter)->GetQuanFilled();
              if ( 0 != nOrderQuanProcessed ) {  // partially processed order, so commission it out before full cancel
                CalculateCommission( (*iter).get(), nOrderQuanProcessed );
              }
            }
            m_lOrderMarket.erase( iter );
            bOrderFound = true;
            break;
          }
        }
      }

      // need to check orders in ask limit list
      if ( !bOrderFound ) {
        for ( mapOrderBook_iter_t iter = m_mapAsks.begin(); iter != m_mapAsks.end(); ++iter ) {
          if ( co.nOrderId == iter->second->GetOrderId() ) {
            if ( co.nOrderId == m_mapAsks.begin()->second->GetOrderId() ) {
              boost::uint32_t nOrderQuanProcessed = iter->second->GetQuanFilled();
              if ( 0 != nOrderQuanProcessed ) {  // partially processed order, so commission it out before full cancel
                CalculateCommission( iter->second.get(), nOrderQuanProcessed );
              }
            }
            m_mapAsks.erase( iter );
            bOrderFound = true;
            break;
          }
        }
      }

      // need to check orders in bid limit list
      if ( !bOrderFound ) {
        for ( mapOrderBook_iter_t iter = m_mapBids.begin(); iter != m_mapBids.end(); ++iter ) {
          if ( co.nOrderId == iter->second->GetOrderId() ) {
            if ( co.nOrderId == m_mapBids.rbegin()->second->GetOrderId() ) {
              boost::uint32_t nOrderQuanProcessed = iter->second->GetQuanFilled();
              if ( 0 != nOrderQuanProcessed ) {  // partially processed order, so commission it out before full cancel
                CalculateCommission( iter->second.get(), nOrderQuanProcessed );
              }
            }
            m_mapBids.erase( iter );
            bOrderFound = true;
            break;
          }
        }
      }

      // need to check orders in stop list sells
      if ( !bOrderFound ) {
        for ( mapOrderBook_iter_t iter = m_mapSellStops.begin(); iter != m_mapSellStops.end(); ++iter ) {
          if ( co.nOrderId == iter->second->GetOrderId() ) {
            m_mapSellStops.erase( iter );
            bOrderFound = true;
            break;
          }
        }
      }

      // need to check orders in stop list buys
      if ( !bOrderFound ) {
        for ( mapOrderBook_iter_t iter = m_mapBuyStops.begin(); iter != m_mapBuyStops.end(); ++iter ) {
          if ( co.nOrderId == iter->second->GetOrderId() ) {
            m_mapBuyStops.erase( iter );
            bOrderFound = true;
            break;
          }
        }
      }

      if ( !bOrderFound ) {  // need an event for this, as it could be legitimate crossing execution prior to cancel
//        std::cout << "no order found to cancel: " << co.nOrderId << std::endl;
        // todo:  propogate this into the OrderManager
        if ( 0 != OnNoOrderFound ) OnNoOrderFound( co.nOrderId );
      }
      else {
        if ( 0 != OnOrderCancelled ) OnOrderCancelled( co.nOrderId );
      }
      m_lCancelDelay.pop_front();  // remove from list
    }
  }

}

} // namespace tf
} // namespace ou

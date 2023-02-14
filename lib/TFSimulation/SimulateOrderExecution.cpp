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

#include <boost/log/trivial.hpp>

#include <boost/lexical_cast.hpp>

#include <OUCommon/TimeSource.h>

#include "SimulateOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace sim { // simulation

int OrderExecution::m_nExecId( 1000 );

OrderExecution::OrderExecution()
: m_dtQueueDelay( milliseconds( 250 ) )
, m_dblCommission( 1.00 )
{
}

OrderExecution::~OrderExecution() {
}

std::string OrderExecution::GetExecId() {
  std::string sId = boost::lexical_cast<std::string>( m_nExecId++ );
  assert( 0 != sId.length() );
  return sId;
}

void OrderExecution::NewQuote( const Quote& quote ) {
  ProcessOrderQueues( quote );
  m_lastQuote = quote;
}

void OrderExecution::NewDepthByMM( const DepthByMM& depth ) {
//  ProcessOrderQueues( quote );
//  m_lastQuote = quote;
}

void OrderExecution::NewDepthByOrder( const DepthByOrder& depth ) {
//  ProcessOrderQueues( quote );
//  m_lastQuote = quote;
}

void OrderExecution::NewTrade( const Trade& trade ) {
  ProcessLimitOrders( trade );
}

void OrderExecution::SubmitOrder( pOrder_t pOrder ) {
  BOOST_LOG_TRIVIAL(info)
    << "simulate," << pOrder->GetOrderId() << ",queued,submit," << pOrder->GetInstrument()->GetInstrumentName();
  m_lOrderDelay.push_back( pOrder );
}

void OrderExecution::CancelOrder( Order::idOrder_t nOrderId ) {
  BOOST_LOG_TRIVIAL(info)
    << "simulate," << nOrderId << ",queued,cancel";
  QueuedCancelOrder qco( ou::TimeSource::LocalCommonInstance().Internal(), nOrderId );
  m_lCancelDelay.push_back( qco );
}

void OrderExecution::CalculateCommission( Order& order, Trade::tradesize_t quan ) {
  // Order or Instrument should have commission calculation?
  if ( 0 != quan ) {
    if ( nullptr != OnCommission ) {
      double dblCommission {};
      switch ( order.GetInstrument()->GetInstrumentType() ) {
        case InstrumentType::ETF:
        case InstrumentType::Stock:
          dblCommission = 0.005 * (double) quan;
          if ( 1.00 > dblCommission ) dblCommission = 1.00;
          break;
        case InstrumentType::Option:
          dblCommission = 0.95 * (double) quan;
          break;
        case InstrumentType::Future:
          dblCommission = 2.20 * (double) quan;  // ES-2.20 GC=2.50?
          break;
        case InstrumentType::FuturesOption:
          dblCommission = 1.42 * (double) quan;  // ES=1.42
          break;
        case InstrumentType::Currency:
          break;
        case InstrumentType::Unknown:
          dblCommission = m_dblCommission * (double) quan;
          break;
        default:
          assert( false );
      }
      ou::tf::Order::idOrder_t idOrder( order.GetOrderId() );
      BOOST_LOG_TRIVIAL(info)
        << "simulate," << idOrder << ",commission," << dblCommission;
      OnCommission( idOrder, dblCommission );
    }
  }
}

void OrderExecution::ProcessOrderQueues( const Quote &quote ) {

  // TODO: may need some quality control: futures options are notoriously noisy

  //if ( !quote.IsValid() ) {
  //  return;
  //}

  ProcessCancelQueue( quote );

  ProcessDelayQueue( quote );

  ProcessStopOrders( quote ); // places orders into market orders queue

  bool bProcessed;
  bProcessed = ProcessMarketOrders( quote );
  if ( !bProcessed ) {
    bProcessed = ProcessLimitOrders( quote );
  }

}

void OrderExecution::ProcessStopOrders( const Quote& quote ) {
  // not yet implemented
}

bool OrderExecution::ProcessMarketOrders( const Quote& quote ) {

  //pOrder_t pOrderFrontOfQueue;  // change this so we reference the order directly, makes things a bit faster
  bool bProcessed = false;

  // process market orders
  if ( !m_lOrderMarket.empty() ) {

    ou::tf::Order& order( *m_lOrderMarket.front() );
    bProcessed = true;

    boost::uint32_t nOrderQuanRemaining = order.GetQuanRemaining();
    assert( 0 != nOrderQuanRemaining );

    // figure out price of execution
    Trade::tradesize_t quanApplied;
    double dblPrice;
    OrderSide::EOrderSide orderSide = order.GetOrderSide();
    switch ( orderSide ) {
      case OrderSide::Buy:
        quanApplied = std::min<Trade::tradesize_t>( nOrderQuanRemaining, quote.AskSize() );
        dblPrice = quote.Ask();
        break;
      case OrderSide::Sell:
        quanApplied = std::min<Trade::tradesize_t>( nOrderQuanRemaining, quote.BidSize() );
        dblPrice = quote.Bid();
        break;
      default:
        throw std::runtime_error( "SimulateOrderExecution::ProcessMarketOrders unknown order side" );
        break;
    }

    // execute order
    if ( nullptr != OnOrderFill ) {
      ou::tf::Order::idOrder_t idOrder( order.GetOrderId() );
      int nId( m_nExecId );  // before it gets incremented in next function
      std::string id = GetExecId();
      BOOST_LOG_TRIVIAL(info)
        << "simulate,"
        << idOrder
        << ",mkt"
        << "," << nId
        << "," << orderSide
        << "," << nOrderQuanRemaining << "-" << quanApplied << "," << dblPrice
        ;
      // using id in first parameter may or may not work
      Execution exec( nId, idOrder, dblPrice, quanApplied, orderSide, "SIMMkt", id );
      OnOrderFill( idOrder, exec );
    }
    else {
      int i = 1;  // we have a problem as nOrderQuanRemaining won't be updated for the next pass through on partial orders
      throw std::runtime_error( "no onorderfill to keep housekeeping in place" );
    }

    nOrderQuanRemaining -= quanApplied;

    // when order done, commission and toss away
    // what happens on cancelled orders and partial fills?
    if ( 0 == nOrderQuanRemaining ) {
      CalculateCommission( order, order.GetQuanFilled() );
      m_lOrderMarket.pop_front();
    }
    else {
      // OrderManager should be calling Order::ReportExecution to update
    }
  }
  return bProcessed;
}

bool OrderExecution::ProcessLimitOrders( const Quote& quote ) {

  //pOrder_t pOrderFrontOfQueue; // change this so we reference the order directly, makes things a bit faster
  bool bProcessed = false;
  boost::uint32_t nOrderQuanRemaining {};

  // todo: what about self's own crossing orders, could fill with out qoute

  if ( !m_mapAsks.empty() ) {
    mapOrderBook_t::value_type& entry( *m_mapAsks.begin() );
    const double bid( quote.Bid() );
    if ( bid >= entry.first ) {
      bProcessed = true;
      ou::tf::Order& order( *entry.second );
      nOrderQuanRemaining = order.GetQuanRemaining();
      assert( 0 != nOrderQuanRemaining );
      Trade::tradesize_t quanApplied = std::min<Trade::tradesize_t>( nOrderQuanRemaining, quote.BidSize() );
      if ( nullptr != OnOrderFill ) {
        std::string id = GetExecId();
        BOOST_LOG_TRIVIAL(info)
          << "simulate,"
          << order.GetOrderId()
          << ",lmt_ask"
          << "," << id
          << "," << nOrderQuanRemaining << "-" << quanApplied << "," << bid
          << "," << order.GetInstrument()->GetInstrumentName()
          ;
        Execution exec( bid, quanApplied, OrderSide::Sell, "SIMLmtSell", id );
        OnOrderFill( order.GetOrderId(), exec );
        nOrderQuanRemaining -= quanApplied;
        if ( 0 == nOrderQuanRemaining ) {
          CalculateCommission( order, order.GetQuanFilled() );
          m_mapAsks.erase( m_mapAsks.begin() );
        }
        else {
          // OrderManager should be calling Order::ReportExecution to update
        }
      }
    }
  }
  if ( !m_mapBids.empty() && !bProcessed) {
    mapOrderBook_t::value_type& entry( *m_mapBids.rbegin() );
    const double ask( quote.Ask() );
    if ( ask <= entry.first ) {
      bProcessed = true;
      ou::tf::Order& order( *entry.second );
      nOrderQuanRemaining = order.GetQuanRemaining();
      assert( 0 != nOrderQuanRemaining );
      Trade::tradesize_t quanApplied = std::min<Trade::tradesize_t>( nOrderQuanRemaining, quote.AskSize() );
      if ( nullptr != OnOrderFill ) {
        std::string id = GetExecId();
        BOOST_LOG_TRIVIAL(info)
          << "simulate,"
          << order.GetOrderId()
          << ",lmt_bid"
          << "," << id
          << "," << nOrderQuanRemaining << "-" << quanApplied << "," << ask
          << "," << order.GetInstrument()->GetInstrumentName()
          ;
        Execution exec( ask, quanApplied, OrderSide::Buy, "SIMLmtBuy", id );
        OnOrderFill( order.GetOrderId(), exec );
        nOrderQuanRemaining -= quanApplied;
        if ( 0 == nOrderQuanRemaining ) {
          CalculateCommission( order, order.GetQuanFilled() );
          m_mapBids.erase( --m_mapBids.rbegin().base() );
        }
        else {
          // OrderManager should be calling Order::ReportExecution to update
        }
      }
    }
  }

  return bProcessed;
}

bool OrderExecution::ProcessLimitOrders( const Trade& trade ) {
  // will need analysis of quote/trade, quotes should reflect results of depletion by a trade

  double ask( trade.Price() );
  if ( !m_mapAsks.empty() ) {
    if ( m_lastQuote.Ask() <= m_mapAsks.begin()->first ) {
      ask = m_lastQuote.Ask();
    }
  }

  double bid( trade.Price() );
  if ( !m_mapBids.empty() ) {
    if ( m_lastQuote.Bid() >= m_mapBids.rbegin()->first ) {
      bid = m_lastQuote.Bid();
    }
  }

  Quote quote( trade.DateTime(), bid, trade.Volume(), ask, trade.Volume() );
  return ProcessLimitOrders( quote );
}

void OrderExecution::ProcessDelayQueue( const Quote& quote ) {

  pOrder_t pOrderFrontOfQueue;  // change this so we reference the order directly, makes things a bit faster

  // process the delay list
  while ( !m_lOrderDelay.empty() ) {
    if ( ( m_lOrderDelay.front()->GetDateTimeOrderSubmitted() + m_dtQueueDelay ) >= quote.DateTime() ) {
      break;
    }
    else {
      pOrderFrontOfQueue = m_lOrderDelay.front();
      ou::tf::Order& order( *pOrderFrontOfQueue );

      //BOOST_LOG_TRIVIAL(info)
      //  << "simulate,"
      //  << order.GetOrderId()
      //  << ",dequeue"
      //  ;

      m_lOrderDelay.pop_front();
      switch ( order.GetOrderType() ) {
        case OrderType::Market:
          // place into market order book
          if ( 0 == m_lOrderMarket.size() ) {
            m_lOrderMarket.push_back( pOrderFrontOfQueue );
          }
          else {
            if ( order.GetOrderSide() == m_lOrderMarket.front()->GetOrderSide() ) {
              m_lOrderMarket.push_back( pOrderFrontOfQueue );
            }
            else {
              // can't have market orders in two different directions
              if ( nullptr != OnOrderCancelled ) OnOrderCancelled( order.GetOrderId() );
            }
          }

          break;
        case OrderType::Limit:
          // place into limit book
          assert( 0 < order.GetPrice1() );
          switch ( order.GetOrderSide() ) {
            case OrderSide::Buy:
              m_mapBids.insert( mapOrderBook_pair_t( order.GetPrice1(), pOrderFrontOfQueue ) );
              break;
            case OrderSide::Sell:
              m_mapAsks.insert( mapOrderBook_pair_t( order.GetPrice1(), pOrderFrontOfQueue ) );
              break;
            default:
              break;
          }
          break;
        case OrderType::Stop:
          // place into stop book
          assert( 0 < order.GetPrice1() );
          switch ( order.GetOrderSide() ) {
            case OrderSide::Buy:
              m_mapBuyStops.insert( mapOrderBook_pair_t( order.GetPrice1(), pOrderFrontOfQueue ) );
              break;
            case OrderSide::Sell:
              m_mapSellStops.insert( mapOrderBook_pair_t( order.GetPrice1(), pOrderFrontOfQueue ) );
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

void OrderExecution::ProcessCancelQueue( const Quote& quote ) {

  // process cancels list
  while ( !m_lCancelDelay.empty() ) {
    if ( ( m_lCancelDelay.front().dtCancellation + m_dtQueueDelay ) >= quote.DateTime() ) {
      break;  // havn't waited long enough to simulate cancel submission
    }
    else {
      QueuedCancelOrder& qco = m_lCancelDelay.front();  // capture the information
      bool bOrderFound = false;

      // need a fusion array based upon orders so can zero in on order without looping through all the structures

      // check the delay queue - change this to a while do
      // not sure if this is even reachable as the cancel comes after an order, which should have no delay remaining
      for ( lOrderQueue_iter_t iter = m_lOrderDelay.begin(); iter != m_lOrderDelay.end(); ++iter ) {
        ou::tf::Order& order( **iter );
        if ( qco.nOrderId == order.GetOrderId() ) {
          m_lOrderDelay.erase( iter );
          bOrderFound = true;
          break;
        }
      }

      // check the market order queue
      if ( !bOrderFound ) {
        for ( lOrderQueue_iter_t iter = m_lOrderMarket.begin(); iter != m_lOrderMarket.end(); ++iter ) {
          ou::tf::Order& order( **iter );
          if ( qco.nOrderId == order.GetOrderId() ) {
            if ( qco.nOrderId == m_lOrderMarket.front()->GetOrderId() ) { // check order front of queue
              boost::uint32_t nOrderQuanProcessed = (*iter)->GetQuanFilled();
              if ( 0 != nOrderQuanProcessed ) {  // partially processed order, so commission it out before full cancel
                CalculateCommission( order, nOrderQuanProcessed );
              }
              else {
                // is there an update to partially complete order? - no as order has been cancelled
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
          ou::tf::Order& order( *iter->second );
          if ( qco.nOrderId == order.GetOrderId() ) {
            if ( qco.nOrderId == m_mapAsks.begin()->second->GetOrderId() ) {
              boost::uint32_t nOrderQuanProcessed = order.GetQuanFilled();
              if ( 0 != nOrderQuanProcessed ) {  // partially processed order, so commission it out before full cancel
                CalculateCommission( order, nOrderQuanProcessed );
              }
              else {
                // is there an update to partially complete order? - no as order has been cancelled
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
          ou::tf::Order& order( *iter->second );
          if ( qco.nOrderId == order.GetOrderId() ) {
            if ( qco.nOrderId == m_mapBids.rbegin()->second->GetOrderId() ) {
              boost::uint32_t nOrderQuanProcessed = order.GetQuanFilled();
              if ( 0 != nOrderQuanProcessed ) {  // partially processed order, so commission it out before full cancel
                CalculateCommission( order, nOrderQuanProcessed );
              }
              else {
                // is there an update to partially complete order? - no as order has been cancelled
              }
            }
            m_mapBids.erase( iter );
            bOrderFound = true;
            break;
          }
        }
      }

      // need to check orders in stop list sells, any partial remaining to commission out? (stop may not be implemented yet)
      if ( !bOrderFound ) {
        for ( mapOrderBook_iter_t iter = m_mapSellStops.begin(); iter != m_mapSellStops.end(); ++iter ) {
          if ( qco.nOrderId == iter->second->GetOrderId() ) {
            m_mapSellStops.erase( iter );
            bOrderFound = true;
            break;
          }
        }
      }

      // need to check orders in stop list buys, any partial remaining to commission out? (stop may not be implemented yet)
      if ( !bOrderFound ) {
        for ( mapOrderBook_iter_t iter = m_mapBuyStops.begin(); iter != m_mapBuyStops.end(); ++iter ) {
          if ( qco.nOrderId == iter->second->GetOrderId() ) {
            m_mapBuyStops.erase( iter );
            bOrderFound = true;
            break;
          }
        }
      }

      if ( !bOrderFound ) {  // need an event for this, as it could be legitimate crossing execution prior to cancel
        //std::cout << "no order found to cancel: " << co.nOrderId << std::endl;
        // todo:  propogate this into the OrderManager
        if ( nullptr != OnNoOrderFound ) OnNoOrderFound( qco.nOrderId );
      }
      else {
        if ( nullptr != OnOrderCancelled ) OnOrderCancelled( qco.nOrderId );
      }
      m_lCancelDelay.pop_front();  // remove from list
    }
  }

}

} // namespace simulation
} // namespace tf
} // namespace ou

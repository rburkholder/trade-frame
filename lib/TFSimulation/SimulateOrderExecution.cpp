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

#include <TFTrading/TradingEnumerations.h>

#include "SimulateOrderExecution.hpp"

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
  m_lastQuote = quote; // should this be: before or after?
}

void OrderExecution::NewDepthByMM( const DepthByMM& depth ) {
}

void OrderExecution::NewDepthByOrder( const DepthByOrder& depth ) {
  // might use this to populate the bid/ask tables
  // queue in the locally generated orders for proper execution sequencing
  // then apply the ou::tf::Trade orders against this list
}

void OrderExecution::NewTrade( const Trade& trade ) {
  ProcessLimitOrders( trade );
}

void OrderExecution::SubmitOrder( pOrder_t pOrder ) {
  // these will be new orders as well as changed orders
  Order::idOrder_t idOrder( pOrder->GetOrderId() );
  BOOST_LOG_TRIVIAL(info)
    << "simulate," << idOrder << ",queued,submit," << pOrder->GetInstrument()->GetInstrumentName();
  m_lOrderDelay.push_back( pOrder );
  TrackOrder( idOrder, OrderState::State::Delay ); // might be new or a change
}

void OrderExecution::CancelOrder( Order::idOrder_t idOrder ) {
  BOOST_LOG_TRIVIAL(info)
    << "simulate," << idOrder << ",queued,cancel";
  QueuedCancelOrder qco( ou::TimeSource::LocalCommonInstance().Internal(), idOrder );
  m_lCancelDelay.push_back( qco );
  TrackOrder( idOrder, OrderState::State::Delay ); // should match an existing order
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
  // called with each new quote

  // TODO: may need some quality control: futures options are notoriously noisy
  //if ( !quote.IsValid() ) {
  //  return;
  //}

  ProcessDelayQueue( quote );

  ProcessStopOrders( quote ); // places orders into market orders queue

  bool bProcessed;
  bProcessed = ProcessMarketOrders( quote );
  if ( !bProcessed ) {
    bProcessed = ProcessLimitOrders( quote );
  }

  ProcessCancelQueue( quote );

}

void OrderExecution::ProcessStopOrders( const Quote& quote ) {
  // not yet implemented
}

bool OrderExecution::ProcessMarketOrders( const Quote& quote ) {

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

    nOrderQuanRemaining -= quanApplied;

    // execute order
    ou::tf::Order::idOrder_t idOrder( order.GetOrderId() );
    int nId( m_nExecId );  // before it gets incremented in next function
    std::string id = GetExecId();
    BOOST_LOG_TRIVIAL(info)
      << "simulate,"
      << idOrder
      << ",mkt"
      << "," << nId
      << "," << ou::tf::OrderSide::Name[ orderSide ]
      << "," << nOrderQuanRemaining << "-" << quanApplied << "," << dblPrice
      ;

    // OrderManager should be calling Order::ReportExecution to update
    if ( nullptr != OnOrderFill ) {
      // using id in first parameter may or may not work
      Execution exec( nId, idOrder, dblPrice, quanApplied, orderSide, "SIMMkt", id );
      OnOrderFill( idOrder, exec );
    }
    else {
      int i = 1;  // we have a problem as nOrderQuanRemaining won't be updated for the next pass through on partial orders
      throw std::runtime_error( "no onorderfill to keep housekeeping in place" );
    }

    CalculateCommission( order, quanApplied );

    // when order done, commission and toss away
    // what happens on cancelled orders and partial fills?
    if ( 0 == nOrderQuanRemaining ) {
      m_lOrderMarket.pop_front();
      MigrateActiveToArchive( idOrder );
    }
    else {
    }
  }

  return bProcessed;
}

bool OrderExecution::ProcessLimitOrders( const Quote& quote ) {

  bool bProcessed( false );
  boost::uint32_t nOrderQuanRemaining {};

  // todo: what about self's own crossing orders, could fill with out qoute

  if ( !m_mapAsks.empty() ) {
    mapOrderBook_ask_t::iterator iterOrderBook( m_mapAsks.begin() );
    const auto [ obKey, obValue ] = *iterOrderBook;
    const boost::posix_time::ptime dt( quote.DateTime() );
    ou::tf::Order& order( *obValue );

    if ( ETimeInForce::GoodTillDate == order.GetTimeInForce() ) {
      if ( dt > order.GetGoodTillDate() ) {
        BOOST_LOG_TRIVIAL(trace)
          << "simulate,gtd,expired"
          << ",lmt_ask"
          << ",quote dt=" << dt
          << ",order gtd=" << order.GetGoodTillDate()
          << "," << order.GetInstrument()->GetInstrumentName()
          ;
        if ( nullptr != OnOrderCancelled ) OnOrderCancelled( order.GetOrderId() );
        m_mapAsks.erase( iterOrderBook );
        MigrateActiveToArchive( order.GetOrderId() );
        bProcessed = true;
      }
    }

    if ( !bProcessed ) {
      const double bid( quote.Bid() );
      if ( bid >= obKey ) {
        if ( 0 < quote.BidSize() ) {

          bProcessed = true;

          ou::tf::Order::idOrder_t idOrder( order.GetOrderId() );
          int nId( m_nExecId );  // before it gets incremented in next function
          std::string id = GetExecId();

          nOrderQuanRemaining = order.GetQuanRemaining();
          if ( 0 == nOrderQuanRemaining ) {
            BOOST_LOG_TRIVIAL(info)
              << "simulate,error"
              << ",lmt_ask"
              << ",size_map=" << m_mapAsks.size()
              << ",order_id=" << idOrder
              << ",exec_id=" << id
              << "," << nOrderQuanRemaining
              << "," << order.GetInstrument()->GetInstrumentName()
              ;
            assert( false );
          }

          Trade::tradesize_t quanApplied = std::min<Trade::tradesize_t>( nOrderQuanRemaining, quote.BidSize() );

          BOOST_LOG_TRIVIAL(info)
            << "simulate"
            << ",lmt_ask"
            << ",size_map=" << m_mapAsks.size()
            << ",order_id=" << idOrder
            << ",exec_id=" << id
            << "," << nOrderQuanRemaining << "-" << quanApplied << "," << bid
            << "," << order.GetInstrument()->GetInstrumentName()
            ;
          nOrderQuanRemaining -= quanApplied;

          if ( nullptr != OnOrderFill ) {
            Execution exec( nId, idOrder, bid, quanApplied, OrderSide::Sell, "SIMLmtSell", id );
            OnOrderFill( idOrder, exec );
          }
          else {
            // OrderManager should be calling Order::ReportExecution to update
          }

          CalculateCommission( order, quanApplied );

          if ( 0 == nOrderQuanRemaining ) {
            BOOST_LOG_TRIVIAL(info)
              << "simulate,lmt_ask,erase=("
              << idOrder << "," << id << ")"
              ;
            m_mapAsks.erase( iterOrderBook );
            MigrateActiveToArchive( idOrder );
          }

        } // 0 < quote.BidSize()
      } // d >= obKey
    } // bProcessed
  }

  if ( !m_mapBids.empty() && !bProcessed) {
    mapOrderBook_bid_t::iterator iterOrderBook( m_mapBids.begin() );
    const auto [ obKey, obValue ] = *iterOrderBook;
    const boost::posix_time::ptime dt( quote.DateTime() );
    ou::tf::Order& order( *obValue );

    if ( ETimeInForce::GoodTillDate == order.GetTimeInForce() ) {
      if ( dt > order.GetGoodTillDate() ) {
        BOOST_LOG_TRIVIAL(trace)
          << "simulate,gtd,expired"
          << ",lmt_bid"
          << ",quote dt=" << dt
          << ",order gtd=" << order.GetGoodTillDate()
          << "," << order.GetInstrument()->GetInstrumentName()
          ;
        if ( nullptr != OnOrderCancelled ) OnOrderCancelled( order.GetOrderId() );
        m_mapBids.erase( iterOrderBook );
        MigrateActiveToArchive( order.GetOrderId() );
        bProcessed = true;
      }
    }

    if ( !bProcessed ) {
      const double ask( quote.Ask() );
      if ( ask <= obKey ) {
        if ( 0 < quote.AskSize() ) {

          bProcessed = true;

          ou::tf::Order::idOrder_t idOrder( order.GetOrderId() );
          int nId( m_nExecId );  // before it gets incremented in next function
          std::string id = GetExecId();

          nOrderQuanRemaining = order.GetQuanRemaining();
          if ( 0 == nOrderQuanRemaining ) {
            BOOST_LOG_TRIVIAL(info)
              << "simulate,error"
              << ",lmt_bid"
              << ",size_map=" << m_mapBids.size()
              << ",order_id=" << idOrder
              << ",exec_id=" << id
              << "," << nOrderQuanRemaining
              << "," << order.GetInstrument()->GetInstrumentName()
              ;
            assert( false );
          }

          Trade::tradesize_t quanApplied = std::min<Trade::tradesize_t>( nOrderQuanRemaining, quote.AskSize() );

          BOOST_LOG_TRIVIAL(info)
            << "simulate"
            << ",lmt_bid"
            << ",size_map=" << m_mapBids.size()
            << ",order_id=" << idOrder
            << ",exec_id=" << id
            << "," << nOrderQuanRemaining << "-" << quanApplied << "," << ask
            << "," << order.GetInstrument()->GetInstrumentName()
            ;
          nOrderQuanRemaining -= quanApplied;

          if ( nullptr != OnOrderFill ) {
            Execution exec( nId, idOrder, ask, quanApplied, OrderSide::Buy, "SIMLmtBuy", id );
            OnOrderFill( idOrder, exec );
          }
          else {
            // OrderManager should be calling Order::ReportExecution to update
          }

          CalculateCommission( order, quanApplied );

          if ( 0 == nOrderQuanRemaining ) {
            BOOST_LOG_TRIVIAL(info)
              << "simulate,lmt_bid,erase=("
              << idOrder << "," << id << ")"
              ;
            // https://stackoverflow.com/questions/1830158/how-to-call-erase-with-a-reverse-iterator
            m_mapBids.erase( iterOrderBook );
            MigrateActiveToArchive( idOrder );
          }
        } // 0 < quote.AskSize()
      } // ask <= obKey
    } // bProcessed
  }

  return bProcessed;
}

bool OrderExecution::ProcessLimitOrders( const Trade& trade ) {
  // will need analysis of quote/trade, quotes should reflect results of depletion by a trade

  if ( false ) { // disable this for now
    double ask( trade.Price() );
    if ( !m_mapAsks.empty() ) {
      if ( m_lastQuote.Ask() <= m_mapAsks.begin()->first ) {
        ask = m_lastQuote.Ask();
      }
    }

    double bid( trade.Price() );
    if ( !m_mapBids.empty() ) {
      if ( m_lastQuote.Bid() >= m_mapBids.begin()->first ) {
        bid = m_lastQuote.Bid();
      }
    }

    Quote quote( trade.DateTime(), bid, trade.Volume(), ask, trade.Volume() );
    //return ProcessLimitOrders( quote );
  }
  return false;
}

void OrderExecution::ProcessDelayQueue( const Quote& quote ) {

  // process the delay list
  while ( !m_lOrderDelay.empty() ) {

    pOrder_t pOrderFrontOfQueue = m_lOrderDelay.front();
    ou::tf::Order& order( *pOrderFrontOfQueue );

    if ( ( order.GetDateTimeOrderSubmitted() + m_dtQueueDelay ) >= quote.DateTime() ) {
      break;
    }
    else {

      //BOOST_LOG_TRIVIAL(info)
      //  << "simulate"
      //  << ",dequeue,"
      //  << order.GetOrderId()
      //  ;

      m_lOrderDelay.pop_front();

      Order::idOrder_t idOrder( order.GetOrderId() );

      if ( IsOrderArchive( idOrder ) ) {
        BOOST_LOG_TRIVIAL(info)
          << "simulate,"
          << idOrder
          << ",archived"
          ;
      }
      else {

        if ( IsOrderActive( idOrder ) ) { // a change order is occuring, so remove old version
          switch ( order.GetOrderType() ) {
            case OrderType::Market:
              assert( false ); // doesn't make sense to do anything else
              break;
            case OrderType::Limit:
              // update the order
                {
                  bool bFound( false );
                  for ( mapOrderBook_ask_t::iterator iter = m_mapAsks.begin(); iter != m_mapAsks.end(); ++iter ) {
                    if ( idOrder == order.GetOrderId() ) {
                      ou::tf::Order& old( *iter->second );
                      assert( OrderType::Limit == old.GetOrderType() );
                      assert( order.GetOrderSide() == old.GetOrderSide() );
                      m_mapAsks.erase( iter );
                      bFound = true;
                      break;
                    }
                  }
                  if ( !bFound ) {
                    for ( mapOrderBook_bid_t::iterator iter = m_mapBids.begin(); iter != m_mapBids.end(); ++iter ) {
                      if ( idOrder == order.GetOrderId() ) {
                        ou::tf::Order& old( *iter->second );
                        assert( OrderType::Limit == old.GetOrderType() );
                        assert( order.GetOrderSide() == old.GetOrderSide() );
                        m_mapBids.erase( iter );
                        break;
                      }
                    }
                  }
                }
              break;
            case OrderType::Stop:
              // update the order
              break;
            default:
              assert( false );
              break;
          }
        }
        else {
          MigrateDelayToActive( idOrder );
        }

        switch ( order.GetOrderType() ) {
          case OrderType::Market:
            // place into market order book
            m_lOrderMarket.push_back( pOrderFrontOfQueue );
            //if ( nullptr != OnOrderCancelled ) OnOrderCancelled( order.GetOrderId() );
            break;
          case OrderType::Limit:
            // place into limit book
            // TODO: can't have limit orders in two different directions
            assert( 0 < order.GetPrice1() );

            switch ( order.GetOrderSide() ) {
              case OrderSide::Sell:
                m_mapAsks.insert( mapOrderBook_ask_t::value_type( order.GetPrice1(), pOrderFrontOfQueue ) );
                break;
              case OrderSide::Buy:
                m_mapBids.insert( mapOrderBook_bid_t::value_type( order.GetPrice1(), pOrderFrontOfQueue ) );
                break;
              default:
                assert( false );
                break;
            }
            break;
          case OrderType::Stop:
            // place into stop book
            assert( 0 < order.GetPrice1() );
            switch ( order.GetOrderSide() ) {
              case OrderSide::Sell:
                m_mapSellStops.insert( mapOrderBook_ask_t::value_type( order.GetPrice1(), pOrderFrontOfQueue ) );
                break;
              case OrderSide::Buy:
                m_mapBuyStops.insert( mapOrderBook_bid_t::value_type( order.GetPrice1(), pOrderFrontOfQueue ) );
                break;
              default:
                assert( false );
                break;
            }
            break;
          default:
            assert( false );
            break;
        }
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
      // right, don't process the delay queue, doesn't make sense temporaly or logically
      //for ( lOrderQueue_iter_t iter = m_lOrderDelay.begin(); iter != m_lOrderDelay.end(); ++iter ) {
      //  ou::tf::Order& order( **iter );
      //  if ( qco.nOrderId == order.GetOrderId() ) {
      //    m_lOrderDelay.erase( iter );
      //    bOrderFound = true;
      //    break;
      //  }
      //}

      // check the market order queue
      if ( !bOrderFound ) {
        for ( lOrderQueue_iter_t iter = m_lOrderMarket.begin(); iter != m_lOrderMarket.end(); ++iter ) {
          ou::tf::Order& order( **iter );
          if ( qco.nOrderId == order.GetOrderId() ) {
            m_lOrderMarket.erase( iter );
            bOrderFound = true;
            break;
          }
        }
      }

      // need to check orders in ask limit list
      if ( !bOrderFound ) {
        for ( mapOrderBook_ask_t::iterator iter = m_mapAsks.begin(); iter != m_mapAsks.end(); ++iter ) {
          ou::tf::Order& order( *iter->second );
          if ( qco.nOrderId == order.GetOrderId() ) {
            m_mapAsks.erase( iter );
            bOrderFound = true;
            break;
          }
        }
      }

      // need to check orders in bid limit list
      if ( !bOrderFound ) {
        for ( mapOrderBook_bid_t::iterator iter = m_mapBids.begin(); iter != m_mapBids.end(); ++iter ) {
          ou::tf::Order& order( *iter->second );
          if ( qco.nOrderId == order.GetOrderId() ) {
            m_mapBids.erase( iter );
            bOrderFound = true;
            break;
          }
        }
      }

      // need to check orders in stop list sells, any partial remaining to commission out? (stop may not be implemented yet)
      if ( !bOrderFound ) {
        for ( mapOrderBook_ask_t::iterator iter = m_mapSellStops.begin(); iter != m_mapSellStops.end(); ++iter ) {
          if ( qco.nOrderId == iter->second->GetOrderId() ) {
            m_mapSellStops.erase( iter );
            bOrderFound = true;
            break;
          }
        }
      }

      // need to check orders in stop list buys, any partial remaining to commission out? (stop may not be implemented yet)
      if ( !bOrderFound ) {
        for ( mapOrderBook_bid_t::iterator iter = m_mapBuyStops.begin(); iter != m_mapBuyStops.end(); ++iter ) {
          if ( qco.nOrderId == iter->second->GetOrderId() ) {
            m_mapBuyStops.erase( iter );
            bOrderFound = true;
            break;
          }
        }
      }

      bool bPop( true );
      if ( bOrderFound ) {  // need an event for this, as it could be legitimate crossing execution prior to cancel
        if ( nullptr != OnOrderCancelled ) OnOrderCancelled( qco.nOrderId );
        MigrateActiveToArchive( qco.nOrderId );
      }
      else {
        mapOrderState_t::iterator iter = m_mapOrderState.find( qco.nOrderId );
        auto state( iter->second.state );
        if ( OrderState::State::Delay == state ) {
          bPop = false;
          break; // skip for now
        }
        else {
          //std::cout << "no order found to cancel: " << co.nOrderId << std::endl;
          // todo:  propogate this into the OrderManager
          //   this actually means that cancel comes through, but order was actually processed
          if ( nullptr != OnNoOrderFound ) OnNoOrderFound( qco.nOrderId );

          // confirm that the order has already been processed
          assert( m_mapOrderState.end() != iter );
          assert( OrderState::State::Archive == state );
        }
      }

      if ( bPop ) {
        m_lCancelDelay.pop_front();  // remove from list
      }

    }
  }

}

void OrderExecution::TrackOrder( Order::idOrder_t idOrder, OrderState::State state ) {
  mapOrderState_t::iterator iter = m_mapOrderState.find( idOrder );
  //assert( m_mapOrderState.end() == iter );
  if ( m_mapOrderState.end() == iter ) {
    auto result = m_mapOrderState.emplace( mapOrderState_t::value_type( idOrder, OrderState( state ) ) );
    assert( result.second );
  }
  else {
    iter->second.nEncounter++;
  }
}

bool OrderExecution::IsOrderArchive( Order::idOrder_t idOrder ) const {
  mapOrderState_t::const_iterator iter = m_mapOrderState.find( idOrder );
  assert( m_mapOrderState.end() != iter );
  return ( OrderState::State::Archive == iter->second.state );
}

bool OrderExecution::IsOrderActive( Order::idOrder_t idOrder ) const {
  mapOrderState_t::const_iterator iter = m_mapOrderState.find( idOrder );
  assert( m_mapOrderState.end() != iter );
  return ( OrderState::State::Active == iter->second.state );
}

bool OrderExecution::IsOrderExist( Order::idOrder_t idOrder ) const {
  mapOrderState_t::const_iterator iter = m_mapOrderState.find( idOrder );
  return ( m_mapOrderState.end() != iter );
}

void OrderExecution::MigrateDelayToActive( Order::idOrder_t idOrder ) {
  mapOrderState_t::iterator iter = m_mapOrderState.find( idOrder );
  assert( m_mapOrderState.end() != iter );
  assert( OrderState::State::Delay == iter->second.state );
  iter->second.state = OrderState::State::Active;
}

void OrderExecution::MigrateActiveToArchive( Order::idOrder_t idOrder ) {
  mapOrderState_t::iterator iter = m_mapOrderState.find( idOrder );
  assert( m_mapOrderState.end() != iter );
  assert( OrderState::State::Active == iter->second.state );
  iter->second.state = OrderState::State::Archive;
}

} // namespace simulation
} // namespace tf
} // namespace ou

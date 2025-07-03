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

#pragma once

// 2012/01/01  could find a way to feed live data in and simulate executions against live quote/tick data
// is this really needed?  useful if no paper trading available

#include <map>
#include <list>
#include <string>
#include <unordered_map>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <TFTimeSeries/DatedDatum.h>

#include <TFTrading/Order.h>
#include <TFTrading/Execution.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace sim { // simulation

class OrderExecution {  // one instance per symbol
public:

  using pOrder_t = Order::pOrder_t;

  OrderExecution();
  ~OrderExecution();

  using OnOrderCancelledHandler = FastDelegate1<Order::idOrder_t>;
  void SetOnOrderCancelled( OnOrderCancelledHandler function ) {
    OnOrderCancelled = function;
  }
  using OnOrderFillHandler = FastDelegate2<Order::idOrder_t, const Execution&>;
  void SetOnOrderFill( OnOrderFillHandler function ) {
    OnOrderFill = function;
  }
  using OnNoOrderFoundHandler = FastDelegate1<Order::idOrder_t>;  // cancelling a non existant order
  void SetOnNoOrderFound( OnNoOrderFoundHandler function ) {
    OnNoOrderFound = function;
  }
  using OnCommissionHandler = FastDelegate2<Order::idOrder_t, double>;  // calculated once order filled
  void SetOnCommission( OnCommissionHandler function ) {
    OnCommission = function;
  }

  void SetOrderDelay( const time_duration &dtOrderDelay ) { m_dtQueueDelay = dtOrderDelay; };
  void SetCommission( double dblCommission ) { m_dblCommission = dblCommission; };

  void NewQuote( const Quote& quote );
  void NewDepthByMM( const DepthByMM& depth ); // has no influence on the self administred order books
  void NewDepthByOrder( const DepthByOrder& depth ); // has no influence on the self administred order books
  void NewTrade( const Trade& trade );

  void SubmitOrder( pOrder_t pOrder );
  void CancelOrder( Order::idOrder_t nOrderId );

protected:
private:

  struct OrderState {
    // prevent repeats, changes, etc
    enum State { Unknown, Delay, Active, Archive } state;
    size_t nEncounter;
    OrderState(): nEncounter( 1 ), state( State::Unknown ) {}
    OrderState( State state_ ): nEncounter( 1 ), state( state_ ) {}
    OrderState( const OrderState& rhs ): nEncounter( rhs.nEncounter ), state( rhs.state ) {}
  };

  using mapOrderState_t = std::unordered_map<Order::idOrder_t,OrderState>;
  mapOrderState_t m_mapOrderState;

  void TrackOrder( Order::idOrder_t, OrderState::State );
  bool IsOrderArchive( Order::idOrder_t ) const;
  bool IsOrderActive( Order::idOrder_t ) const;
  bool IsOrderExist( Order::idOrder_t ) const;
  void MigrateDelayToActive( Order::idOrder_t );
  void MigrateActiveToArchive( Order::idOrder_t );

  struct QueuedCancelOrder {
    ptime dtCancellation;
    Order::idOrder_t nOrderId;
    QueuedCancelOrder( const ptime &dtCancellation_, unsigned long nOrderId_ )
      : dtCancellation( dtCancellation_ ), nOrderId( nOrderId_ ) {};
  };
  boost::posix_time::time_duration m_dtQueueDelay; // used to simulate network / handling delays
  double m_dblCommission;  // currency, per share (need also per trade)

  Quote m_lastQuote;

  OnOrderCancelledHandler OnOrderCancelled;
  OnOrderFillHandler OnOrderFill;
  OnNoOrderFoundHandler OnNoOrderFound;
  OnCommissionHandler OnCommission;

  using lOrderQueue_t = std::list<pOrder_t>;
  using lOrderQueue_iter_t = lOrderQueue_t::iterator;

  std::list<QueuedCancelOrder> m_lCancelDelay; // separate structure for the cancellations, since not an order

  lOrderQueue_t m_lOrderDelay;  // all orders put in delay queue, taken out then processed as limit or market or stop
  lOrderQueue_t m_lOrderMarket;  // market orders to be processed

  using mapOrderBook_ask_t = std::multimap<double,pOrder_t, std::less<double> >;
  mapOrderBook_ask_t m_mapAsks; // lowest at beginning
  mapOrderBook_ask_t m_mapSellStops;  // pending sell stops, turned into market order when touched

  using mapOrderBook_bid_t = std::multimap<double,pOrder_t, std::greater<double> >;
  mapOrderBook_bid_t m_mapBids; // highest at beginning
  mapOrderBook_bid_t m_mapBuyStops;  // pending buy stops, turned into market order when touched

  void ProcessOrderQueues( const Quote& quote );
  void CalculateCommission( Order&, Trade::tradesize_t quan );
  void ProcessCancelQueue( const Quote& quote );
  void ProcessDelayQueue( const Quote& quote );
  void ProcessStopOrders( const Quote& quote ); // true if order executed, not yet implemented
  bool ProcessMarketOrders( const Quote& quote ); // true if order executed
  bool ProcessLimitOrders( const Quote& quote ); // true if order executed
  bool ProcessLimitOrders( const Trade& trade );

  static int m_nExecId;  // static provides unique number across universe of symbols
  std::string GetExecId();

};

} // namespace sim
} // namespace tf
} // namespace ou

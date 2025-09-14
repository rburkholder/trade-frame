/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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

/*
 * File:    TrackOrderBase.hpp
 * Author:  raymond@burkholder.net
 * Project: TFBitsNPieces
 * Created: August 17, 2025 16:45:42
 */

#pragma once

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryShape.h>

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>

#include "TradeState.hpp"

namespace ou { // namespace oneunified
namespace tf { // namespace tradeframe

class TrackBracketOrder;

// inherited by lib/TFBitsNPieces/TrackOrder.hpp
// inherited by lib/TFBitsNPieces/TrackCurrencyOrder.hpp
// inherited by lib/TFBitsNPieces/TrackBracketOrder.hpp
class TrackOrderBase {
  friend TrackBracketOrder;
public:

  using quantity_t = ou::tf::Order::quantity_t;

  using pPosition_t = ou::tf::Position::pPosition_t;

  using fCancelled_t = std::function<void()>; // may recurse back into TrackOrder

  using fOrderCancelled_t = std::function<void( quantity_t, double )>; // filled, if any at price
  using fOrderFilled_t = std::function<void( quantity_t, double )>; // filled, if any at price, no commission

  struct OrderArgs {

    boost::posix_time::ptime dt; // mark the order on the chart
    quantity_t quantity;
    double signal;
    double limit;
    // double stop; // not used here as order tracked outside of order submission
    unsigned int duration; // limit order duration seconds

    fOrderCancelled_t fOrderCancelled;
    fOrderFilled_t    fOrderFilled;

    OrderArgs(): quantity {}, signal {}, limit {}, duration {} {}

    // market
    explicit OrderArgs( boost::posix_time::ptime dt_, quantity_t quantity_, double signal_ )
    : dt( dt_ ), quantity( quantity_ ), signal( signal_ ), limit {}, duration {}
    , fOrderCancelled( nullptr ), fOrderFilled( nullptr )
    {
      assert( 0 < quantity );
    }

    // limit
    explicit OrderArgs( boost::posix_time::ptime dt_, quantity_t quantity_, double signal_, double limit_ )
    : dt( dt_ ), quantity( quantity_ ), signal( signal_ ), limit( limit_ ), duration {}
    , fOrderCancelled( nullptr ), fOrderFilled( nullptr )
    {
      assert( 0 < quantity );
    }

    // limit time limit
    explicit OrderArgs( boost::posix_time::ptime dt_, quantity_t quantity_, double signal_, double limit_, int duration_ )
    : dt( dt_ ), quantity( quantity_ ), signal( signal_ ), limit( limit_ ), duration( duration_ )
    , fOrderCancelled( nullptr ), fOrderFilled( nullptr )
    {
      assert( 0 < quantity );
    }

    void Set( fOrderCancelled_t&& fOrderCancelled_, fOrderFilled_t&& fOrderFilled_ ) {
      fOrderCancelled = std::move( fOrderCancelled_ );
      fOrderFilled = std::move( fOrderFilled );
    }

  };

  TrackOrderBase();
  TrackOrderBase( pPosition_t, ou::ChartDataView&, int slot );
  virtual ~TrackOrderBase();

  virtual void Set( pPosition_t, ou::ChartDataView&, int slot );

  ETradeState& State() { return m_stateTrade; }

  double PriceInterval( const double price ) const;

  void QueryStats( double& unrealized, double& realized, double& commission, double& total ) const;

  void EnterLongLmt( const OrderArgs& ); // enter with Long limit
  void EnterLongMkt( const OrderArgs& ); // enter with long market

  void EnterShortLmt( const OrderArgs& ); // enter with short limit
  void EnterShortMkt( const OrderArgs& ); // enter with short market

  void ExitLongLmt( const OrderArgs& ); // exit short with long limit
  void ExitLongMkt( const OrderArgs& ); // exit short with long market

  void ExitShortLmt( const OrderArgs& ); // exit long with short limit
  void ExitShortMkt( const OrderArgs& ); // exit long with short market

  void Cancel( fCancelled_t&& ); // used by CurrencyTrader, refactor?
  void Close();                  // used by CurrencyTrader, refactor?

  void HandleCancel( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration );

protected:

  using pOrder_t = ou::tf::Order::pOrder_t;
  pOrder_t m_pOrderPending;

  pPosition_t m_pPosition;

  void Common( const OrderArgs&, pOrder_t& );
  void EnterCommon( const OrderArgs&, pOrder_t& );
  void ExitCommon( const OrderArgs&, pOrder_t& );

  double Normalize( double price ) const;

  void SetGoodTill( const OrderArgs&, pOrder_t& );

  virtual void HandleOrderCancelled( const ou::tf::Order& );
  virtual void HandleOrderFilled( const ou::tf::Order& );

  ETradeState m_stateTrade;

  ou::ChartEntryShape m_ceEntrySubmit;
  ou::ChartEntryShape m_ceEntryFill;
  ou::ChartEntryShape m_ceExitSubmit;
  ou::ChartEntryShape m_ceExitFill;

private:

  fCancelled_t m_fCancelled;

  fOrderCancelled_t m_fOrderCancelled;
  fOrderFilled_t m_fOrderFilled;

};

} // namespace tradeframe
} // namespace oneunified

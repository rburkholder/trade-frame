/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    TrackOrder.hpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: April 28, 2024 10:39:16
 */

#pragma once

#include <OUCharting/ChartDataView.h>

#include <OUCharting/ChartEntryShape.h>

#include <TFTimeSeries/DatedDatum.h>

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>

class TrackOrder {
public:

  enum class ETradeState {
    Init,  // initiaize state in current market
    Search,  // looking for long or short enter
    EntrySubmitted, // order has been submitted, waiting for confirmation
    ExitSignal,  // position exists, looking for exit
    ExitSubmitted, // wait for exit to complete
    Cancelling,
    Cancelled,
    NoTrade, // from the config file, no trading, might be a future
    EndOfDayCancel,
    EndOfDayNeutral,
    Done // no more action
    };

  ETradeState m_stateTrade;

  using pPosition_t = ou::tf::Position::pPosition_t;
  using quantity_t = ou::tf::Order::quantity_t;

  pPosition_t m_pPosition;

  TrackOrder();
  ~TrackOrder();

  void Set( quantity_t, pPosition_t, ou::ChartDataView&, int slot );

  void EnterLong( const ou::tf::Quote& );
  void EnterShort( const ou::tf::Quote& );

  void ExitLong( const ou::tf::Quote& );
  void ExitShort( const ou::tf::Quote& );

  void HandleCancel( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration );

protected:
private:

  using pOrder_t = ou::tf::Order::pOrder_t;

  std::string m_sProfitDescription;
  double m_dblProfitMax;
  double m_dblUnRealized;
  double m_dblProfitMin;

  ou::ChartEntryShape m_ceEntrySubmit;
  ou::ChartEntryShape m_ceEntryFill;
  ou::ChartEntryShape m_ceExitSubmit;
  ou::ChartEntryShape m_ceExitFill;

  ou::tf::Order::quantity_t m_quantityToOrder;

  pOrder_t m_pOrderPending;

  void ShowOrder( pOrder_t );

  void HandleOrderCancelled( const ou::tf::Order& );
  void HandleOrderFilled( const ou::tf::Order& );

  void HandleExitOrderCancelled( const ou::tf::Order& ); // unused
  void HandleExitOrderFilled( const ou::tf::Order& ); // unused

  void ExitPosition( const ou::tf::Quote& );

};
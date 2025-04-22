/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    TradeLifeTime.hpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: March 9, 2022 16:38
 */

#pragma once

#include <OUCharting/ChartEntryShape.h>

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>

#include "Indicators.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
  class TreeItem;
  struct PanelOrderButtons_Order;
} // namespace tf
} // namespace ou

class TreeItem;

class TradeLifeTime {
public:

  using TreeItem = ou::tf::TreeItem;

  using fDone_t = std::function<void(TradeLifeTime&)>;
  using pPosition_t = ou::tf::Position::pPosition_t;

  TradeLifeTime( pPosition_t, Indicators&, fDone_t&& );
  TradeLifeTime( TradeLifeTime&& ) = delete;
  TradeLifeTime( const TradeLifeTime& ) = delete;
  virtual ~TradeLifeTime();

  ou::tf::Order::idOrder_t Id() const { return m_pOrderEntry->GetOrderId(); }

  virtual void Cancel();
  virtual void Close();
  virtual void EmitStatus();
  //virtual void Close();  // doesn't make sense here, too many moving parts

protected:

  using pOrder_t = ou::tf::Order::pOrder_t;

  enum class EPositionState {
    InitializeEntry
  , EnteringPosition // sent order, waiting for fill
  , EnteredPosition  // filled, determine safety exit strategy
  , EnteringStop     // sent order, waiting for fill
  , EnteringProfit   // sent profit order, (stop & profit will co-exist)
  , Watching         // actively maintaining stop & profit monitoring
  , Finish           // clean up
  , Done
  };

  EPositionState m_statePosition;

  pPosition_t m_pPosition;

  TreeItem* m_pTreeItem;

  //ou::tf::Quote m_quote;

  pOrder_t m_pOrderProfit;
  pOrder_t m_pOrderEntry;
  pOrder_t m_pOrderStop;

  bool m_bWatching;
  bool m_bWatchStop;

  double m_dblStopCurrent;
  double m_dblStopTrailDelta;

  ou::ChartEntryShape& m_ceBuySubmit;
  ou::ChartEntryShape& m_ceBuyFill;
  ou::ChartEntryShape& m_ceSellSubmit;
  ou::ChartEntryShape& m_ceSellFill;
  ou::ChartEntryShape& m_ceCancelled;

  fDone_t m_fDone;

  void StartWatch();
  void StopWatch();

  virtual void HandleQuote( const ou::tf::Quote& );

  double PriceInterval( double price ) const;
  double NormalizePrice( double price ) const;

  uint32_t Quantity( pPosition_t, const ou::tf::PanelOrderButtons_Order& ) const;

  void HandleOrderCancelled( const ou::tf::Order& );
  void HandleOrderFilled( const ou::tf::Order& );

  void BuildTreeItem( TreeItem* pTreeItemParent, const std::string& sText );

private:

  void ClearOrders();

};

// ===== TradeWithABuy

class TradeWithABuy: public TradeLifeTime {
public:
  TradeWithABuy( pPosition_t, TreeItem*, const ou::tf::PanelOrderButtons_Order&, Indicators&, fDone_t&& );
  virtual ~TradeWithABuy();

  virtual void Cancel();

protected:

  virtual void HandleQuote( const ou::tf::Quote& );

private:
  void HandleEntryOrderCancelled( const ou::tf::Order& );
  void HandleEntryOrderFilled( const ou::tf::Order& );

  void HandleProfitOrderCancelled( const ou::tf::Order& );
  void HandleProfitOrderFilled( const ou::tf::Order& );

  void HandleStopOrderCancelled( const ou::tf::Order& );
  void HandleStopOrderFilled( const ou::tf::Order& );
};

// ===== TradeWithASell

class TradeWithASell: public TradeLifeTime {
public:
  TradeWithASell( pPosition_t, TreeItem*, const ou::tf::PanelOrderButtons_Order&, Indicators&, fDone_t&& );
  virtual ~TradeWithASell();

  virtual void Cancel();

protected:

  virtual void HandleQuote( const ou::tf::Quote& );

private:
  void HandleEntryOrderCancelled( const ou::tf::Order& );
  void HandleEntryOrderFilled( const ou::tf::Order& );

  void HandleProfitOrderCancelled( const ou::tf::Order& );
  void HandleProfitOrderFilled( const ou::tf::Order& );

  void HandleStopOrderCancelled( const ou::tf::Order& );
  void HandleStopOrderFilled( const ou::tf::Order& );
};

// use bracket or parts of of - Position doesn't know how to handle conditional legs
// manually run bracket
// after entry fill, submit the stop and profit
//  if the profit exit met, then cancel the stop, and enter the profit exit

// to check: position may handle the two sides of the bracket closing (both are sells)

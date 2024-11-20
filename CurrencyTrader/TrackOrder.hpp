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

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>

#include "TradeState.hpp"

class TrackOrder {
public:

  using pPosition_t = ou::tf::Position::pPosition_t;
  using quantity_t = ou::tf::Order::quantity_t;

  using fTransferFunds_t = std::function<void(
    ou::tf::Currency::ECurrency, double debit,
    ou::tf::Currency::ECurrency, double credit,
    double commission
    )>;

  using fFillPrice_t = std::function<void(double,double)>; // exchange rate, commission
  using fCancel_t = std::function<void()>;
  using fClose_t = std::function<void()>;

  struct OrderArgs {

    boost::posix_time::ptime dt;
    double signal;
    double limit;
    double profit;
    double stop;
    unsigned int duration; // limit order duration seconds

    OrderArgs(): signal {}, limit {}, profit {}, stop {}, duration {} {}

    explicit OrderArgs( boost::posix_time::ptime dt_, double signal_ )
    : dt( dt_ ), signal( signal_ ), limit {}, profit {}, stop {}, duration {} {}

    explicit OrderArgs( boost::posix_time::ptime dt_, double signal_, double limit_ )
    : dt( dt_ ), signal( signal_ ), limit( limit_ ), profit {}, stop {}, duration {}
    {}

    explicit OrderArgs( boost::posix_time::ptime dt_, double signal_, double limit_, double stop_ )
    : dt( dt_ ), signal( signal_ ), limit( limit_ ), profit {}, stop( stop_ ), duration {}
    {}

    explicit OrderArgs( boost::posix_time::ptime dt_, double signal_, double limit_, int duration_ )
    : dt( dt_ ), signal( signal_ ), limit( limit_ ), profit {}, stop {}, duration( duration_ )
    {}

    explicit OrderArgs( boost::posix_time::ptime dt_, double signal_, double limit_, double stop_, int duration_ )
    : dt( dt_ ), signal( signal_ ), limit( limit_ ), profit {}, stop( stop_ ), duration( duration_ )
    {}
  };

  ETradeState m_stateTrade;

  TrackOrder();
  ~TrackOrder();

  void Set( quantity_t, fTransferFunds_t& );
  void Set( pPosition_t, ou::ChartDataView&, int slot );
  void Set( fFillPrice_t&& );

  double PriceInterval( double price ) const;

  void QueryStats( double& unrealized, double& realized, double& commission, double& total );

  void EnterLongLmt( const OrderArgs& );
  void EnterLongMkt( const OrderArgs& );
  void EnterLongBracket( const OrderArgs& );

  void EnterShortLmt( const OrderArgs& );
  void EnterShortMkt( const OrderArgs& );

  void ExitLongLmt( const OrderArgs& );
  void ExitLongMkt( const OrderArgs& );

  void ExitShortLmt( const OrderArgs& );
  void ExitShortMkt( const OrderArgs& );

  void Cancel( fCancel_t&& );
  void Close( fClose_t&& );

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

  ou::tf::Order::quantity_t m_quantityBaseCurrency;
  fTransferFunds_t m_fTransferFunds;

  fFillPrice_t m_fFillPrice;
  fCancel_t m_fCancelled;
  fClose_t m_fClosed;

  pOrder_t m_pOrderPending;
  pPosition_t m_pPosition;

  ou::tf::Currency::ECurrency m_curBase;
  ou::tf::Currency::ECurrency m_curQuote;

  double Normalize( double price ) const;

  void SetGoodTill( const OrderArgs&, pOrder_t& );

  void Common( const OrderArgs&, pOrder_t& );
  void EnterCommon( const OrderArgs&, pOrder_t& );
  void ExitCommon( const OrderArgs&, pOrder_t& );

  void ShowOrder( pOrder_t& );

  void HandleOrderCancelled( const ou::tf::Order& );
  void HandleOrderFilled( const ou::tf::Order& );

  void HandleExitOrderCancelled( const ou::tf::Order& ); // unused
  void HandleExitOrderFilled( const ou::tf::Order& ); // unused

  void ExitPosition( const ou::tf::Quote& ); // unused

};

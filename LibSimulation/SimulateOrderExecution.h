/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <list>
#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <LibCommon/FastDelegate.h>
using namespace fastdelegate;

#include <LibTimeSeries/DatedDatum.h>
#include <LibTrading/Order.h>
#include <LibTrading/Execution.h>

class CSimulateOrderExecution {  // one object per symbol
public:

  typedef COrder::pOrder_t pOrder_t;

  CSimulateOrderExecution(void);
  ~CSimulateOrderExecution(void);

  typedef FastDelegate1<COrder::orderid_t> OnOrderCancelledHandler;
  void SetOnOrderCancelled( OnOrderCancelledHandler function ) {
    OnOrderCancelled = function;
  }
  typedef FastDelegate2<COrder::orderid_t, const CExecution&> OnOrderFillHandler;
  void SetOnOrderFill( OnOrderFillHandler function ) {
    OnOrderFill = function;
  }
  typedef FastDelegate1<COrder::orderid_t> OnNoOrderFoundHandler;  // cancelling a non existant order
  void SetOnNoOrderFound( OnNoOrderFoundHandler function ) {
    OnNoOrderFound = function;
  }
  typedef FastDelegate2<COrder::orderid_t, double> OnCommissionHandler;  // calculated once order filled
  void SetOnCommission( OnCommissionHandler function ) {
    OnCommission = function;
  }

  void SetOrderDelay( const time_duration &dtOrderDelay ) { m_dtQueueDelay = dtOrderDelay; };
  void SetCommission( double Commission ) { m_dblCommission = Commission; };

  void NewTrade( const CTrade& trade );
  void NewQuote( const CQuote& quote );

  void SubmitOrder( pOrder_t pOrder );
  void CancelOrder( COrder::orderid_t nOrderId );

protected:
  struct structCancelOrder {
    ptime dtCancellation;
    COrder::orderid_t nOrderId;
    structCancelOrder( const ptime &dtCancellation_, unsigned long nOrderId_ ) 
      : dtCancellation( dtCancellation_ ), nOrderId( nOrderId_ ) {};
  };
  boost::posix_time::time_duration m_dtQueueDelay; // used to simulate network / handling delays
  double m_dblCommission;  // currency, per share (need also per trade)

  OnOrderCancelledHandler OnOrderCancelled;
  OnOrderFillHandler OnOrderFill;
  OnNoOrderFoundHandler OnNoOrderFound;
  OnCommissionHandler OnCommission;

  typedef std::list<pOrder_t> lDelayOrder_t;
  typedef lDelayOrder_t::iterator lDelayOrder_iter_t;
  lDelayOrder_t m_lDelayOrder;  // all orders put in delay queue, taken out then processed as limit or market
  std::list<structCancelOrder> m_lDelayCancel;

  CTrade::tradesize_t m_nOrderQuanRemaining;
  CTrade::tradesize_t m_nOrderQuanProcessed;

  pOrder_t m_pCurrentOrder;
  bool m_bOrdersQueued;
  bool m_bCancelsQueued;

  void ProcessDelayQueues( const CTrade &trade );
  void CalculateCommission( COrder::orderid_t nOrderId, CTrade::tradesize_t quan );

  static int m_nExecId;
  std::stringstream m_ssExecId;
  const char *GetExecId( void ) { 
    m_ssExecId.str( "" ); // works according to time tutorial in boost
    //m_ssExecId.flush(); // this does work
    //m_ssExecId.clear(); // this doesn't work
    m_ssExecId << ++m_nExecId << std::endl; 
    assert( 0 != m_ssExecId.str().size() );
    return m_ssExecId.str().c_str();
  }
private:
};

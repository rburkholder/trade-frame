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

#include <map>
#include <list>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;
#include <boost/lexical_cast.hpp>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <TFTimeSeries/DatedDatum.h>
#include <TFTrading/Order.h>
#include <TFTrading/Execution.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class CSimulateOrderExecution {  // one object per symbol
public:

  typedef COrder::pOrder_t pOrder_t;

  CSimulateOrderExecution(void);
  ~CSimulateOrderExecution(void);

  typedef FastDelegate1<COrder::idOrder_t> OnOrderCancelledHandler;
  void SetOnOrderCancelled( OnOrderCancelledHandler function ) {
    OnOrderCancelled = function;
  }
  typedef FastDelegate2<COrder::idOrder_t, const CExecution&> OnOrderFillHandler;
  void SetOnOrderFill( OnOrderFillHandler function ) {
    OnOrderFill = function;
  }
  typedef FastDelegate1<COrder::idOrder_t> OnNoOrderFoundHandler;  // cancelling a non existant order
  void SetOnNoOrderFound( OnNoOrderFoundHandler function ) {
    OnNoOrderFound = function;
  }
  typedef FastDelegate2<COrder::idOrder_t, double> OnCommissionHandler;  // calculated once order filled
  void SetOnCommission( OnCommissionHandler function ) {
    OnCommission = function;
  }

  enum enumExecuteAgainst {
    EAQuotes, EATrades
  };

  void SetExecuteAgainst( enumExecuteAgainst ea ) { 
    assert( ( EAQuotes == ea ) || ( EATrades == ea ) );
    m_ea = ea;
  }
  void SetOrderDelay( const time_duration &dtOrderDelay ) { m_dtQueueDelay = dtOrderDelay; };
  void SetCommission( double Commission ) { m_dblCommission = Commission; };

  void NewTrade( const CTrade& trade );
  void NewQuote( const CQuote& quote );

  void SubmitOrder( pOrder_t pOrder );
  void CancelOrder( COrder::idOrder_t nOrderId );

protected:

  struct structCancelOrder {
    ptime dtCancellation;
    COrder::idOrder_t nOrderId;
    structCancelOrder( const ptime &dtCancellation_, unsigned long nOrderId_ ) 
      : dtCancellation( dtCancellation_ ), nOrderId( nOrderId_ ) {};
  };
  boost::posix_time::time_duration m_dtQueueDelay; // used to simulate network / handling delays
  double m_dblCommission;  // currency, per share (need also per trade)
  enumExecuteAgainst m_ea;

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

  typedef std::multimap<double,pOrder_t> mapOrderBook_t;
  mapOrderBook_t m_mapAsks; // lowest at beginning
  mapOrderBook_t m_mapBids; // highest at end

  pOrder_t m_pCurrentOrder;

  void ProcessDelayQueues( const CQuote &quote );
  void CalculateCommission( COrder* pOrder, CTrade::tradesize_t quan );

  static int m_nExecId;  // static provides unique number across universe of symbols
  void GetExecId( std::string* sId ) { 
    *sId = boost::lexical_cast<std::string>( m_nExecId++ );
    assert( 0 != sId->length() );
    return;
  }
private:
};

} // namespace tf
} // namespace ou

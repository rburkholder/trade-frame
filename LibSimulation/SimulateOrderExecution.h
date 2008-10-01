#pragma once

#include <list>
#include <sstream>

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "FastDelegate.h"
using namespace fastdelegate;

#include "DatedDatum.h"
#include "Order.h"
#include "Execution.h"
#include "TimeSource.h"
#include "Symbol.h"

class CSimulateOrderExecution {  // one object per symbol
public:
  CSimulateOrderExecution(void);
  ~CSimulateOrderExecution(void);

  typedef FastDelegate1<COrder::orderid_t> OnOrderCancelledHandler;
  void SetOnOrderCancelled( OnOrderCancelledHandler function ) {
    OnOrderCancelled = function;
  }
  typedef FastDelegate1<const CExecution &> OnOrderFillHandler;
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

  void NewTrade( CSymbol::trade_t trade );
  void NewQuote( CSymbol::quote_t quote );

  void SubmitOrder( COrder *pOrder );
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

  std::list<COrder *> m_lDelayOrder;  // all orders put in delay queue, taken out then processed as limit or market
  std::list<structCancelOrder> m_lDelayCancel;

  CTrade::tradesize_t m_nOrderQuanRemaining;
  CTrade::tradesize_t m_nOrderQuanProcessed;

  COrder *m_pCurrentOrder;
  bool m_bOrdersQueued;
  bool m_bCancelsQueued;

  CTimeSource ts;

  void ProcessDelayQueues( const CTrade &trade );
  void CalculateCommission( COrder::orderid_t nOrderId, CTrade::tradesize_t quan );

  static int m_nExecId;
  std::stringstream m_ssExecId;
  const char *GetExecId( void ) { 
    m_ssExecId.flush();
    //m_ssExecId.clear();
    m_ssExecId << ++m_nExecId; 
    return m_ssExecId.str().c_str();
  }
private:
};

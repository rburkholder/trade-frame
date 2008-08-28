#pragma once

#include <list>

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "FastDelegate.h"
using namespace fastdelegate;

#include "DatedDatum.h"
#include "Order.h"
#include "Execution.h"

class CSimulateOrderExecution {  // one object per symbol
public:
  CSimulateOrderExecution(void);
  ~CSimulateOrderExecution(void);

  typedef FastDelegate1<unsigned long> OnOrderCancelledHandler;
  void SetOnOrderCancelled( OnOrderCancelledHandler function ) {
    OnOrderCancelled = function;
  }
  typedef FastDelegate1<const CExecution &> OnOrderFillHandler;
  void SetOnOrderFill( OnOrderFillHandler function ) {
    OnOrderFill = function;
  }

  void SetOrderDelay( const time_duration &dtOrderDelay ) { m_dtOrderDelay = dtOrderDelay; };
  void SetCommission( double Commission ) { m_dblCommission = Commission; };
  void NewTrade( const CTrade &trade );
  void NewQuote( const CQuote &quote );

  void SubmitOrder( COrder *pOrder );
  void CancelOrder( unsigned long nOrderId );

protected:
  boost::posix_time::time_duration m_dtOrderDelay; // used to simulate network / handling delays
  double m_dblCommission;  // currency, per share (need also per trade)
  OnOrderCancelledHandler OnOrderCancelled;
  OnOrderFillHandler OnOrderFill;
  std::list<COrder *> m_lDelay;  // all orders put in delay queue, taken out then processed as limit or market
  double m_dtLatestTradePrice;
  CDatedDatum::tradesize_t m_nLastTradeSize;

  void ProcessDelayQueue( const ptime &dtMark );
private:
};

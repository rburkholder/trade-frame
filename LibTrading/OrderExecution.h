#pragma once

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "TradingEnumerations.h"

#include "Order.h"

class COrderExecution {
public:
  COrderExecution(void);
  COrderExecution( COrder *order, ptime dtExecutionTime, unsigned long nExecutedQuantity, OrderSide::enumOrderSide eOrderSide );
  virtual ~COrderExecution(void);
protected:
  COrder *m_pOrder;
  ptime m_dtExecutionTime;
  unsigned long m_nExecutedQuantity;
  OrderSide::enumOrderSide m_eOrderSide;
  ExecutionStatus::enumExecutionStatus m_eExecutionStatus;
private:
};

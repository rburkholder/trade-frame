#pragma once

#include "ProviderInterface.h"
#include "TradingEnumerations.h"

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <string>

class COrder {
public:
  COrder(void);
  COrder( 
    CProviderInterface *provider, 
    const std::string &sSymbol, 
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderAmount,
    ptime dtOrderSubmitted
    );
  virtual ~COrder(void);
protected:
  std::string m_sSymbol;
  unsigned long m_nOrderId;
  OrderSide::enumOrderSide m_eOrderSide;
  unsigned long m_nOrderQuantity;
  unsigned long m_nOrderQuantityRemaining;

  CProviderInterface *m_pProvider;  // associated provider
  ptime m_dtOrderSubmitted;
  ptime m_dtOrderCancelled;
  ptime m_dtOrderExecuted;
private:
};

#ifndef execution_def
#define execution_def

#include "IBString.h"

struct Execution
{
   Execution()
   {
      shares = 0;
      price = 0;
      permId = 0;
      clientId = 0;
      orderId = 0;
	  cumQty = 0;
	  avgPrice = 0;
   }

   // main order fields
   IBString     execId;
   IBString     time;
   IBString     acctNumber;
   IBString     exchange;
   IBString     side;
   int         shares;
   double      price;
   int         permId;
   long        clientId;
   long        orderId;
   int         liquidation;
   int         cumQty;
   double      avgPrice;
};

struct ExecutionFilter
{
	ExecutionFilter()
		: m_clientId(0)
	{
	}

   // Filter fields
   long        m_clientId;
   IBString     m_acctCode;
   IBString     m_time;
   IBString     m_symbol;
   IBString     m_secType;
   IBString     m_exchange;
   IBString     m_side;
};

#endif // execution_def

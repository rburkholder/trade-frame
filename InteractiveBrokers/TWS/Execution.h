#ifndef execution_def
#define execution_def

struct Execution
{
   Execution()
   {
      shares = 0;
      price = 0;
      permId = 0;
      clientId = 0;
      orderId = 0;
   }

   // main order fields
   CString     execId;
   CString     time;
   CString     acctNumber;
   CString     exchange;
   CString     side;
   int         shares;
   double      price;
   int         permId;
   long        clientId;
   long        orderId;
   int         liquidation;
};

struct ExecutionFilter
{
	ExecutionFilter()
		: m_clientId(0)
	{
	}

   // Filter fields
   long        m_clientId;
   CString     m_acctCode;
   CString     m_time;
   CString     m_symbol;
   CString     m_secType;
   CString     m_exchange;
   CString     m_side;
};

#endif // execution_def

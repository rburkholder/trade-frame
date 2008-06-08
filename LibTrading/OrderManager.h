#pragma once

#include <map>

#include "Order.h"
//#include "ProviderInterface.h"
#include "Execution.h"

//
// CMapOrderToProvider
//

class CProviderInterface;

class CMapOrderToProvider {
public:
  CMapOrderToProvider( CProviderInterface *pProvider, COrder *pOrder );
  ~CMapOrderToProvider( void );
  unsigned long GetOrderId( void ) { return m_pOrder->GetOrderId(); };
  CProviderInterface *GetProvider( void ) { return m_pProvider; };
  COrder *GetOrder( void ) { return m_pOrder; };
protected:
  CProviderInterface *m_pProvider;
  COrder *m_pOrder;
private:
};

//
// COrderManager
//

class COrderManager {
public:
  COrderManager(void);
  ~COrderManager(void);
  void PlaceOrder( CProviderInterface *pProvider, COrder *pOrder );
  void CancelOrder( unsigned long nOrderId );
  void ReportExecution( const CExecution &exec );  // feedback from provider
protected:
  typedef std::map<unsigned long, CMapOrderToProvider*> orders_t;
  typedef std::pair<unsigned long, CMapOrderToProvider*> mappair_t;
  static orders_t m_mapActiveOrders;  // two lists in order to minimize lookup times on active orders
  static orders_t m_mapCompletedOrders;
private:
  static unsigned short m_nRefCount;
};

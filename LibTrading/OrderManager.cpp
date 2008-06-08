#include "StdAfx.h"
#include "OrderManager.h"
#include "ProviderInterface.h"

#include <stdexcept>

//
// CMapOrderToProvider
//

CMapOrderToProvider::CMapOrderToProvider(CProviderInterface *pProvider, COrder *pOrder ) 
: m_pProvider( pProvider ), m_pOrder( pOrder )
{
}

CMapOrderToProvider::~CMapOrderToProvider(void) {
  delete m_pOrder;  // perhaps should make pOrder as smartpointer;
}

//
// COrderManager
//

unsigned short COrderManager::m_nRefCount = 0;
COrderManager::orders_t COrderManager::m_mapActiveOrders;
COrderManager::orders_t COrderManager::m_mapCompletedOrders;

COrderManager::COrderManager(void) {
  ++m_nRefCount;
  if ( 1 == m_nRefCount ) {
  }
}

COrderManager::~COrderManager(void) {
  --m_nRefCount;
  if ( 0 == m_nRefCount ) {
    orders_t::iterator iter;
    for ( iter = m_mapActiveOrders.begin(); iter != m_mapActiveOrders.end(); ++iter ) {
      delete iter->second;
    }
    m_mapActiveOrders.clear();
    for ( iter = m_mapCompletedOrders.begin(); iter != m_mapCompletedOrders.end(); ++iter ) {
      delete iter->second;
    }
    m_mapCompletedOrders.clear();
  }
}

void COrderManager::PlaceOrder(CProviderInterface *pProvider, COrder *pOrder) {
  assert( NULL != pProvider );
  assert( NULL != pOrder );
  CMapOrderToProvider *pMapping 
    = new CMapOrderToProvider( pProvider, pOrder );
  m_mapActiveOrders.insert( mappair_t( pOrder->GetOrderId(), pMapping ) );
  pOrder->SetSendingToProvider();
  pProvider->PlaceOrder( pOrder );
}

void COrderManager::CancelOrder(unsigned long nOrderId) {
  orders_t::iterator iter;
  bool bOrderFound = true;
  iter = m_mapActiveOrders.find( nOrderId );
  if ( m_mapActiveOrders.end() == iter ) {
    iter = m_mapCompletedOrders.find( nOrderId );
    if ( m_mapCompletedOrders.end() == iter ) {
      std::cout << "COrderManager::ReportExecution order not found:  " << nOrderId << std::endl;
      throw std::out_of_range( "OrderId not found" );
    }
  }
  iter->second->GetProvider()->CancelOrder( nOrderId );
}

void COrderManager::ReportExecution(const CExecution &exec) {
  orders_t::iterator iter;
  iter = m_mapActiveOrders.find( exec.GetOrderId() );
  bool bOrderFound = true;
  if ( m_mapActiveOrders.end() == iter ) {
    iter = m_mapCompletedOrders.find( exec.GetOrderId() );
    if ( m_mapCompletedOrders.end() == iter ) {
      std::cout << "COrderManager::ReportExecution order not found:  " << exec.GetOrderId() << std::endl;
      bOrderFound = false;
    }
  }
  if ( bOrderFound ) {
    OrderStatus::enumOrderStatus status = iter->second->GetOrder()->ReportExecution( exec );
    switch ( status ) {
      case OrderStatus::Filled:
        // event callback here?
        m_mapCompletedOrders.insert( mappair_t( iter->second->GetOrderId(), iter->second ) );
        m_mapActiveOrders.erase( iter );
        break;
    }
  }
}

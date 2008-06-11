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

COrderManager::orders_t::iterator COrderManager::LocateOrder( unsigned long nOrderId ) {
  orders_t::iterator iter = m_mapActiveOrders.find( nOrderId );
  if ( m_mapActiveOrders.end() == iter ) {
    iter = m_mapCompletedOrders.find( nOrderId );
    if ( m_mapCompletedOrders.end() == iter ) {
      std::cout << "COrderManager::LocateOrder order not found:  " << nOrderId << std::endl;
      throw std::out_of_range( "OrderId not found" );
    }
  }
  return iter;
}

void COrderManager::CancelOrder(unsigned long nOrderId) {
  try {
    LocateOrder( nOrderId )->second->GetProvider()->CancelOrder( nOrderId );
  }
  catch (...) {
    std::cout << "Problems in COrderManager::CancelOrder" << std::endl;
  }
}

void COrderManager::ReportCommission( unsigned long nOrderId, double dblCommission ) {
  try {
    LocateOrder( nOrderId )->second->GetOrder()->SetCommission( dblCommission );
  }
  catch (...) {
    std::cout << "Problems in COrderManager::ReportCommission" << std::endl;
  }
}

void COrderManager::MoveActiveOrderToCompleted( unsigned long nOrderId ) {
  orders_t::iterator iter = m_mapActiveOrders.find( nOrderId );
  if ( m_mapActiveOrders.end() != iter ) {
    m_mapCompletedOrders.insert( mappair_t( nOrderId, iter->second ) );
    m_mapActiveOrders.erase( iter );
    //OnOrderCompleted( *(iter->second->GetOrder()) );
  }
}

void COrderManager::ReportExecution(const CExecution &exec) {
  try {
    orders_t::iterator iter = LocateOrder( exec.GetOrderId() );
    OrderStatus::enumOrderStatus status = 
      iter->second->GetOrder()->ReportExecution( exec );
    switch ( status ) {
      case OrderStatus::Filled:
        MoveActiveOrderToCompleted( exec.GetOrderId() );
        break;
    }
  }
  catch (...) {
    std::cout << "Problems in COrderManager::ReportExecution" << std::endl;
  }
}

void COrderManager::ReportErrors(unsigned long nOrderId, OrderErrors::enumOrderErrors eError) {
  try {
    orders_t::iterator iter = LocateOrder( nOrderId );
    iter->second->GetOrder()->ActOnError( eError );
    MoveActiveOrderToCompleted( nOrderId );
  }
  catch (...) {
    std::cout << "Problems in COrderManager::ReportErrors" << std::endl;
  }
}

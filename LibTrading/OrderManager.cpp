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

#include "StdAfx.h"

#include <stdexcept>

#include "ProviderInterface.h"

#include "OrderManager.h"

//
// COrderManager
//

COrderManager::COrderManager(void) {
}

COrderManager::~COrderManager(void) {
  m_mapActiveOrders.clear();
  m_mapCompletedOrders.clear();
}

void COrderManager::PlaceOrder(CProviderInterfaceBase *pProvider, COrder::pOrder_t pOrder) {
  assert( NULL != pProvider );
  m_mapActiveOrders.insert( pairIdOrder_t( pOrder->GetOrderId(), pairProviderOrder_t( pProvider, pOrder ) ) );
  pOrder->SetSendingToProvider();
}

COrderManager::mapOrders_t::iterator COrderManager::LocateOrder( unsigned long nOrderId ) {
  mapOrders_t::iterator iter = m_mapActiveOrders.find( nOrderId );
  if ( m_mapActiveOrders.end() == iter ) {
    iter = m_mapCompletedOrders.find( nOrderId );
    if ( m_mapCompletedOrders.end() == iter ) {
      std::cout << "COrderManager::LocateOrder order not found:  " << nOrderId << std::endl;
      throw std::out_of_range( "OrderId not found" );
    }
  }
  return iter;
}

void COrderManager::CancelOrder( COrder::orderid_t nOrderId) {
  try {
    mapOrders_t::iterator iter = LocateOrder( nOrderId );
    iter->second.first->CancelOrder( iter->second.second );
  }
  catch (...) {
    std::cout << "Problems in COrderManager::CancelOrder" << std::endl;
  }
}

void COrderManager::ReportCommission( COrder::orderid_t nOrderId, double dblCommission ) {
  try {
    LocateOrder( nOrderId )->second.second->SetCommission( dblCommission );
  }
  catch (...) {
    std::cout << "Problems in COrderManager::ReportCommission" << std::endl;
  }
}

void COrderManager::MoveActiveOrderToCompleted( COrder::orderid_t nOrderId ) {
  mapOrders_t::iterator iter = m_mapActiveOrders.find( nOrderId );
  if ( m_mapActiveOrders.end() != iter ) {
    m_mapCompletedOrders.insert( *iter );
    m_mapActiveOrders.erase( iter );
    //OnOrderCompleted( *(iter->second->GetOrder()) );
  }
}

void COrderManager::ReportExecution( COrder::orderid_t nOrderId, const CExecution& exec) {
  try {
    mapOrders_t::iterator iter = LocateOrder( nOrderId );
    OrderStatus::enumOrderStatus status = 
      iter->second.second->ReportExecution( exec );
    switch ( status ) {
      case OrderStatus::Filled:
        MoveActiveOrderToCompleted( nOrderId );
        break;
    }
  }
  catch (...) {
    std::cout << "Problems in COrderManager::ReportExecution" << std::endl;
  }
}

void COrderManager::ReportErrors( COrder::orderid_t nOrderId, OrderErrors::enumOrderErrors eError) {
  try {
    mapOrders_t::iterator iter = LocateOrder( nOrderId );
    iter->second.second->ActOnError( eError );
    MoveActiveOrderToCompleted( nOrderId );
  }
  catch (...) {
    std::cout << "Problems in COrderManager::ReportErrors" << std::endl;
  }
}

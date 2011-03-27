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

#include "OrderManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

//
// COrderManager
//

COrderManager::COrderManager(void) 
: m_orderIds( Trading::DbFileName, "OrderId" )
{
}

COrderManager::~COrderManager(void) {
  m_mapActiveOrders.clear();
  m_mapCompletedOrders.clear();
}

COrder::idOrder_t COrderManager::CheckOrderId( idOrder_t id ) {
  idOrder_t oldId = m_orderIds.GetCurrentId();
  if ( id > oldId ) {
    m_orderIds.SetNextId( id );
  }
  return oldId;
}

void COrderManager::PlaceOrder(CProviderInterfaceBase *pProvider, pOrder_t pOrder) {
  assert( NULL != pProvider );
  idOrder_t id = m_orderIds.GetNextId();
  pOrder->SetOrderId( id );
  mapOrders_t::iterator iter = m_mapAllOrders.find( id );
  if ( m_mapAllOrders.end() != iter ) {
    std::runtime_error( "COrderManager::PlaceOrder duplicate order placed" );
  }
  m_mapAllOrders.insert( pairIdOrder_t( id, pairProviderOrder_t( pProvider, pOrder ) ) );
  m_mapActiveOrders.insert( pairIdOrder_t( id, pairProviderOrder_t( pProvider, pOrder ) ) );
  pOrder->SetSendingToProvider();
  pProvider->PlaceOrder( pOrder );
}

COrderManager::mapOrders_t::iterator COrderManager::LocateOrder( idOrder_t nOrderId ) {
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

void COrderManager::CancelOrder( idOrder_t nOrderId) {
  try {
    mapOrders_t::iterator iter = LocateOrder( nOrderId );
    iter->second.first->CancelOrder( iter->second.second );
  }
  catch (...) {
    std::cout << "Problems in COrderManager::CancelOrder" << std::endl;
  }
}

void COrderManager::ReportCommission( idOrder_t nOrderId, double dblCommission ) {
  try {
    LocateOrder( nOrderId )->second.second->SetCommission( dblCommission );
  }
  catch (...) {
    std::cout << "Problems in COrderManager::ReportCommission" << std::endl;
  }
}

void COrderManager::MoveActiveOrderToCompleted( idOrder_t nOrderId ) {
  mapOrders_t::iterator iter = m_mapActiveOrders.find( nOrderId );
  if ( m_mapActiveOrders.end() != iter ) {
    m_mapCompletedOrders.insert( *iter );
    m_mapActiveOrders.erase( iter );
    //OnOrderCompleted( *(iter->second->GetOrder()) );
  }
}

void COrderManager::ReportExecution( idOrder_t nOrderId, const CExecution& exec) {
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

void COrderManager::ReportErrors( idOrder_t nOrderId, OrderErrors::enumOrderErrors eError) {
  try {
    mapOrders_t::iterator iter = LocateOrder( nOrderId );
    iter->second.second->ActOnError( eError );
    MoveActiveOrderToCompleted( nOrderId );
  }
  catch (...) {
    std::cout << "Problems in COrderManager::ReportErrors" << std::endl;
  }
}

void COrderManager::RegisterTablesForCreation( void ) {
  m_pDbSession->RegisterTable<COrder::TableCreateDef>( tablenames::sOrder );
  m_pDbSession->RegisterTable<CExecution::TableCreateDef>( tablenames::sExecution );
}

void COrderManager::RegisterRowDefinitions( void ) {
  m_pDbSession->MapRowDefToTableName<COrder::TableRowDef>( tablenames::sOrder );
  m_pDbSession->MapRowDefToTableName<CExecution::TableRowDef>( tablenames::sExecution );
}

void COrderManager::PopulateTables( void ) {
}

} // namespace tf
} // namespace ou

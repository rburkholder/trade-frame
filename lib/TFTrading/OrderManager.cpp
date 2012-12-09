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

#include <OUCommon/TimeSource.h>

#include "OrderManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

//
// COrderManager
//

COrderManager::COrderManager(void) 
//: 
//   m_orderIds( Trading::DbFileName, "OrderId" )  // need to remove dependency on DB4 and migrate to sql
{
}

COrderManager::~COrderManager(void) {
}

Order::idOrder_t COrderManager::CheckOrderId( idOrder_t id ) {
  idOrder_t oldId = m_orderIds.GetCurrentId();
  if ( id > oldId ) {
    m_orderIds.SetNextId( id );
  }
  return oldId;
}

COrderManager::pOrder_t COrderManager::ConstructOrder( // market order
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType, OrderSide::enumOrderSide eOrderSide, 
    boost::uint32_t nOrderQuantity, 
    idPosition_t idPosition
    ) {
  assert( nOrderQuantity > 0 );
  pOrder_t pOrder( new Order( instrument,  eOrderType, eOrderSide, nOrderQuantity, idPosition ) );
  ConstructOrder( pOrder );
  return pOrder;
}

COrderManager::pOrder_t COrderManager::ConstructOrder( // limit or stop
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType, OrderSide::enumOrderSide eOrderSide, 
    boost::uint32_t nOrderQuantity, double dblPrice1,  
    idPosition_t idPosition
    ) {
  assert( nOrderQuantity > 0 );
  assert( dblPrice1 > 0 );
  pOrder_t pOrder( new Order( instrument, eOrderType, eOrderSide, nOrderQuantity, dblPrice1, idPosition ) );
  ConstructOrder( pOrder );
  return pOrder;
}

COrderManager::pOrder_t COrderManager::ConstructOrder( // limit and stop
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType, OrderSide::enumOrderSide eOrderSide, 
    boost::uint32_t nOrderQuantity, double dblPrice1, double dblPrice2,
    idPosition_t idPosition
    ) {
  assert( nOrderQuantity > 0 );
  assert( dblPrice1 > 0 );
  assert( dblPrice2 > 0 );
  pOrder_t pOrder( new Order( instrument, eOrderType, eOrderSide, nOrderQuantity, dblPrice1, dblPrice2, idPosition ) );
  ConstructOrder( pOrder );
  return pOrder;
}

void COrderManager::ConstructOrder( pOrder_t& pOrder ) {
  // obtain an order id, then insert into maps, may need to change how this happens later in order to get rid of db4
  idOrder_t id = m_orderIds.GetNextId();
  pOrder->SetOrderId( id );
  // need to create an exception free way to check that order does not exist, in the meantime:
  try {
    iterOrders_t iter;
    if ( LocateOrder( id, iter ) ) {
      std::cout << "COrderManager::ConstructOrder:  OrderId Already Exists" << std::endl;
    }
    else {
      pairOrderState_t pair( id, structOrderState( pOrder ) );
      m_mapOrders.insert( pair );

      if ( 0 != m_pSession ) {
        // add to database
        assert( 0 != pOrder->GetRow().idPosition );
        ou::db::QueryFields<Order::TableRowDef>::pQueryFields_t pQuery
          = m_pSession->Insert<Order::TableRowDef>( const_cast<Order::TableRowDef&>( pOrder->GetRow() ) );
      }
    }
  }
  catch (...) {
    std::cout << "COrderManager::ConstructOrder:  Major Problems" << std::endl;
  }
  
}

namespace OrderManagerQueries {
  struct UpdateAtPlaceOrder {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "orderstatus", eOrderStatus );
      ou::db::Field( a, "datetimesubmitted", dtOrderSubmitted );
      ou::db::Field( a, "orderid", idOrder );
    }
    Order::idOrder_t idOrder;
    ptime dtOrderSubmitted;
    OrderStatus::enumOrderStatus eOrderStatus;
    UpdateAtPlaceOrder( Order::idOrder_t id, OrderStatus::enumOrderStatus status, ptime dtOrderSubmitted_ )
      : idOrder( id ), dtOrderSubmitted( dtOrderSubmitted_ ), eOrderStatus( status ) {};
  };
}

void COrderManager::PlaceOrder(CProviderInterfaceBase *pProvider, pOrder_t pOrder) {

  try {
    iterOrders_t iter;
    if ( LocateOrder( pOrder->GetOrderId(), iter ) ) {
      assert( NULL != pProvider );
      iter->second.pProvider = pProvider;
      pOrder->SetSendingToProvider();
      pProvider->PlaceOrder( pOrder );
      if ( 0 != m_pSession ) {
        OrderManagerQueries::UpdateAtPlaceOrder 
          update( pOrder->GetOrderId(), pOrder->GetRow().eOrderStatus, pOrder->GetRow().dtOrderSubmitted );
        ou::db::QueryFields<OrderManagerQueries::UpdateAtPlaceOrder>::pQueryFields_t pQuery
          = m_pSession->SQL<OrderManagerQueries::UpdateAtPlaceOrder>( // todo:  cache this query
            "update orders set orderstatus=?, datetimesubmitted=?", update ).Where( "orderid=?" );
      }
    }
    else {
      std::cout << "COrderManager::PlaceOrder:  OrderId Not Found" << std::endl;
    }
  }
  catch (...) {
    std::cout << "COrderManager::PlaceOrder:  Major Problems" << std::endl;
  }
}

namespace OrderManagerQueries {
  struct OrderKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "orderid", idOrder );
    }
    ou::tf::keytypes::idOrder_t idOrder;
    OrderKey( ou::tf::keytypes::idOrder_t idOrder_ ): idOrder( idOrder_ ) {};
  };

  struct ExecutionKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "executionid", idExecution );
    }
    ou::tf::keytypes::idExecution_t idExecution;
    ExecutionKey( ou::tf::keytypes::idExecution_t idExecution_ ): idExecution( idExecution_ ) {};
  };
}

bool COrderManager::LocateOrder( idOrder_t nOrderId, iterOrders_t& iter ) {
  // if not in memory, the load order and executions from disk
  bool bFound = false;
//  iterOrders_t iter = m_mapOrders.find( nOrderId );
  iter = m_mapOrders.find( nOrderId );
  if ( m_mapOrders.end() != iter ) {
    bFound = true;
  }
  else {
    // check in database first, and if found, load order and executions
    if ( 0 != m_pSession ) {
      OrderManagerQueries::OrderKey keyOrder( nOrderId );
      ou::db::QueryFields<OrderManagerQueries::OrderKey>::pQueryFields_t pOrderExistsQuery
        = m_pSession->SQL<OrderManagerQueries::OrderKey>( "select * from orders", keyOrder ).Where( "orderid=?" ).NoExecute();
      m_pSession->Bind<OrderManagerQueries::OrderKey>( pOrderExistsQuery );
      if ( m_pSession->Execute( pOrderExistsQuery ) ) {
        bool bFound = true;
        // load order as well as associated executions
        Order::TableRowDef rowOrder;
        m_pSession->Columns<OrderManagerQueries::OrderKey, Order::TableRowDef>( pOrderExistsQuery, rowOrder );
        if ( 0 == OnOrderNeedsDetails ) {
          throw std::runtime_error( "COrderManager::LocateOrder: needs Order Details Callback" );
        }
        pInstrument_t pInstrument;
        OnOrderNeedsDetails( rowOrder.idInstrument, pInstrument );
        pOrder_t pOrder( new Order( rowOrder, pInstrument ) );
        std::pair<iterOrders_t, bool> response;
        response = m_mapOrders.insert( pairOrderState_t(rowOrder.idOrder, structOrderState( pOrder ) ) );
        if ( false == response.second ) {
          throw std::runtime_error( "COrderManager::LocateOrder:  couldn't insert order into map" );
        }
        iter = response.first;

        // load up executions
        ou::db::QueryFields<OrderManagerQueries::OrderKey>::pQueryFields_t pExecutionQuery
          = m_pSession->SQL<OrderManagerQueries::OrderKey>( "select * from executions", keyOrder ).Where( "orderid=?" ).NoExecute();
        while ( m_pSession->Execute( pExecutionQuery ) ) {
          CExecution::TableRowDef rowExecution;
          m_pSession->Columns<OrderManagerQueries::OrderKey, CExecution::TableRowDef>( pExecutionQuery, rowExecution );
          pExecution_t pExecution( new CExecution( rowExecution ) );
          iter->second.pmapExecutions->insert( pairExecution_t( rowExecution.idExecution, pExecution ) );
        }
      }
    }
  }
//  if ( !bFound ) {
//    std::cout << "COrderManager::LocateOrder order not found:  " << nOrderId << std::endl;
//    throw std::out_of_range( "OrderId not found" );
//  }
//  return iter;
  return bFound;
}

namespace OrderManagerQueries {
  struct UpdateAtOrderClose {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "orderstatus", eOrderStatus );
      ou::db::Field( a, "datetimeclosed", dtOrderClosed );
      ou::db::Field( a, "orderid", idOrder );
    }
    Order::idOrder_t idOrder;
    ptime dtOrderClosed;
    OrderStatus::enumOrderStatus eOrderStatus;
    UpdateAtOrderClose( Order::idOrder_t id, OrderStatus::enumOrderStatus status, ptime dtOrderClosed_ )
      : idOrder( id ), dtOrderClosed( dtOrderClosed_ ), eOrderStatus( status ) {};
  };
}

void COrderManager::CancelOrder( idOrder_t nOrderId) {  // this needs to work in conjunction with ReportCancellation, database update maybe premature
  try {
    mapOrders_t::iterator iter;
    if ( LocateOrder( nOrderId, iter ) ) {
      pOrder_t pOrder = iter->second.pOrder;
      iter->second.pProvider->CancelOrder( pOrder );  // check which fields have changed for the db
    }
    else {
      std::cout << "COrderManager::CancelOrder:  OrderId Not Found" << std::endl;
    }
  }
  catch (...) {
    std::cout << "COrderManager::CancelOrder:  Major Problems" << std::endl;
  }
}

void COrderManager::ReportCancellation( idOrder_t nOrderId ) {
  try {
    iterOrders_t iter;
    if ( LocateOrder( nOrderId, iter ) ) {
      pOrder_t pOrder = iter->second.pOrder;
      pOrder->MarkAsCancelled();
      if ( 0 != m_pSession ) {
        OrderManagerQueries::UpdateAtOrderClose 
          close( pOrder->GetOrderId(), pOrder->GetRow().eOrderStatus, pOrder->GetRow().dtOrderClosed );
        ou::db::QueryFields<OrderManagerQueries::UpdateAtOrderClose>::pQueryFields_t pQuery
          = m_pSession->SQL<OrderManagerQueries::UpdateAtOrderClose>( // todo:  cache this query
            "update orders set orderstatus=?, datetimeclosed=?", close ).Where( "orderid=?" );
      }
    }
    else {
      std::cout << "COrderManager::ReportCancellation:  OrderId Not Found" << std::endl;
    }
  }
  catch (...) {
    std::cout << "COrderManager::ReportCancellation:  Major Problems" << std::endl;
  }
}

namespace OrderManagerQueries {
  struct UpdateOrder {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "orderstatus", eOrderStatus );
      ou::db::Field( a, "quantityremaining", nQuantityRemaining );
      ou::db::Field( a, "quantityfilled", nQuantityFilled );
      ou::db::Field( a, "averagefillprice", dblAverageFillPrice );
      ou::db::Field( a, "datetimeclosed", dtClosed );
      ou::db::Field( a, "orderid", idOrder );
    }
    OrderStatus::enumOrderStatus eOrderStatus;
    boost::uint32_t nQuantityRemaining;
    boost::uint32_t nQuantityFilled;
    ptime dtClosed;
    double dblAverageFillPrice;
    Order::idOrder_t idOrder;
    UpdateOrder( Order::idOrder_t idOrder_, OrderStatus::enumOrderStatus eOrderStatus_, 
      boost::uint32_t nQuantityRemaining_, boost::uint32_t nQuantityFilled_, double dblAverageFillPrice_, ptime dtClosed_ = boost::date_time::not_a_date_time )
      : idOrder( idOrder_ ), eOrderStatus( eOrderStatus_ ), 
      nQuantityRemaining( nQuantityRemaining_ ), nQuantityFilled( nQuantityFilled_ ), 
      dblAverageFillPrice( dblAverageFillPrice_ ), dtClosed( dtClosed_ ) {};
  };

  std::string sUpdateOrderQuery( "update orders set orderstatus=?, quantityremaining=?, quantityfilled=?, averagefillprice=?, datetimeclosed=?" );
}

void COrderManager::ReportExecution( idOrder_t nOrderId, const CExecution& exec) { 
  try {
    mapOrders_t::iterator iter;
    if ( LocateOrder( nOrderId, iter ) ) {
      pOrder_t pOrder = iter->second.pOrder;
      OrderStatus::enumOrderStatus status = pOrder->ReportExecution( exec );
      if ( 0 != m_pSession ) {
        const Order::TableRowDef& row( pOrder->GetRow() );
        switch ( status ) {
        case OrderStatus::CancelledWithPartialFill:
        case OrderStatus::Filled:
          {
            OrderManagerQueries::UpdateOrder 
              order( nOrderId, row.eOrderStatus, row.nQuantityRemaining, row.nQuantityFilled, row.dblAverageFillPrice, ou::TimeSource::LocalCommonInstance().Internal() );
            ou::db::QueryFields<OrderManagerQueries::UpdateOrder>::pQueryFields_t pQuery
              = m_pSession->SQL<OrderManagerQueries::UpdateOrder>( // todo:  cache this query
              OrderManagerQueries::sUpdateOrderQuery, order ).Where( "orderid=?" );
          }
          break;
        default:
          {
            OrderManagerQueries::UpdateOrder 
              order( nOrderId, row.eOrderStatus, row.nQuantityRemaining, row.nQuantityFilled, row.dblAverageFillPrice );
            ou::db::QueryFields<OrderManagerQueries::UpdateOrder>::pQueryFields_t pQuery
              = m_pSession->SQL<OrderManagerQueries::UpdateOrder>( // todo:  cache this query
              OrderManagerQueries::sUpdateOrderQuery, order ).Where( "orderid=?" );
          }
          break;
        }
        // add execution record
        pExecution_t pExecution( new CExecution( exec ) );
        pExecution->SetOrderId( nOrderId );
        ou::db::QueryFields<CExecution::TableRowDefNoKey>::pQueryFields_t pQueryExecutionWrite
          = m_pSession->Insert<CExecution::TableRowDefNoKey>( 
            const_cast<CExecution::TableRowDefNoKey&>( dynamic_cast<const CExecution::TableRowDefNoKey&>( pExecution->GetRow() ) ) );
        idExecution_t idExecution = m_pSession->GetLastRowId();
        pairExecution_t pair( idExecution, pExecution );
        iter->second.pmapExecutions->insert( pair );
      }
  //    switch ( status ) {
  //      case OrderStatus::Filled:
          //MoveActiveOrderToCompleted( nOrderId );
  //        break;
  //    }
    }
    else {
      std::cout << "COrderManager::ReportExecution:  OrderId not found" << std::endl;
    }
  }
  catch (...) {
    std::cout << "COrderManager::ReportExecution:  Major Problems" << std::endl;
  }
}

namespace OrderManagerQueries {
  struct UpdateCommission {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "commission", dblCommission );
      ou::db::Field( a, "orderid", idOrder );
    }
    Order::idOrder_t idOrder;
    double dblCommission;
    UpdateCommission( Order::idOrder_t id, double dblCommission_ )
      : idOrder( id ), dblCommission( dblCommission_ ) {};
  };
}

void COrderManager::ReportCommission( idOrder_t nOrderId, double dblCommission ) {
  try {
    mapOrders_t::iterator iter;
    if ( LocateOrder( nOrderId, iter ) ) {
      pOrder_t pOrder = iter->second.pOrder;
      if ( 0 != m_pSession ) {
        OrderManagerQueries::UpdateCommission 
          commission( pOrder->GetOrderId(), dblCommission );
        ou::db::QueryFields<OrderManagerQueries::UpdateCommission>::pQueryFields_t pQuery
          = m_pSession->SQL<OrderManagerQueries::UpdateCommission>( // todo:  cache this query
            "update orders set commission=?", commission ).Where( "orderid=?" );
      }
      pOrder->SetCommission( dblCommission );  // need to do afterwards as delegated objects may query the db (other stuff above may not obey this format)
      // as a result, may need to set delegates here so database is updated before order calls delegates.
    }
    else {
      std::cout << "COrderManager::ReportCommission:  Can't locate order id" << std::endl;
    }
  }
  catch (...) {
    std::cout << "COrderManager::ReportCommission:  Major Problems" << std::endl;
  }
}

namespace OrderManagerQueries {
  struct UpdateOnOrderError {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "orderstatus", eOrderStatus );
      ou::db::Field( a, "datetimeclosed", dtOrderClosed );
      ou::db::Field( a, "orderid", idOrder );
    }
    Order::idOrder_t idOrder;
    ptime dtOrderClosed;
    OrderStatus::enumOrderStatus eOrderStatus;
    UpdateOnOrderError( Order::idOrder_t id, OrderStatus::enumOrderStatus status, ptime dtOrderClosed_ )
      : idOrder( id ), dtOrderClosed( dtOrderClosed_ ), eOrderStatus( status ) {};
  };
}

void COrderManager::ReportErrors( idOrder_t nOrderId, OrderErrors::enumOrderErrors eError) {
  try {
    mapOrders_t::iterator iter;
    if ( LocateOrder( nOrderId, iter ) ) {
      pOrder_t pOrder = iter->second.pOrder;
      pOrder->ActOnError( eError );
      //MoveActiveOrderToCompleted( nOrderId );
      if ( 0 != m_pSession ) {
        OrderManagerQueries::UpdateOnOrderError 
          error( pOrder->GetOrderId(), pOrder->GetRow().eOrderStatus, pOrder->GetRow().dtOrderClosed );
        ou::db::QueryFields<OrderManagerQueries::UpdateOnOrderError>::pQueryFields_t pQuery
          = m_pSession->SQL<OrderManagerQueries::UpdateOnOrderError>( // todo:  cache this query
            "update orders set orderstatus=?, datetimeclosed=?", error ).Where( "orderid=?" );
      }
    }
    else {
      std::cout << "COrderManager::ReportErrors:  OrderId not found" << std::endl;
    }
  }
  catch (...) {
    std::cout << "COrderManager::ReportErrors:  Major Problems" << std::endl;
  }
}

void COrderManager::HandleRegisterTables( ou::db::Session& session ) {
  session.RegisterTable<Order::TableCreateDef>( tablenames::sOrder );
  session.RegisterTable<CExecution::TableCreateDef>( tablenames::sExecution );
}

void COrderManager::HandleRegisterRows( ou::db::Session& session ) {
  session.MapRowDefToTableName<Order::TableRowDef>( tablenames::sOrder );
  session.MapRowDefToTableName<CExecution::TableRowDef>( tablenames::sExecution );
  session.MapRowDefToTableName<CExecution::TableRowDefNoKey>( tablenames::sExecution );
}

void COrderManager::HandlePopulateTables( ou::db::Session& session ) {
}

// this stuff could probably be rolled into Session with a template
void COrderManager::AttachToSession( ou::db::Session* pSession ) {
  ManagerBase::AttachToSession( pSession );
  pSession->OnRegisterTables.Add( MakeDelegate( this, &COrderManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Add( MakeDelegate( this, &COrderManager::HandleRegisterRows ) );
  pSession->OnPopulate.Add( MakeDelegate( this, &COrderManager::HandlePopulateTables ) );

}

void COrderManager::DetachFromSession( ou::db::Session* pSession ) {
  pSession->OnRegisterTables.Remove( MakeDelegate( this, &COrderManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Remove( MakeDelegate( this, &COrderManager::HandleRegisterRows ) );
  pSession->OnPopulate.Remove( MakeDelegate( this, &COrderManager::HandlePopulateTables ) );
  ManagerBase::DetachFromSession( pSession );
}

} // namespace tf
} // namespace ou

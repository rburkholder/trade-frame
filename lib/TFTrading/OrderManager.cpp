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

#include "stdafx.h"

#include <OUCommon/TimeSource.h>

#include "OrderManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

//
// OrderManager
//

OrderManager::OrderManager(void) 
//: 
//   m_orderIds( Trading::DbFileName, "OrderId" )  // need to remove dependency on DB4 and migrate to sql
{
}

OrderManager::~OrderManager(void) {
}

Order::idOrder_t OrderManager::CheckOrderId( idOrder_t id ) {
  idOrder_t oldId = m_orderIds.GetCurrentId();
  if ( id > oldId ) {
    m_orderIds.SetNextId( id );
  }
  return oldId;
}

OrderManager::pOrder_t OrderManager::ConstructOrder( // market order
    Instrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType, OrderSide::enumOrderSide eOrderSide, 
    boost::uint32_t nOrderQuantity, 
    idPosition_t idPosition
    ) {
  assert( nOrderQuantity > 0 );
  pOrder_t pOrder( new Order( instrument,  eOrderType, eOrderSide, nOrderQuantity, idPosition ) );
  ConstructOrder( pOrder );
  return pOrder;
}

OrderManager::pOrder_t OrderManager::ConstructOrder( // limit or stop
    Instrument::pInstrument_cref instrument, 
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

OrderManager::pOrder_t OrderManager::ConstructOrder( // limit and stop
    Instrument::pInstrument_cref instrument, 
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

void OrderManager::ConstructOrder( pOrder_t& pOrder ) {
  // obtain an order id, then insert into maps, may need to change how this happens later in order to get rid of db4
  idOrder_t id = m_orderIds.GetNextId();
  pOrder->SetOrderId( id );
  // need to create an exception free way to check that order does not exist, in the meantime:
  try {
    iterOrders_t iter;
    if ( LocateOrder( id, iter ) ) {
      std::cout << "OrderManager::ConstructOrder:  OrderId Already Exists" << std::endl;
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
    std::cout << "OrderManager::ConstructOrder:  Major Problems" << std::endl;
  }
  
}

namespace OrderManagerQueries {
  struct UpdateAtPlaceOrder1 {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "orderstatus", eOrderStatus );
      ou::db::Field( a, "datetimesubmitted", dtOrderSubmitted );
      ou::db::Field( a, "orderid", idOrder );
    }
    Order::idOrder_t idOrder;
    ptime dtOrderSubmitted;
    OrderStatus::enumOrderStatus eOrderStatus;
    UpdateAtPlaceOrder1( Order::idOrder_t id, OrderStatus::enumOrderStatus status, ptime dtOrderSubmitted_ )
      : idOrder( id ), dtOrderSubmitted( dtOrderSubmitted_ ), eOrderStatus( status ) {};
  };
}

void OrderManager::PlaceOrder(ProviderInterfaceBase *pProvider, pOrder_t pOrder) {

  try {
    iterOrders_t iter;
    if ( LocateOrder( pOrder->GetOrderId(), iter ) ) {
      assert( NULL != pProvider );
      iter->second.pProvider = pProvider;
      pOrder->SetSendingToProvider();
      pProvider->PlaceOrder( pOrder );
      if ( 0 != m_pSession ) {
        OrderManagerQueries::UpdateAtPlaceOrder1
          update( pOrder->GetOrderId(), pOrder->GetRow().eOrderStatus, pOrder->GetRow().dtOrderSubmitted );
        ou::db::QueryFields<OrderManagerQueries::UpdateAtPlaceOrder1>::pQueryFields_t pQuery
          = m_pSession->SQL<OrderManagerQueries::UpdateAtPlaceOrder1>( // todo:  cache this query
            "update orders set orderstatus=?, datetimesubmitted=?", update ).Where( "orderid=?" );
      }
    }
    else {
      std::cout << "OrderManager::PlaceOrder:  OrderId Not Found" << std::endl;
    }
  }
  catch (...) {
    std::cout << "OrderManager::PlaceOrder:  Major Problems" << std::endl;
  }
}

namespace OrderManagerQueries {
  struct UpdateAtPlaceOrder2 {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "price1", dblPrice1 );
      ou::db::Field( a, "price2", dblPrice2 );
      ou::db::Field( a, "orderid", idOrder );
    }
    Order::idOrder_t idOrder;
    double dblPrice1;
    double dblPrice2;
    UpdateAtPlaceOrder2( Order::idOrder_t id, double dblPrice1_, double dblPrice2_ )
      : idOrder( id ), dblPrice1( dblPrice1_ ), dblPrice2( dblPrice2_ ) {};
  };
}

void OrderManager::UpdateOrder(ProviderInterfaceBase *pProvider, pOrder_t pOrder) {

  try {
    iterOrders_t iter;
    if ( LocateOrder( pOrder->GetOrderId(), iter ) ) {
      assert( NULL != pProvider );
      iter->second.pProvider = pProvider;
      pOrder->SetSendingToProvider();
      pProvider->PlaceOrder( pOrder );  // for Interactive Brokers, can 'place' again to update, given same order number
      if ( 0 != m_pSession ) {
        OrderManagerQueries::UpdateAtPlaceOrder2
          update( pOrder->GetOrderId(), pOrder->GetRow().dblPrice1, pOrder->GetRow().dblPrice2 );
        ou::db::QueryFields<OrderManagerQueries::UpdateAtPlaceOrder2>::pQueryFields_t pQuery
          = m_pSession->SQL<OrderManagerQueries::UpdateAtPlaceOrder2>( // todo:  cache this query
            "update orders set price1=?, price2=?", update ).Where( "orderid=?" );
      }
    }
    else {
      std::cout << "OrderManager::UpdateOrder:  OrderId Not Found" << std::endl;
    }
  }
  catch (...) {
    std::cout << "OrderManager::UpdateOrder:  Major Problems" << std::endl;
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

bool OrderManager::LocateOrder( idOrder_t nOrderId, iterOrders_t& iter ) {
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
        bFound = true;
        // load order as well as associated executions
        Order::TableRowDef rowOrder;
        m_pSession->Columns<OrderManagerQueries::OrderKey, Order::TableRowDef>( pOrderExistsQuery, rowOrder );
        if ( 0 == OnOrderNeedsDetails ) {
          throw std::runtime_error( "OrderManager::LocateOrder: needs Order Details Callback" );
        }
        pInstrument_t pInstrument;
        OnOrderNeedsDetails( rowOrder.idInstrument, pInstrument );
        pOrder_t pOrder( new Order( rowOrder, pInstrument ) );
        std::pair<iterOrders_t, bool> response;
        response = m_mapOrders.insert( pairOrderState_t(rowOrder.idOrder, structOrderState( pOrder ) ) );
        if ( false == response.second ) {
          throw std::runtime_error( "OrderManager::LocateOrder:  couldn't insert order into map" );
        }
        iter = response.first;

        // load up executions
        ou::db::QueryFields<OrderManagerQueries::OrderKey>::pQueryFields_t pExecutionQuery
          = m_pSession->SQL<OrderManagerQueries::OrderKey>( "select * from executions", keyOrder ).Where( "orderid=?" ).NoExecute();
        while ( m_pSession->Execute( pExecutionQuery ) ) {
          Execution::TableRowDef rowExecution;
          m_pSession->Columns<OrderManagerQueries::OrderKey, Execution::TableRowDef>( pExecutionQuery, rowExecution );
          pExecution_t pExecution( new Execution( rowExecution ) );
          iter->second.pmapExecutions->insert( pairExecution_t( rowExecution.idExecution, pExecution ) );
        }
      }
    }
  }
//  if ( !bFound ) {
//    std::cout << "OrderManager::LocateOrder order not found:  " << nOrderId << std::endl;
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

void OrderManager::CancelOrder( idOrder_t nOrderId) {  // this needs to work in conjunction with ReportCancellation, database update maybe premature
  try {
    mapOrders_t::iterator iter;
    if ( LocateOrder( nOrderId, iter ) ) {
      pOrder_t pOrder = iter->second.pOrder;
      iter->second.pProvider->CancelOrder( pOrder );  // check which fields have changed for the db
    }
    else {
      std::cout << "OrderManager::CancelOrder:  OrderId Not Found" << std::endl;
    }
  }
  catch (...) {
    std::cout << "OrderManager::CancelOrder:  Major Problems" << std::endl;
  }
}

void OrderManager::ReportCancellation( idOrder_t nOrderId ) {
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
      std::cout << "OrderManager::ReportCancellation:  OrderId Not Found" << std::endl;
    }
  }
  catch (...) {
    std::cout << "OrderManager::ReportCancellation:  Major Problems" << std::endl;
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

void OrderManager::ReportExecution( idOrder_t nOrderId, const Execution& exec) { 
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
        pExecution_t pExecution( new Execution( exec ) );
        pExecution->SetOrderId( nOrderId );
        ou::db::QueryFields<Execution::TableRowDefNoKey>::pQueryFields_t pQueryExecutionWrite
          = m_pSession->Insert<Execution::TableRowDefNoKey>( 
            const_cast<Execution::TableRowDefNoKey&>( dynamic_cast<const Execution::TableRowDefNoKey&>( pExecution->GetRow() ) ) );
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
      std::cout << "OrderManager::ReportExecution:  OrderId not found" << std::endl;
    }
  }
  catch (...) {
    std::cout << "OrderManager::ReportExecution:  Major Problems" << std::endl;
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

void OrderManager::ReportCommission( idOrder_t nOrderId, double dblCommission ) {
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
      std::cout << "OrderManager::ReportCommission:  Can't locate order id" << std::endl;
    }
  }
  catch (...) {
    std::cout << "OrderManager::ReportCommission:  Major Problems" << std::endl;
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

void OrderManager::ReportErrors( idOrder_t nOrderId, OrderErrors::enumOrderErrors eError) {
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
      std::cout << "OrderManager::ReportErrors:  OrderId not found" << std::endl;
    }
  }
  catch (...) {
    std::cout << "OrderManager::ReportErrors:  Major Problems" << std::endl;
  }
}

void OrderManager::HandleRegisterTables( ou::db::Session& session ) {
  session.RegisterTable<Order::TableCreateDef>( tablenames::sOrder );
  session.RegisterTable<Execution::TableCreateDef>( tablenames::sExecution );
}

void OrderManager::HandleRegisterRows( ou::db::Session& session ) {
  session.MapRowDefToTableName<Order::TableRowDef>( tablenames::sOrder );
  session.MapRowDefToTableName<Execution::TableRowDef>( tablenames::sExecution );
  session.MapRowDefToTableName<Execution::TableRowDefNoKey>( tablenames::sExecution );
}

void OrderManager::HandlePopulateTables( ou::db::Session& session ) {
}

// this stuff could probably be rolled into Session with a template
void OrderManager::AttachToSession( ou::db::Session* pSession ) {
  ManagerBase::AttachToSession( pSession );
  pSession->OnRegisterTables.Add( MakeDelegate( this, &OrderManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Add( MakeDelegate( this, &OrderManager::HandleRegisterRows ) );
  pSession->OnPopulate.Add( MakeDelegate( this, &OrderManager::HandlePopulateTables ) );

}

void OrderManager::DetachFromSession( ou::db::Session* pSession ) {
  pSession->OnRegisterTables.Remove( MakeDelegate( this, &OrderManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Remove( MakeDelegate( this, &OrderManager::HandleRegisterRows ) );
  pSession->OnPopulate.Remove( MakeDelegate( this, &OrderManager::HandlePopulateTables ) );
  ManagerBase::DetachFromSession( pSession );
}

} // namespace tf
} // namespace ou

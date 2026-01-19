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

#pragma once

// 2010/09/12
// At some point, make order manager responsible for constructing Order

#include <map>

#include <OUCommon/Delegate.h>
#include <OUCommon/ManagerBase.h>

#include "KeyTypes.h"

#include "ProviderInterface.h"
#include "TradingEnumerations.h"
#include "Order.h"
#include "Execution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

//
// OrderManager
//

class ProviderInterfaceBase;

// this is a singleton so use the Instance() call from all users
class OrderManager: public ou::db::ManagerBase<OrderManager> {
public:

  using idInstrument_t = keytypes::idInstrument_t;
  using pInstrument_t = Instrument::pInstrument_t;

  using idPosition_t = keytypes::idPosition_t;

  using idOrder_t = keytypes::idOrder_t;
  using pOrder_t = Order::pOrder_t;

  using idExecution_t = keytypes::idExecution_t;
  using pExecution_t = Execution::pExecution_t;

  OrderManager();
  virtual ~OrderManager();

  pOrder_t ConstructOrder( // market order
    Instrument::pInstrument_cref instrument,
    OrderType::EOrderType eOrderType, OrderSide::EOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity,
    idPosition_t idPosition = 0
    );
  pOrder_t ConstructOrder( // limit or stop
    Instrument::pInstrument_cref instrument,
    OrderType::EOrderType eOrderType, OrderSide::EOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity, double dblPrice1,
    idPosition_t idPosition = 0
    );
  pOrder_t ConstructOrder( // limit and stop
    Instrument::pInstrument_cref instrument,
    OrderType::EOrderType eOrderType, OrderSide::EOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity, double dblPrice1, double dblPrice2,
    idPosition_t idPosition = 0
    );

  void PlaceOrder( ProviderInterfaceBase* pProvider, Order::pOrder_t pOrder );
  void UpdateOrder( ProviderInterfaceBase* pProvider, Order::pOrder_t pOrder );
  void CancelOrder( idOrder_t nOrderId );
  void ReportCancellation( idOrder_t nOrderId );  // feedback from provider
  void ReportExecution( idOrder_t orderId, const Execution& exec );  // feedback from provider
  void ReportCommission( idOrder_t nOrderId, double dblCommission );  // feedback from provider
  void ReportErrors( idOrder_t nOrderId, OrderError::EOrderError eError );  // feedback from provider

  void UpdateReference( idOrder_t nOrderId, const std::string& sReference );

  idOrder_t CheckOrderId( idOrder_t );  // used by ibtws to sync order ids
  void SetOrderId( pOrder_t& pOrder ) { pOrder->SetOrderId( m_orderIds.GetNextId() ); } // bypasses some checks

  // need a query to find pending orders like GTC, etc

  using OnOrderNeedsDetailsHandler = FastDelegate2<idInstrument_t,pInstrument_t&>;
  void SetOnOrderNeedsDetails( OnOrderNeedsDetailsHandler function ) {
    OnOrderNeedsDetails = function;
  }

  void AttachToSession( ou::db::Session* pSession );
  void DetachFromSession( ou::db::Session* pSession );

protected:

  using pairExecution_t = std::pair<idExecution_t, pExecution_t>;
  using mapExecutions_t = std::map<idExecution_t, pExecution_t>;
  using iterExecutions_t = mapExecutions_t::iterator;
  using pmapExecutions_t = boost::shared_ptr<mapExecutions_t>;

  struct structOrderState {
    pOrder_t pOrder;
    ProviderInterfaceBase* pProvider;
    pmapExecutions_t pmapExecutions;
    structOrderState( pOrder_t& pOrder_ )
      : pOrder( pOrder_ ), pProvider( 0 ), pmapExecutions( new mapExecutions_t ) {};
    structOrderState( pOrder_t& pOrder_, ProviderInterfaceBase* pProvider_ )
      : pOrder( pOrder_ ), pProvider( pProvider_ ), pmapExecutions( new mapExecutions_t ) {};
    ~structOrderState() {
//      delete pmapExecutions;  // check that executions have been committed to db?
      // check that orders have been committed to db?
    }
  };

  using pairOrderState_t = std::pair<idOrder_t, structOrderState>;
  using mapOrders_t = std::map<idOrder_t, structOrderState>;  // used for active orders
  using iterOrders_t = mapOrders_t::iterator;

private:

  // AutoIncKey m_orderIds;  // may need to worry about multi-threading at some point in time
  // ToDo:  migrate away from this later on, may need to deal with multiple programs interacting with same
  //  database table, and will need to auto-key from the order table instead.

  struct AutoIncKey {
    int key;
    AutoIncKey() : key( 1 ) {};
    int GetNextId() { return ++key; };
    void SetNextId( int keyNew ) { assert( 0 < keyNew ); key = keyNew; };
    int GetCurrentId() { return key; };
  } m_orderIds;

  mapOrders_t m_mapOrders; // all orders for when checking for consistency

//  iterOrders_t LocateOrder( idOrder_t nOrderId );  // in memory or from disk
  bool LocateOrder( idOrder_t nOrderId, iterOrders_t& );  // in memory or from disk, return true if order found

  OnOrderNeedsDetailsHandler OnOrderNeedsDetails;

  bool ConstructOrder( pOrder_t& pOrder );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );
  void HandlePopulateTables( ou::db::Session& session );
  void HandleLoadTables( ou::db::Session& session );

};

} // namespace tf
} // namespace ou

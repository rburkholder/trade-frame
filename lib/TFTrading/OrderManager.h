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
// At some point, make order manager responsible for constructing COrder

#include <map>
#include <stdexcept>

#include <OUCommon/Delegate.h>

#include <OUBerkeleyDb/AutoIncKeys.h>

#include "KeyTypes.h"

#include "ProviderInterface.h"
#include "TradingEnumerations.h"
#include "ManagerBase.h"
#include "Order.h"
#include "Execution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

//
// COrderManager
//

class CProviderInterfaceBase;

// this is a singleton so use the Instance() call from all users
class COrderManager: public ManagerBase<COrderManager> {
public:

  typedef keytypes::idPosition_t idPosition_t;
  typedef keytypes::idOrder_t idOrder_t;
  typedef COrder::pOrder_t pOrder_t;
  typedef keytypes::idExecution_t idExecution_t;
  typedef CExecution::pExecution_t pExecution_t;

  COrderManager(void);
  ~COrderManager(void);
  pOrder_t ConstructOrder( // market order
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType, OrderSide::enumOrderSide eOrderSide, 
    boost::uint32_t nOrderQuantity, 
    idPosition_t idPosition = 0
    );
  pOrder_t ConstructOrder( // limit or stop
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType, OrderSide::enumOrderSide eOrderSide, 
    boost::uint32_t nOrderQuantity, double dblPrice1,  
    idPosition_t idPosition = 0
    );
  pOrder_t ConstructOrder( // limit and stop
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType, OrderSide::enumOrderSide eOrderSide, 
    boost::uint32_t nOrderQuantity, double dblPrice1, double dblPrice2,
    idPosition_t idPosition = 0
    );
  void PlaceOrder( CProviderInterfaceBase* pProvider, COrder::pOrder_t pOrder );
  void CancelOrder( idOrder_t nOrderId );
  void ReportExecution( idOrder_t orderId, const CExecution& exec );  // feedback from provider
  void ReportCommission( idOrder_t nOrderId, double dblCommission );
  void ReportErrors( idOrder_t nOrderId, OrderErrors::enumOrderErrors eError );
  ou::Delegate<const COrder &> OnOrderCompleted;

  idOrder_t CheckOrderId( idOrder_t );  // used by ibtws to sync order ids

  void RegisterTablesForCreation( void );
  void RegisterRowDefinitions( void );
  void PopulateTables( void );

protected:

  typedef std::pair<idExecution_t, pExecution_t> pairExecution_t;
  typedef std::map<idExecution_t, pExecution_t> mapExecutions_t;
  typedef mapExecutions_t::iterator iterExecutions_t;

  struct structOrder {
    CProviderInterfaceBase* pProvider;
    pOrder_t pOrder;
    mapExecutions_t mapExecutions;
  };

  typedef std::pair<CProviderInterfaceBase*,pOrder_t> pairProviderOrder_t;
  typedef std::pair<idOrder_t, pairProviderOrder_t> pairIdOrder_t;
  typedef std::map<idOrder_t, pairProviderOrder_t> mapOrders_t;

private:

  CAutoIncKey m_orderIds;  // may need to worry about multi-threading at some point in time

  mapOrders_t m_mapActiveOrders;  // two lists in order to minimize lookup times on active orders
  mapOrders_t m_mapCompletedOrders;
  mapOrders_t m_mapAllOrders; // all orders for when checking for consistency
  mapOrders_t::iterator LocateOrder( idOrder_t nOrderId );
  void MoveActiveOrderToCompleted( idOrder_t nOrderId );

  void ConstructOrder( pOrder_t& pOrder );

};

} // namespace tf
} // namespace ou

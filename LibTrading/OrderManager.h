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

#include <map>

#include "LibCommon/Delegate.h"

// need to change the PlaceOrder method to pass a CInstrument rather than symbol name

#include "ManagerBase.h"
#include "Order.h"
#include "Execution.h"

//
// CMapOrderToProvider
//

class CProviderInterfaceBase;

class CMapOrderToProvider {
public:

  CMapOrderToProvider( CProviderInterfaceBase *pProvider, COrder::pOrder_ref pOrder );
  ~CMapOrderToProvider( void );

  unsigned long GetOrderId( void ) { return m_pOrder->GetOrderId(); };
  CProviderInterfaceBase *GetProvider( void ) { return m_pProvider; };
  COrder::pOrder_t GetOrder( void ) { return m_pOrder; };

protected:

  CProviderInterfaceBase *m_pProvider;
  COrder::pOrder_t m_pOrder;

private:
};

//
// COrderManager
//

// this is a singleton so use the Instance() call from all users
class COrderManager: public ManagerBase<COrderManager, COrder::orderid_t, COrder> {
public:

  COrderManager(void);
  ~COrderManager(void);
  void PlaceOrder( CProviderInterfaceBase *pProvider, COrder::pOrder_t pOrder );
  void CancelOrder( COrder::orderid_t nOrderId );
  void ReportExecution( COrder::orderid_t orderId, const CExecution& exec );  // feedback from provider
  void ReportCommission( COrder::orderid_t nOrderId, double dblCommission );
  void ReportErrors( COrder::orderid_t nOrderId, OrderErrors::enumOrderErrors eError );
  Delegate<const COrder &> OnOrderCompleted;

protected:

  typedef std::map<COrder::orderid_t, CMapOrderToProvider*> orders_t;
  typedef std::pair<COrder::orderid_t, CMapOrderToProvider*> mappair_t;
  static orders_t m_mapActiveOrders;  // two lists in order to minimize lookup times on active orders
  static orders_t m_mapCompletedOrders;
  orders_t::iterator LocateOrder( COrder::orderid_t nOrderId );
  void MoveActiveOrderToCompleted( COrder::orderid_t nOrderId );

private:

  static unsigned short m_nRefCount;
};

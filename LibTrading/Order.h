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

// should provider be included?  No, this allows an order routing process to select an 
//   an appropriate provider based upon other criteria
// the provider will be associated later for Execution evaluation

#pragma once

#include <string>

#include "boost/shared_ptr.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "LibCommon/Delegate.h"

#include "TradingEnumerations.h"
#include "Instrument.h"
#include "PersistedOrderId.h"
#include "Execution.h"

class COrder {
public:

  typedef unsigned long orderid_t;
  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef boost::shared_ptr<COrder> pOrder_t;
  typedef const pOrder_t& pOrder_ref;

  COrder(  // market 
    CInstrument::pInstrument_cref instrument, // not deleted here, need a smart pointer
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    ptime dtOrderSubmitted = not_a_date_time
    );
  COrder(  // limit or stop
    CInstrument::pInstrument_cref instrument, // not deleted here, need a smart pointer
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    double dblPrice1,  
    ptime dtOrderSubmitted = not_a_date_time
    );
  COrder(  // limit and stop
    CInstrument::pInstrument_cref instrument, // not deleted here, need a smart pointer
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    double dblPrice1,  
    double dblPrice2,
    ptime dtOrderSubmitted = not_a_date_time
    );
  virtual ~COrder(void);

  void SetOutsideRTH( bool bOutsideRTH ) { m_bOutsideRTH = bOutsideRTH; };
  bool GetOutsideRTH( void ) const { return m_bOutsideRTH; };
  CInstrument::pInstrument_t GetInstrument( void ) const { return m_pInstrument; };
  const char *GetOrderSideName( void ) const { return OrderSide::Name[ m_eOrderSide ]; };
  unsigned long GetQuantity( void ) const { return m_nOrderQuantity; };
  OrderType::enumOrderType GetOrderType( void ) const { return m_eOrderType; };
  OrderSide::enumOrderSide GetOrderSide( void ) const { return m_eOrderSide; };
  double GetPrice1( void ) const { return m_dblPrice1; };  // need to validate this on creation
  double GetPrice2( void ) const { return m_dblPrice2; };
  double GetAverageFillPrice( void ) const { return m_dblAverageFillPrice; };
  orderid_t GetOrderId( void ) const { return m_nOrderId; };
  unsigned long GetNextExecutionId( void ) { return ++m_nNextExecutionId; };
  void SetSendingToProvider( void );
  OrderStatus::enumOrderStatus ReportExecution( const CExecution &exec ); // called from COrderManager
  void SetCommission( double dblCommission ) { m_dblCommission = dblCommission; };
  void ActOnError( OrderErrors::enumOrderErrors eError );
  unsigned long GetQuanRemaining( void ) const { return m_nRemaining; };
  unsigned long GetQuanOrdered( void ) const { return m_nOrderQuantity; };
  unsigned long GetQuanFilled( void ) const { return m_nFilled; };
  void SetSignalPrice( double dblSignalPrice ) { m_dblSignalPrice = dblSignalPrice; };
  double GetSignalPrice( void ) const { return m_dblSignalPrice; };
  const ptime &GetDateTimeOrderSubmitted( void ) const { 
    assert( not_a_date_time != m_dtOrderSubmitted ); 
    return m_dtOrderSubmitted; 
  };
  const ptime &GetDateTimeOrderFilled( void ) const { 
    assert( not_a_date_time != m_dtOrderFilled ); 
    return m_dtOrderFilled; 
  };

  Delegate<std::pair<const COrder&, const CExecution&> > OnExecution;
  Delegate<const COrder&> OnOrderFilled; // on final fill
  Delegate<const COrder&> OnPartialFill; // on intermediate fills only

protected:
  CInstrument::pInstrument_t m_pInstrument;
  orderid_t m_nOrderId;
  OrderType::enumOrderType m_eOrderType;
  OrderSide::enumOrderSide m_eOrderSide;
  unsigned long m_nOrderQuantity;
  //unsigned long m_nOrderQuantityRemaining;
  double m_dblPrice1;  // for limit
  double m_dblPrice2;  // for stop
  double m_dblSignalPrice;  // mark at which algorithm requested order

  //CProviderInterface *m_pProvider;  // associated provider
  ptime m_dtOrderCreated;
  ptime m_dtOrderSubmitted;
  ptime m_dtOrderCancelled;
  ptime m_dtOrderFilled;

  bool m_bOutsideRTH;

  OrderStatus::enumOrderStatus m_eOrderStatus;
  unsigned long m_nNextExecutionId;

  CPersistedOrderId m_persistedorderid;  // make this static?
  void AssignOrderId( void );

  // statistics and status
  //unsigned long m_nTotalOrdered;
  unsigned long m_nFilled;
  unsigned long m_nRemaining;
  double m_dblCommission;
  double m_dblPriceXQuantity; // used for calculating average price
  double m_dblAverageFillPrice;
  //double m_dblAverageFillPriceWithCommission;

private:
  COrder(void);
};

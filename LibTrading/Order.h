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
#include <stdexcept>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <LibSqlite/sqlite3.h>

#include <LibCommon/Delegate.h>

#include "TradingEnumerations.h"
#include "Instrument.h"
#include "Execution.h"

class COrderManager;

class COrder {
  friend class COrderManager;
public:

  typedef sqlite3_int64 idOrder_t;
  typedef sqlite3_int64 idPosition_t;
  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef CInstrument::idInstrument_t idInstrument_t;
  typedef boost::shared_ptr<COrder> pOrder_t;
  typedef const pOrder_t& pOrder_ref;

  COrder(  // market 
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    idPosition_t idPosition = 0,
    ptime dtOrderSubmitted = not_a_date_time
    );
  COrder(  // limit or stop
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    double dblPrice1,  
    idPosition_t idPosition = 0,
    ptime dtOrderSubmitted = not_a_date_time
    );
  COrder(  // limit and stop
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    double dblPrice1,  
    double dblPrice2,
    idPosition_t idPosition = 0,
    ptime dtOrderSubmitted = not_a_date_time
    );
  COrder( idOrder_t idOrder, sqlite3_stmt* pStmt );
  ~COrder(void);

  void SetOutsideRTH( bool bOutsideRTH ) { m_bOutsideRTH = bOutsideRTH; };  // not persisted yet
  bool GetOutsideRTH( void ) const { return m_bOutsideRTH; };
  void SetInstrument( CInstrument::pInstrument_cref pInstrument ) {  // used only when class created from database
    if ( NULL != m_pInstrument.get() ) {
      throw std::runtime_error( "Corder::SetInstrument: instrument already assigned" );
    }
    if ( m_idInstrument != pInstrument->GetInstrumentName() ) {
      throw std::runtime_error( "COrder::SetInstrument: instrument name does not match expected" );
    }
    m_pInstrument = pInstrument;
  }
  CInstrument::pInstrument_t GetInstrument( void ) const { 
    if ( NULL == m_pInstrument.get() ) {
      throw std::runtime_error( "COrder::GetInstrument:  no instrument defined" );
    }
    return m_pInstrument; 
  };
  const char *GetOrderSideName( void ) const { return OrderSide::Name[ m_eOrderSide ]; };
  unsigned long GetQuantity( void ) const { return m_nOrderQuantity; };
  OrderType::enumOrderType GetOrderType( void ) const { return m_eOrderType; };
  OrderSide::enumOrderSide GetOrderSide( void ) const { return m_eOrderSide; };
  double GetPrice1( void ) const { return m_dblPrice1; };  // need to validate this on creation
  double GetPrice2( void ) const { return m_dblPrice2; };
  double GetAverageFillPrice( void ) const { return m_dblAverageFillPrice; };
  idOrder_t GetOrderId( void ) const { assert( 0 != m_idOrder ); return m_idOrder; };
  unsigned long GetNextExecutionId( void ) { return ++m_nNextExecutionId; };
  void SetSendingToProvider( void );
  OrderStatus::enumOrderStatus ReportExecution( const CExecution &exec ); // called from COrderManager
  void SetCommission( double dblCommission );
  double GetCommission( void ) const{ return m_dblCommission; };
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
    assert( not_a_date_time != m_dtOrderClosed ); 
    return m_dtOrderClosed; 
  };

  Delegate<const std::pair<const COrder&, const CExecution&>& > OnExecution;
  Delegate<const COrder&> OnOrderFilled; // on final fill
  Delegate<const COrder&> OnPartialFill; // on intermediate fills only
  Delegate<const COrder&> OnCommission;

  static void CreateDbTable( sqlite3* pDb );
  int BindDbKey( sqlite3_stmt* pStmt );
  int BindDbVariables( sqlite3_stmt* pStmt );
  static const std::string& GetSqlSelect( void ) { return m_sSqlSelect; };
  static const std::string& GetSqlInsert( void ) { return m_sSqlInsert; };
  static const std::string& GetSqlUpdate( void ) { return m_sSqlUpdate; };
  static const std::string& GetSqlDelete( void ) { return m_sSqlDelete; };

protected:

  CInstrument::pInstrument_t m_pInstrument;
  idInstrument_t m_idInstrument;  // used temporarily in order to get instrument_t in place
  
  OrderStatus::enumOrderStatus m_eOrderStatus;
  OrderType::enumOrderType m_eOrderType;
  OrderSide::enumOrderSide m_eOrderSide;

  double m_dblPrice1;  // for limit
  double m_dblPrice2;  // for stop
  double m_dblSignalPrice;  // mark at which algorithm requested order

  ptime m_dtOrderCreated;
  ptime m_dtOrderSubmitted;
  ptime m_dtOrderClosed;

  bool m_bOutsideRTH;

  unsigned long m_nNextExecutionId;

  // statistics and status
  unsigned long m_nOrderQuantity;
  unsigned long m_nFilled;
  unsigned long m_nRemaining;
  double m_dblCommission;
  double m_dblPriceXQuantity; // used for calculating average price
  double m_dblAverageFillPrice;  // excludes commission

  void ConstructOrder( void );
  void SetOrderId( idOrder_t );  // used by OrderManager

private:

  static const std::string m_sSqlCreate;
  static const std::string m_sSqlSelect;
  static const std::string m_sSqlInsert;
  static const std::string m_sSqlUpdate;
  static const std::string m_sSqlDelete;

  std::string m_sDescription;

  idPosition_t m_idPosition;
  idOrder_t m_idOrder;

  COrder(void);  // no default constructor

};

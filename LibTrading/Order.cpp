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

#include <LibCommon/TimeSource.h>

#include "Order.h"

const std::string COrder::m_sSqlCreate(     
  "create table if not exists orders ( \
    orderid INTEGER PRIMARY KEY, \
    version SMALLINT DEFAULT 1, \
    positionid BIGINT NOT NULL, \
    description TEXT NOT NULL, \
    CONSTRAINT fk_orders_positionid \
      FOREIGN KEY(positionid) REFERENCES positions(positionid) \
        ON DELETE RESTRICT ON UPDATE CASCADE \
       \
    );"
 );
const std::string COrder::m_sSqlSelect( "" );
const std::string COrder::m_sSqlInsert( "" );
const std::string COrder::m_sSqlUpdate( "" );
const std::string COrder::m_sSqlDelete( "" );

COrder::COrder(void) {
}

COrder::COrder( 
  CInstrument::pInstrument_cref instrument,
  OrderType::enumOrderType eOrderType,
  OrderSide::enumOrderSide eOrderSide, 
  unsigned long nOrderQuantity,
  ptime dtOrderSubmitted
  ) 
:
  m_pInstrument( instrument ), m_eOrderType( eOrderType ),
  m_eOrderSide( eOrderSide ), m_nOrderQuantity( nOrderQuantity ),
  m_dtOrderSubmitted( dtOrderSubmitted ),
  m_eOrderStatus( OrderStatus::Created ),
  m_nNextExecutionId ( 0 ),
  m_dblCommission( 0 ), m_dblPriceXQuantity( 0 ), m_dblAverageFillPrice( 0 ),
  m_nFilled( 0 ), m_nRemaining( nOrderQuantity ),
  m_dblPrice1( 0 ), m_dblPrice2( 0 ), m_bOutsideRTH( false ),
  m_dblSignalPrice( 0 ),
  m_nOrderId( 0 )
{
  ConstructOrder();
}

COrder::COrder( 
  CInstrument::pInstrument_cref instrument,
  OrderType::enumOrderType eOrderType,
  OrderSide::enumOrderSide eOrderSide, 
  unsigned long nOrderQuantity,
  double dblPrice1,
  ptime dtOrderSubmitted
  ) 
:
  m_pInstrument( instrument ), m_eOrderType( eOrderType ),
  m_eOrderSide( eOrderSide ), m_nOrderQuantity( nOrderQuantity ),
  m_dtOrderSubmitted( dtOrderSubmitted ), 
  m_eOrderStatus( OrderStatus::Created ),
  m_nNextExecutionId ( 0 ),
  m_dblCommission( 0 ), m_dblPriceXQuantity( 0 ), m_dblAverageFillPrice( 0 ),
  m_nFilled( 0 ), m_nRemaining( nOrderQuantity ),
  m_dblPrice1( dblPrice1 ), m_dblPrice2( 0 ), m_bOutsideRTH( false ),
  m_dblSignalPrice( 0 ),
  m_nOrderId( 0 )
{
  ConstructOrder();
}

COrder::COrder( 
  CInstrument::pInstrument_cref instrument,
  OrderType::enumOrderType eOrderType,
  OrderSide::enumOrderSide eOrderSide, 
  unsigned long nOrderQuantity,
  double dblPrice1, double dblPrice2,
  ptime dtOrderSubmitted
  ) 
:
  m_pInstrument( instrument ), m_eOrderType( eOrderType ),
  m_eOrderSide( eOrderSide ), m_nOrderQuantity( nOrderQuantity ),
  m_dtOrderSubmitted( dtOrderSubmitted ), 
  m_eOrderStatus( OrderStatus::Created ),
  m_nNextExecutionId ( 0 ),
  m_dblCommission( 0 ), m_dblPriceXQuantity( 0 ), m_dblAverageFillPrice( 0 ),
  m_nFilled( 0 ), m_nRemaining( nOrderQuantity ),
  m_dblPrice1( dblPrice1 ), m_dblPrice2( dblPrice2 ), m_bOutsideRTH( false ),
  m_dblSignalPrice( 0 ),
  m_nOrderId( 0 )
{
  ConstructOrder();
}

COrder::~COrder(void) {
}

void COrder::ConstructOrder() {
//  try {
  m_dtOrderCreated = CTimeSource::Instance().Internal();
  //m_dtOrderSubmitted = not_a_date_time;  // already set as such
//  m_nOrderId = m_persistedorderid.GetNextOrderId();
//  }
//  catch (...) {
//    bOrderIdOk = false;
//    std::cout << "CIBTWS::PlaceOrder: Couldn't get the next order key." << std::endl;
//  }
}

void COrder::SetSendingToProvider() {
  assert( OrderStatus::Created == m_eOrderStatus );
  m_eOrderStatus = OrderStatus::SendingToProvider;
  m_dtOrderSubmitted = CTimeSource::Instance().Internal();
}

OrderStatus::enumOrderStatus COrder::ReportExecution(const CExecution &exec) { 
  // need to worry about fill after cancel
  assert( exec.GetOrderSide() == m_eOrderSide );
  bool bOverDone = false;
  if ( 0 == m_nRemaining ) {
    // yes this has happened, 2008/07/09 vmw
    std::cout << "Order " << m_nOrderId << " overfilled with +" << exec.GetSize() << std::endl;
    m_eOrderStatus = OrderStatus::OverFilled;
    bOverDone = true;
  }
  else {
    m_nRemaining -= exec.GetSize();
  }
  m_nFilled += exec.GetSize();
  if ( m_nFilled > m_nOrderQuantity ) {
    std:: cout << "Order " << m_nOrderId << " overfilled with +" << exec.GetSize() << std::endl;
    bOverDone = true;
  }
  if ( !bOverDone ) {
    m_dblPriceXQuantity += exec.GetPrice() * exec.GetSize();
    m_dblAverageFillPrice = m_dblPriceXQuantity / m_nFilled;
    if ( 0 == m_nRemaining ) {
      m_eOrderStatus = OrderStatus::Filled;
      m_dtOrderFilled = CTimeSource::Instance().Internal();
      OnOrderFilled( *this );
    }
    else {
      switch ( m_eOrderStatus ) {
      case OrderStatus::SendingToProvider:
      case OrderStatus::Submitted:
      case OrderStatus::Filling:
      case OrderStatus::PreSubmission:
        m_eOrderStatus = OrderStatus::Filling;
        break;
      case OrderStatus::Cancelled:
      case OrderStatus::CancelSubmitted:
      case OrderStatus::FillingDuringCancel:
      case OrderStatus::CancelledWithPartialFill:
        m_eOrderStatus = OrderStatus::FillingDuringCancel;
        break;
      case OrderStatus::OverFilled:
        break;
      default:
        std::cout << "COrder::ReportExecution " << static_cast<char>( m_eOrderStatus ) << std::endl;
        break;
      }
      OnPartialFill( *this );
    }
  }
  OnExecution( std::pair<const COrder&, const CExecution&>( *this, exec ) );
  return m_eOrderStatus;
}

void COrder::ActOnError(OrderErrors::enumOrderErrors eError) {
  switch( eError ) {
    case OrderErrors::Cancelled:
      m_eOrderStatus = OrderStatus::Cancelled;
      break;
    case OrderErrors::Rejected:
    case OrderErrors::InstrumentNotFound:
      m_eOrderStatus = OrderStatus::Rejected;
      break;
    case OrderErrors::NotCancellable:
      break;
  }
}

void COrder::SetCommission( double dblCommission ) { 
  m_dblCommission = dblCommission; 
  OnCommission( *this );
}

void COrder::SetOrderId( orderid_t id ) {
  assert( 0 != id );
  assert( m_nOrderId == 0 );
  m_nOrderId = id;
}

void COrder::CreateDbTable( sqlite3* pDb ) {

  char* pMsg;
  int rtn;

  rtn = sqlite3_exec( pDb, m_sSqlCreate.c_str(), 0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating table orders: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }

  rtn = sqlite3_exec( pDb, 
    "create index idx_orders_positionid on orders( positionid );",
    0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating index idx_orders_positionid: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }
}



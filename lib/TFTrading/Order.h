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

#include <OUCommon/Delegate.h>

#include "TradingEnumerations.h"
#include "KeyTypes.h"
#include "Instrument.h"
#include "Execution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class COrderManager;

class Order {
  friend class COrderManager;
public:

  typedef keytypes::idOrder_t idOrder_t;
  typedef keytypes::idPosition_t idPosition_t;
  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef keytypes::idInstrument_t idInstrument_t;
  typedef boost::shared_ptr<Order> pOrder_t;
  typedef const pOrder_t& pOrder_ref;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "orderid", idOrder );
      ou::db::Field( a, "positionid", idPosition );
      ou::db::Field( a, "instrumentid", idInstrument );
      ou::db::Field( a, "description", sDescription );
      ou::db::Field( a, "orderstatus", eOrderStatus );
      ou::db::Field( a, "ordertype", eOrderType );
      ou::db::Field( a, "orderside", eOrderSide );
      ou::db::Field( a, "price1", dblPrice1 );
      ou::db::Field( a, "price2", dblPrice2 );
      ou::db::Field( a, "signalprice", dblSignalPrice );
      ou::db::Field( a, "quantityordered", nOrderQuantity );
      ou::db::Field( a, "quantityremaining", nQuantityRemaining );
      ou::db::Field( a, "quantityfilled", nQuantityFilled );
      ou::db::Field( a, "quantitypaired", nQuantityPaired );  // used for pairing profitable trades
      ou::db::Field( a, "averagefillprice", dblAverageFillPrice );
      ou::db::Field( a, "commission", dblCommission );
      ou::db::Field( a, "datetimecreated", dtOrderCreated );
      ou::db::Field( a, "datetimesubmitted", dtOrderSubmitted );
      ou::db::Field( a, "datetimeclosed", dtOrderClosed );
    }
    //"create index idx_orders_positionid on orders( positionid );",

    idOrder_t idOrder;
    idPosition_t idPosition;
    idInstrument_t idInstrument;
    std::string sDescription;
    OrderStatus::enumOrderStatus eOrderStatus;
    OrderType::enumOrderType eOrderType;
    OrderSide::enumOrderSide eOrderSide;
    double dblPrice1; // for limit
    double dblPrice2; // for stop
    double dblSignalPrice;  // mark at which algorithm requested order
    boost::uint32_t nOrderQuantity;
    boost::uint32_t nQuantityRemaining;
    boost::uint32_t nQuantityFilled;
    boost::uint32_t nQuantityPaired;
    double dblAverageFillPrice;  // excludes commission
    double dblCommission;
    ptime dtOrderCreated;
    ptime dtOrderSubmitted;
    ptime dtOrderClosed;

    TableRowDef( void ) // default constructor
      : idOrder( 0 ), idPosition( 0 ), 
        eOrderStatus( OrderStatus::Created ), eOrderType( OrderType::Unknown ), eOrderSide( OrderSide::Unknown ), 
        dblPrice1( 0.0 ), dblPrice2( 0.0 ), dblSignalPrice( 0.0 ), 
        nOrderQuantity( 0 ), nQuantityRemaining( 0 ), nQuantityFilled( 0 ), nQuantityPaired( 0 ),
        dblAverageFillPrice( 0.0 ), dblCommission( 0.0 ) {};
    TableRowDef( // market order
      idPosition_t idPosition_, idInstrument_t idInstrument_, OrderType::enumOrderType eOrderType_, OrderSide::enumOrderSide eOrderSide_,
      boost::uint32_t nOrderQuantity_, ptime dtOrderSubmitted_ )
      : idOrder( 0 ), idPosition( idPosition_ ), idInstrument( idInstrument_ ), eOrderStatus( OrderStatus::Created ), 
        eOrderType( eOrderType_ ), eOrderSide( eOrderSide_ ), dblPrice1( 0.0 ), dblPrice2( 0.0 ), dblSignalPrice( 0.0 ),
        nOrderQuantity( nOrderQuantity_ ), nQuantityRemaining( nOrderQuantity_ ), nQuantityFilled( 0 ), nQuantityPaired( 0 ),
        dblAverageFillPrice( 0 ), dblCommission( 0 ), dtOrderCreated( boost::date_time::not_a_date_time ),
        dtOrderSubmitted( dtOrderSubmitted_ ), dtOrderClosed( boost::date_time::not_a_date_time ) {};
    TableRowDef( // limit or stop
      idPosition_t idPosition_, idInstrument_t idInstrument_, OrderType::enumOrderType eOrderType_, OrderSide::enumOrderSide eOrderSide_,
      boost::uint32_t nOrderQuantity_, double dblPrice1_, ptime dtOrderSubmitted_ )
      : idOrder( 0 ), idPosition( idPosition_ ), idInstrument( idInstrument_ ), eOrderStatus( OrderStatus::Created ), 
        eOrderType( eOrderType_ ), eOrderSide( eOrderSide_ ), dblPrice1( dblPrice1_ ), dblPrice2( 0.0 ), dblSignalPrice( 0.0 ),
        nOrderQuantity( nOrderQuantity_ ), nQuantityRemaining( nOrderQuantity_ ), nQuantityFilled( 0 ), nQuantityPaired( 0 ),
        dblAverageFillPrice( 0 ), dblCommission( 0 ), dtOrderCreated( boost::date_time::not_a_date_time ),
        dtOrderSubmitted( dtOrderSubmitted_ ), dtOrderClosed( boost::date_time::not_a_date_time ) {};
    TableRowDef( // limit and stop
      idPosition_t idPosition_, idInstrument_t idInstrument_, OrderType::enumOrderType eOrderType_, OrderSide::enumOrderSide eOrderSide_,
      boost::uint32_t nOrderQuantity_, double dblPrice1_, double dblPrice2_, ptime dtOrderSubmitted_ )
      : idOrder( 0 ), idPosition( idPosition_ ), idInstrument( idInstrument_ ), eOrderStatus( OrderStatus::Created ), 
        eOrderType( eOrderType_ ), eOrderSide( eOrderSide_ ), dblPrice1( dblPrice1_ ), dblPrice2( dblPrice2_ ), dblSignalPrice( 0.0 ),
        nOrderQuantity( nOrderQuantity_ ), nQuantityRemaining( nOrderQuantity_ ), nQuantityFilled( 0 ), nQuantityPaired( 0 ),
        dblAverageFillPrice( 0 ), dblCommission( 0 ), dtOrderCreated( boost::date_time::not_a_date_time ),
        dtOrderSubmitted( dtOrderSubmitted_ ), dtOrderClosed( boost::date_time::not_a_date_time ) {};
    TableRowDef( // complete row provided
      idOrder_t idOrder_, idPosition_t idPosition_, idInstrument_t idInstrument_, std::string sDescription_, 
      OrderStatus::enumOrderStatus eOrderStatus_, OrderType::enumOrderType eOrderType_, OrderSide::enumOrderSide eOrderSide_, 
      double dblPrice1_, double dblPrice2_, double dblSignalPrice_,
      boost::uint32_t nOrderQuantity_, boost::uint32_t nQuantityRemaining_, boost::uint32_t nQuantityFilled_, 
      double dblAverageFillPrice_, double dblCommission_, 
      ptime dtOrderCreated_, ptime dtOrderSubmitted_, ptime dtOrderClosed_ )
      : idOrder( idOrder_ ), idPosition( idPosition_ ), idInstrument( idInstrument_ ), sDescription( sDescription_ ),
        eOrderStatus( eOrderStatus_ ), eOrderType( eOrderType_ ), eOrderSide( eOrderSide_ ),
        dblPrice1( dblPrice1_ ), dblPrice2( dblPrice2_ ), dblSignalPrice( dblSignalPrice_ ),
        nOrderQuantity( nOrderQuantity_ ), nQuantityRemaining( nQuantityRemaining_ ), nQuantityFilled( nQuantityFilled_ ), nQuantityPaired( 0 ),
        dblAverageFillPrice( dblAverageFillPrice_ ), dblCommission( dblCommission_ ),
        dtOrderCreated( dtOrderCreated_ ), dtOrderSubmitted( dtOrderSubmitted_ ), dtOrderClosed( dtOrderClosed_ ) {};
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "orderid" );
      ou::db::Constraint( a, "positionid", tablenames::sPosition, "positionid" );
      ou::db::Constraint( a, "instrumentid", tablenames::sInstrument, "instrumentid" );
    }
  };

  Order(  // market 
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    boost::uint32_t nOrderQuantity,
    idPosition_t idPosition = 0,
    ptime dtOrderSubmitted = not_a_date_time
    );
  Order(  // limit or stop
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    boost::uint32_t nOrderQuantity,
    double dblPrice1,  
    idPosition_t idPosition = 0,
    ptime dtOrderSubmitted = not_a_date_time
    );
  Order(  // limit and stop
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    boost::uint32_t nOrderQuantity,
    double dblPrice1,  
    double dblPrice2,
    idPosition_t idPosition = 0,
    ptime dtOrderSubmitted = not_a_date_time
    );
  Order( const TableRowDef& row, pInstrument_t& pInstrument );
  ~Order(void);

  void SetOutsideRTH( bool bOutsideRTH ) { m_bOutsideRTH = bOutsideRTH; };  // not persisted yet
  bool GetOutsideRTH( void ) const { return m_bOutsideRTH; };
  void SetInstrument( CInstrument::pInstrument_cref pInstrument ) {  // used only when class created from database
    if ( NULL != m_pInstrument.get() ) {
      throw std::runtime_error( "Corder::SetInstrument: instrument already assigned" );
    }
    if ( m_idInstrument != pInstrument->GetInstrumentName() ) {
      throw std::runtime_error( "Order::SetInstrument: instrument name does not match expected" );
    }
    m_pInstrument = pInstrument;
  }
  CInstrument::pInstrument_t GetInstrument( void ) const { 
    if ( NULL == m_pInstrument.get() ) {
      throw std::runtime_error( "Order::GetInstrument:  no instrument defined" );
    }
    return m_pInstrument; 
  };
  const char *GetOrderSideName( void ) const { return OrderSide::Name[ m_row.eOrderSide ]; };
  boost::uint32_t GetQuantity( void ) const { return m_row.nOrderQuantity; };
  OrderType::enumOrderType GetOrderType( void ) const { return m_row.eOrderType; };
  OrderSide::enumOrderSide GetOrderSide( void ) const { return m_row.eOrderSide; };
  double GetPrice1( void ) const { return m_row.dblPrice1; };  // need to validate this on creation
  double GetPrice2( void ) const { return m_row.dblPrice2; };
  double GetAverageFillPrice( void ) const { return m_row.dblAverageFillPrice; };
  idOrder_t GetOrderId( void ) const { assert( 0 != m_row.idOrder ); return m_row.idOrder; };
  boost::uint32_t GetNextExecutionId( void ) { return ++m_nNextExecutionId; };
  void SetSendingToProvider( void );
  OrderStatus::enumOrderStatus ReportExecution( const CExecution &exec ); // called from COrderManager
  void SetCommission( double dblCommission );
  double GetCommission( void ) const{ return m_row.dblCommission; };
  void ActOnError( OrderErrors::enumOrderErrors eError );
  boost::uint32_t GetQuanRemaining( void ) const { return m_row.nQuantityRemaining; };
  boost::uint32_t GetQuanOrdered( void ) const { return m_row.nOrderQuantity; };
  boost::uint32_t GetQuanFilled( void ) const { return m_row.nQuantityFilled; };
  void SetSignalPrice( double dblSignalPrice ) { m_row.dblSignalPrice = dblSignalPrice; };
  double GetSignalPrice( void ) const { return m_row.dblSignalPrice; };
  const ptime &GetDateTimeOrderSubmitted( void ) const { 
    assert( not_a_date_time != m_row.dtOrderSubmitted ); // is this a valid test?
    return m_row.dtOrderSubmitted; 
  };
  const ptime &GetDateTimeOrderFilled( void ) const { 
    assert( not_a_date_time != m_row.dtOrderClosed ); // is this a valid test?
    return m_row.dtOrderClosed; 
  };
  void MarkAsCancelled( void );  // called from COrderManager

  ou::Delegate<const std::pair<const Order&, const CExecution&>& > OnExecution;
  ou::Delegate<const Order&> OnOrderCancelled;
  ou::Delegate<const Order&> OnPartialFill; // on intermediate fills only
  ou::Delegate<const Order&> OnOrderFilled; // on final fill
  ou::Delegate<const Order&> OnCommission;

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:

  CInstrument::pInstrument_t m_pInstrument;
  idInstrument_t m_idInstrument;  // used temporarily in order to get instrument_t in place

  bool m_bOutsideRTH;

  boost::uint32_t m_nNextExecutionId;  // when is this set?

  // statistics and status
  double m_dblPriceXQuantity; // used for calculating average price

  void ConstructOrder( void );
  void SetOrderId( idOrder_t );  // used by OrderManager

private:

  TableRowDef m_row;

  Order(void);  // no default constructor

};

} // namespace tf
} // namespace ou

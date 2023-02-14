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
#include <memory>
#include <stdexcept>

#include <OUCommon/Delegate.h>

#include "KeyTypes.h"
#include "Execution.h"
#include "Instrument.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class OrderManager;

class Order {
  friend class OrderManager;
public:

  using idOrder_t = keytypes::idOrder_t;
  using idPosition_t = keytypes::idPosition_t;
  using pInstrument_t = Instrument::pInstrument_t;
  using idInstrument_t = keytypes::idInstrument_t;
  using pOrder_t = std::shared_ptr<Order>;
  using pOrder_ref = const pOrder_t&;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "orderid", idOrder );
      ou::db::Field( a, "positionid", idPosition );
      ou::db::Field( a, "instrumentid", idInstrument );
      ou::db::Field( a, "reference", sReference );
      ou::db::Field( a, "description", sDescription );
      ou::db::Field( a, "timeinforce", eTimeInForce );
      ou::db::Field( a, "goodtilldate", dtGoodTillDate );
      ou::db::Field( a, "goodaftertime", dtGoodAfterTime );
      ou::db::Field( a, "parentid", idParent );
      ou::db::Field( a, "transmit", bTransmit );
      ou::db::Field( a, "outsiderth", bOutsideRTH );
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
    std::string sReference; // typically, unique order id from broker
    std::string sDescription;
    ETimeInForce eTimeInForce;
    ptime dtGoodTillDate;
    ptime dtGoodAfterTime;
    idOrder_t idParent;
    bool bTransmit;
    bool bOutsideRTH;
    OrderStatus::EOrderStatus eOrderStatus;
    OrderType::EOrderType eOrderType;
    OrderSide::EOrderSide eOrderSide;
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

    TableRowDef() // default constructor
      : idOrder( 0 ), idPosition( 0 ),
        eTimeInForce( ou::tf::ETimeInForce::Day ),
        dtGoodTillDate( boost::date_time::not_a_date_time  ), dtGoodAfterTime( boost::date_time::not_a_date_time  ),
        idParent( 0 ), bTransmit( true ), bOutsideRTH( false ),
        eOrderStatus( OrderStatus::Created ), eOrderType( OrderType::Unknown ), eOrderSide( OrderSide::Unknown ),
        dblPrice1( 0.0 ), dblPrice2( 0.0 ), dblSignalPrice( 0.0 ),
        nOrderQuantity( 0 ), nQuantityRemaining( 0 ), nQuantityFilled( 0 ), nQuantityPaired( 0 ),
        dblAverageFillPrice( 0.0 ), dblCommission( 0.0 ) {};
    TableRowDef( // market order
      idPosition_t idPosition_, idInstrument_t idInstrument_, OrderType::EOrderType eOrderType_, OrderSide::EOrderSide eOrderSide_,
      boost::uint32_t nOrderQuantity_, ptime dtOrderSubmitted_ )
      : idOrder( 0 ), idPosition( idPosition_ ), idInstrument( idInstrument_ ),
        eTimeInForce( ou::tf::ETimeInForce::Day ),
        dtGoodTillDate( boost::date_time::not_a_date_time  ), dtGoodAfterTime( boost::date_time::not_a_date_time  ),
        idParent( 0 ), bTransmit( true ), bOutsideRTH( false ),
        eOrderStatus( OrderStatus::Created ),
        eOrderType( eOrderType_ ), eOrderSide( eOrderSide_ ), dblPrice1( 0.0 ), dblPrice2( 0.0 ), dblSignalPrice( 0.0 ),
        nOrderQuantity( nOrderQuantity_ ), nQuantityRemaining( nOrderQuantity_ ), nQuantityFilled( 0 ), nQuantityPaired( 0 ),
        dblAverageFillPrice( 0 ), dblCommission( 0 ), dtOrderCreated( boost::date_time::not_a_date_time ),
        dtOrderSubmitted( dtOrderSubmitted_ ), dtOrderClosed( boost::date_time::not_a_date_time ) {};
    TableRowDef( // limit or stop
      idPosition_t idPosition_, idInstrument_t idInstrument_, OrderType::EOrderType eOrderType_, OrderSide::EOrderSide eOrderSide_,
      boost::uint32_t nOrderQuantity_, double dblPrice1_, ptime dtOrderSubmitted_ )
      : idOrder( 0 ), idPosition( idPosition_ ), idInstrument( idInstrument_ ),
        eTimeInForce( ou::tf::ETimeInForce::Day ),
        dtGoodTillDate( boost::date_time::not_a_date_time  ), dtGoodAfterTime( boost::date_time::not_a_date_time  ),
        idParent( 0 ), bTransmit( true ), bOutsideRTH( false ),
        eOrderStatus( OrderStatus::Created ),
        eOrderType( eOrderType_ ), eOrderSide( eOrderSide_ ), dblPrice1( dblPrice1_ ), dblPrice2( 0.0 ), dblSignalPrice( 0.0 ),
        nOrderQuantity( nOrderQuantity_ ), nQuantityRemaining( nOrderQuantity_ ), nQuantityFilled( 0 ), nQuantityPaired( 0 ),
        dblAverageFillPrice( 0 ), dblCommission( 0 ), dtOrderCreated( boost::date_time::not_a_date_time ),
        dtOrderSubmitted( dtOrderSubmitted_ ), dtOrderClosed( boost::date_time::not_a_date_time ) {};
    TableRowDef( // limit and stop
      idPosition_t idPosition_, idInstrument_t idInstrument_, OrderType::EOrderType eOrderType_, OrderSide::EOrderSide eOrderSide_,
      boost::uint32_t nOrderQuantity_, double dblPrice1_, double dblPrice2_, ptime dtOrderSubmitted_ )
      : idOrder( 0 ), idPosition( idPosition_ ), idInstrument( idInstrument_ ),
        eTimeInForce( ou::tf::ETimeInForce::Day ),
        dtGoodTillDate( boost::date_time::not_a_date_time  ), dtGoodAfterTime( boost::date_time::not_a_date_time  ),
        idParent( 0 ), bTransmit( true ), bOutsideRTH( false ),
        eOrderStatus( OrderStatus::Created ),
        eOrderType( eOrderType_ ), eOrderSide( eOrderSide_ ), dblPrice1( dblPrice1_ ), dblPrice2( dblPrice2_ ), dblSignalPrice( 0.0 ),
        nOrderQuantity( nOrderQuantity_ ), nQuantityRemaining( nOrderQuantity_ ), nQuantityFilled( 0 ), nQuantityPaired( 0 ),
        dblAverageFillPrice( 0 ), dblCommission( 0 ), dtOrderCreated( boost::date_time::not_a_date_time ),
        dtOrderSubmitted( dtOrderSubmitted_ ), dtOrderClosed( boost::date_time::not_a_date_time ) {};
    TableRowDef( // complete row provided
      idOrder_t idOrder_, idPosition_t idPosition_, idInstrument_t idInstrument_, std::string sDescription_,
      OrderStatus::EOrderStatus eOrderStatus_, OrderType::EOrderType eOrderType_, OrderSide::EOrderSide eOrderSide_,
      double dblPrice1_, double dblPrice2_, double dblSignalPrice_,
      boost::uint32_t nOrderQuantity_, boost::uint32_t nQuantityRemaining_, boost::uint32_t nQuantityFilled_,
      double dblAverageFillPrice_, double dblCommission_,
      ptime dtOrderCreated_, ptime dtOrderSubmitted_, ptime dtOrderClosed_ )
      : idOrder( idOrder_ ), idPosition( idPosition_ ), idInstrument( idInstrument_ ), sDescription( sDescription_ ),
        eOrderStatus( eOrderStatus_ ), eOrderType( eOrderType_ ),
        eTimeInForce( ou::tf::ETimeInForce::Day ),
        dtGoodTillDate( boost::date_time::not_a_date_time  ), dtGoodAfterTime( boost::date_time::not_a_date_time  ),
        idParent( 0 ), bTransmit( true ), bOutsideRTH( false ),
        eOrderSide( eOrderSide_ ),
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

  Order() = delete;
  Order(  // market
    Instrument::pInstrument_cref instrument,
    OrderType::EOrderType eOrderType,
    OrderSide::EOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity,
    idPosition_t idPosition = 0,
    ptime dtOrderSubmitted = not_a_date_time
    );
  Order(  // limit or stop
    Instrument::pInstrument_cref instrument,
    OrderType::EOrderType eOrderType,
    OrderSide::EOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity,
    double dblPrice1,
    idPosition_t idPosition = 0,
    ptime dtOrderSubmitted = not_a_date_time
    );
  Order(  // limit and stop
    Instrument::pInstrument_cref instrument,
    OrderType::EOrderType eOrderType,
    OrderSide::EOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity,
    double dblPrice1,
    double dblPrice2,
    idPosition_t idPosition = 0,
    ptime dtOrderSubmitted = not_a_date_time
    );
  Order( const TableRowDef& row, pInstrument_t& pInstrument );
  ~Order();

  void SetParentOrderId( idOrder_t idParent ) { m_row.idParent = idParent; }
  idOrder_t GetParentOrderId() const { return m_row.idParent; }

  void SetReference( const std::string& sReference ) { m_row.sReference = sReference; }
  const std::string& GetReference() const { return m_row.sReference; }

  void SetOutsideRTH( bool bOutsideRTH ) { m_row.bOutsideRTH = bOutsideRTH; }; // need to persist to db
  bool GetOutsideRTH() const { return m_row.bOutsideRTH; };

  void SetTimeInForce( ou::tf::ETimeInForce tif ) { m_row.eTimeInForce = tif; } // need to persist to db
  ou::tf::ETimeInForce GetTimeInForce() const { return m_row.eTimeInForce; }

  void SetTransmit( bool bTransmit ) { m_row.bTransmit = bTransmit; } // need to persist to db
  bool GetTransmit() const { return m_row.bTransmit; }

  void SetGoodTillDate( ptime dtGTD ) { m_row.dtGoodTillDate = dtGTD; } // need to persist to db
  ptime GetGoodTillDate() const { return m_row.dtGoodTillDate; }

  void SetGoodAfterTime( ptime dtGAT ) { m_row.dtGoodAfterTime = dtGAT; } // need to persist to db
  ptime GetGoodAfterTime() const { return m_row.dtGoodAfterTime; }

  Instrument::pInstrument_t GetInstrument() const {
    if ( NULL == m_pInstrument.get() ) {
      throw std::runtime_error( "Order::GetInstrument:  no instrument defined" );
    }
    return m_pInstrument;
  };

  const char *GetOrderSideName() const { return OrderSide::Name[ m_row.eOrderSide ]; };
  OrderType::EOrderType GetOrderType() const { return m_row.eOrderType; };
  OrderSide::EOrderSide GetOrderSide() const { return m_row.eOrderSide; };
  double GetPrice1() const { return m_row.dblPrice1; };  // need to validate this on creation
  void SetPrice1( double dblPrice ) { m_row.dblPrice1 = dblPrice; } // prepares for UpdatePrice
  double GetPrice2() const { return m_row.dblPrice2; };
  void SetPrice2( double dblPrice ) { m_row.dblPrice2 = dblPrice; } // prepares for UpdatePrice
  double GetAverageFillPrice() const { return m_row.dblAverageFillPrice; };
  idOrder_t GetOrderId() const { assert( 0 != m_row.idOrder ); return m_row.idOrder; };
  boost::uint32_t GetNextExecutionId() { return ++m_nNextExecutionId; };
  void SetSendingToProvider();
  OrderStatus::EOrderStatus ReportExecution( const Execution &exec ); // called from OrderManager
  void SetCommission( double dblCommission );
  double GetCommission() const{ return m_row.dblCommission; };
  void ActOnError( OrderError::EOrderError eError );
  boost::uint32_t GetQuanRemaining() const { return m_row.nQuantityRemaining; };
  boost::uint32_t GetQuanOrdered() const { return m_row.nOrderQuantity; };
  boost::uint32_t GetQuanFilled() const { return m_row.nQuantityFilled; };
  void SetSignalPrice( double dblSignalPrice ) { m_row.dblSignalPrice = dblSignalPrice; };
  double GetSignalPrice() const { return m_row.dblSignalPrice; };
  void SetDescription( const std::string& sDescription ) { m_row.sDescription = sDescription; }
  const std::string& GetDescription() const { return m_row.sDescription; }

  const ptime &GetDateTimeOrderCreated() const {
    assert( not_a_date_time != m_row.dtOrderCreated ); // is this a valid test?
    return m_row.dtOrderCreated;
  };
  const ptime &GetDateTimeOrderSubmitted() const {
    assert( not_a_date_time != m_row.dtOrderSubmitted ); // is this a valid test?
    return m_row.dtOrderSubmitted;
  };
  const ptime &GetDateTimeOrderFilled() const {
    assert( not_a_date_time != m_row.dtOrderClosed ); // is this a valid test?
    return m_row.dtOrderClosed;
  };

  double GetIncrementalCommission() const { return m_dblIncrementalCommission; };
  void MarkAsCancelled();  // called from OrderManager

  ou::Delegate<const std::pair<const Order&, const Execution&>& > OnExecution;
  ou::Delegate<const Order&> OnOrderCancelled;
  ou::Delegate<const Order&> OnPartialFill; // on intermediate fills only
  ou::Delegate<const Order&> OnOrderFilled; // on final fill
  ou::Delegate<const Order&> OnCommission;

  OrderStatus::EOrderStatus OrderStatus() const { return m_row.eOrderStatus; }

  const TableRowDef& GetRow() const { return m_row; };

protected:

  Instrument::pInstrument_t m_pInstrument;

  boost::uint32_t m_nNextExecutionId;  // when is this used?

  // statistics and status
  double m_dblPriceXQuantity; // used for calculating average price

  void ConstructOrder();
  void SetOrderId( idOrder_t );  // use OrderManager to construct orders

private:

  TableRowDef m_row;

  double m_dblIncrementalCommission; // Something for the Position Manager as commission is updated for the order.

};

} // namespace tf
} // namespace ou

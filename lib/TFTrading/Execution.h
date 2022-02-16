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

#include <memory>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <OUSQL/Functions.h>

#include "KeyTypes.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class Execution {
public:

  using idExecution_t = keytypes::idExecution_t;  // used for database updates, need to persist like orderid
  using idOrder_t = keytypes::idOrder_t;
  using pExecution_t = std::shared_ptr<Execution>;
  using pExecution_ref = const pExecution_t&;

  struct TableRowDefNoKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "orderid", idOrder );
      ou::db::Field( a, "quantity", nQuantity );
      ou::db::Field( a, "price", dblPrice );
      ou::db::Field( a, "orderside", eOrderSide );
      ou::db::Field( a, "executiontimestamp", dtExecutionTimeStamp );
      ou::db::Field( a, "exchange", sExchange );
      ou::db::Field( a, "exchangeexecutionid", sExchangeExecutionId );
    }
    //  "create index if not exists idx_executions_orderid on executions( orderid );",

    idOrder_t idOrder;
    boost::uint32_t nQuantity;  // quantity executed
    double dblPrice;  // execution price
    OrderSide::enumOrderSide eOrderSide;
    ptime dtExecutionTimeStamp;
    std::string sExchange;
    std::string sExchangeExecutionId;  // unique execution id supplied by provider

    TableRowDefNoKey() : idOrder( 0 ), nQuantity( 0 ), dblPrice( 0.0 ),
      eOrderSide( OrderSide::Unknown ) {};
    TableRowDefNoKey( idOrder_t idOrder_,
      boost::uint32_t nQuantity_, double dblPrice_, OrderSide::enumOrderSide eOrderSide_,
      std::string sExchange_, std::string sExchangeExecutionId_ )
      : idOrder( idOrder_ ), nQuantity( nQuantity_ ),
        dblPrice( dblPrice_ ), eOrderSide( eOrderSide_ ),
        sExchange( sExchange_ ), sExchangeExecutionId( sExchangeExecutionId_ ) {};
    TableRowDefNoKey( /* idOrder_t idOrder_, */
      boost::uint32_t nQuantity_, double dblPrice_, OrderSide::enumOrderSide eOrderSide_,
      std::string sExchange_, std::string sExchangeExecutionId_ )
      : idOrder( 0 ), nQuantity( nQuantity_ ),  // idOrder from owner
        dblPrice( dblPrice_ ), eOrderSide( eOrderSide_ ),
        sExchange( sExchange_ ), sExchangeExecutionId( sExchangeExecutionId_ ) {};
  };

  struct TableRowDef: TableRowDefNoKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "executionid", idExecution );
      TableRowDefNoKey::Fields( a );
    }
    idExecution_t idExecution;

    TableRowDef() : idExecution( 0 ), TableRowDefNoKey() {};
    TableRowDef( idExecution_t idExecution_, idOrder_t idOrder_,
      boost::uint32_t nQuantity_, double dblPrice_, OrderSide::enumOrderSide eOrderSide_,
      std::string sExchange_, std::string sExchangeExecutionId_ )
      : idExecution( idExecution_ ), TableRowDefNoKey( idOrder_, nQuantity_,
        dblPrice_, eOrderSide_, sExchange_, sExchangeExecutionId_ ) {};
    TableRowDef( /* idExecution_t idExecution_, idOrder_t idOrder_, */
      boost::uint32_t nQuantity_, double dblPrice_, OrderSide::enumOrderSide eOrderSide_,
      std::string sExchange_, std::string sExchangeExecutionId_ )
      : idExecution( 0 ), TableRowDefNoKey( nQuantity_,  // executionid from db, idOrder from owner
        dblPrice_, eOrderSide_, sExchange_, sExchangeExecutionId_ ) {};
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "executionid" );
      ou::db::Constraint( a, "orderid", tablenames::sOrder, "orderid" );
    }
  };

  Execution( const TableRowDef& row ): m_row( row ) {};
  Execution( // when relating to database
    idExecution_t idExecution, idOrder_t nOrderId,
    double dblPrice, boost::uint32_t nQuantity, OrderSide::enumOrderSide eOrderSide,
    const std::string& sExchange, const std::string& sExchangeExecutionId );
  Execution( // when supplied by provider
    double dblPrice, boost::uint32_t nQuantity, OrderSide::enumOrderSide eOrderSide,
    const std::string& sExchange, const std::string& sExchangeExecutionId );
  ~Execution();

  double GetPrice() const { return m_row.dblPrice; };
  boost::uint32_t GetSize() const { return m_row.nQuantity; };
  OrderSide::enumOrderSide GetOrderSide() const { return m_row.eOrderSide; };
  const std::string& GetExchange() const { return m_row.sExchange; };
  const std::string& GetExchangeExecutionId() const { return m_row.sExchangeExecutionId; };
  ptime GetTimeStamp() const { return m_row.dtExecutionTimeStamp; };
  void SetOrderId( idOrder_t idOrder ) { m_row.idOrder = idOrder; };

  const TableRowDef& GetRow() const { return m_row; };

protected:

private:

  TableRowDef m_row;

};

} // namespace tf
} // namespace ou

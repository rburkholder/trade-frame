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

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "TradingEnumerations.h"
#include "KeyTypes.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CExecution {
public:

  typedef keytypes::idExecution_t idExecution_t;  // used for database updates, need to persist like orderid
  typedef keytypes::idOrder_t idOrder_t;
  typedef boost::shared_ptr<CExecution> pExecution_t;
  typedef const pExecution_t& pExecution_ref;

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

    TableRowDefNoKey( void ) : idOrder( 0 ), nQuantity( 0 ), dblPrice( 0.0 ),
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

    TableRowDef( void ) : idExecution( 0 ), TableRowDefNoKey() {};
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

  CExecution( const TableRowDef& row ): m_row( row ) {};
  CExecution( // when relating to database
    idExecution_t idExecution, idOrder_t nOrderId,
    double dblPrice, boost::uint32_t nQuantity, OrderSide::enumOrderSide eOrderSide,
    const std::string& sExchange, const std::string& sExchangeExecutionId );
  CExecution( // when supplied by provider
    double dblPrice, boost::uint32_t nQuantity, OrderSide::enumOrderSide eOrderSide,
    const std::string& sExchange, const std::string& sExchangeExecutionId );
  ~CExecution(void);

  double GetPrice( void ) const { return m_row.dblPrice; };
  boost::uint32_t GetSize( void ) const { return m_row.nQuantity; };
  OrderSide::enumOrderSide GetOrderSide( void ) const { return m_row.eOrderSide; };
  const std::string& GetExchange( void ) const { return m_row.sExchange; };
  const std::string& GetExchangeExecutionId( void ) const { return m_row.sExchangeExecutionId; };
  ptime GetTimeStamp( void ) const { return m_row.dtExecutionTimeStamp; };

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:

private:

  TableRowDef m_row;

};

} // namespace tf
} // namespace ou

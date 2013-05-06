/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

// started 2013-04-06

#pragma once

#include <boost/shared_ptr.hpp>

#include <OUCommon/Decimal.h>

#include "TradingEnumerations.h"

#include "KeyTypes.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
  
// updated by cash transactions
// updated by portfolio changes (positions opening and closing)
// how to deal with margin adjustments?

// multiple records per account. one record per trading currency. 
// should match the balance of the user's brokerage account as each transaction is processed

class CashAccount { 
public:

  typedef keytypes::idAccount_t idAccount_t;
  //typedef keytypes::idCashAccount_t idCashAccount_t;  // currency identification
  typedef keytypes::idCurrency_t idCurrency_t;
  typedef dec::decimal6 money_t;
  typedef boost::shared_ptr<CashAccount> pCashAccount_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "accountid", idAccount );
      ou::db::Field( a, "currencyid", idCurrency );  // currency
      ou::db::Field( a, "balance", mnyBalance );  // used for period end totals
    }

    idAccount_t idAccount;
    idCurrency_t idCurrency;  // currency, string value
    money_t mnyBalance;

    TableRowDef( void ): idCurrency( Currency::Name[ Currency::USD ] ), mnyBalance( 0 ) {};
    TableRowDef ( const TableRowDef& row ) 
      : idAccount( row.idAccount ), idCurrency( row.idCurrency ), mnyBalance( row.mnyBalance ) {};
    TableRowDef( const idAccount_t idAccount_, idCurrency_t idCurrency_, money_t mnyBalance_ )
      : idAccount( idAccount_ ), idCurrency( idCurrency_ ), mnyBalance( mnyBalance_ ) {};
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "accountid" );  // need unique account id plus currency type
      ou::db::Key( a, "currencyid" ); 
      ou::db::Constraint( a, "accountid", tablenames::sAccount, "accountid" );
    }
  };

  CashAccount(   // db version
    const idAccount_t& idAccount, 
    idCurrency_t idCurrency, 
    money_t mnyBalance = money_t( 0 )
     );
  CashAccount(  // non db version
    idCurrency_t idCurrency, money_t mnyBalance = money_t( 0 ) );
  CashAccount( const TableRowDef& row ) : m_row( row ) {};
  ~CashAccount(void);

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:
private:
  TableRowDef m_row;
};

} // namespace tf
} // namespace ou

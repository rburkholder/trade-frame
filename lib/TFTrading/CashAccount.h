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

  typedef keytypes::idAccountOwner_t idAccount_t;
  typedef keytypes::idCashAccount_t idCashAccount_t;
  //typedef keytypes::idCurrency_t idCurrency_t;
  typedef Currency::type currency_t;
  typedef dec::decimal6 money_t;
  typedef boost::shared_ptr<CashAccount> pCashAccount_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& A ) {
      ou::db::Field( a, "cashaccountid", idCashAccount );
      ou::db::Field( a, "accountid", idAccount );
      ou::db::Field( a, "balance", mnyBalance );
      ou::db::Field( a, "currency", sCurrency );
    }

    idCashAccount_t idCashAccount;
    idAccount_t idAccount;
    money_t mnyBalance;
    currency_t sCurrency;

    TableRowDef( void ): idCashAccount( 0 ), mnyBalance( 0 ), sCurrency( Currency::Name[ Currency::USD ] ) {};
    TableRowDef ( const TableRowDef& row ) 
      : idCashAccount( row.idCashAccount ), idAccount( row.idAccount ), mnyBalance( row.mnyBalance ), sCurrency( row.sCurrency ) {};
    TableRowDef( idCashAccount_t idCashAccount_, const idAccount_t idAccount_, money_t mnyBalance_, currency_t sCurrency_ )
      : idCashAccount( idCashAccount_ ), idAccount( idAccount_ ), mnyBalance( mnyBalance_ ), sCurrency( sCurrency_ ) {};
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "cashaccountid" );
      ou::db::Constraint( a, "accountid", tablenames::sAccount, "accountid" );
    }
  };

  CashAccount(   // db version
    idCashAccount_t idCashAccount, 
    const idAccount_t& idAccount, 
    money_t mnyBalance = money_t( 0 ),
    const currency_t& sCurrency = Currency::Name[ Currency::USD ]
     );
  CashAccount(  // non db version
    money_t mnyBalance = money_t( 0 ), 
    const currency_t& sCurrency = Currency::Name[ Currency::USD ] );
  CashAccount( const TableRowDef& row ) : m_row( row ) {};
  ~CashAccount(void);

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:
private:
  TableRowDef m_row;
};

} // namespace tf
} // namespace ou

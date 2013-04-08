/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

// deposits
// withdrawals
// dividends
// adjustments
// interest credit/debit
// market fees
// commissions
// postion entry/exit
//

// currency is implied as updates here affect the associated summary record in the CashAccount record

class CashTransaction {
public:

  typedef keytypes::idCashTransaction_t idCashTransaction_t;
  typedef keytypes::idCashAccount_t idCashAccount_t;
  typedef dec::decimal6 money_t;
  typedef boost::shared_ptr<CashTransaction> pCashTransaction_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "cashtransactionid", idCashTransaction );
      ou::db::Field( a, "cashaccountid", idCashAccount );
      ou::db::Field( a, "credit", mnyCredit );
      ou::db::Field( a, "debit", mnyDebit );
      ou::db::Field( a, "description", sDescription );
    }

    idCashTransaction_t idCashTransaction;
    idCashAccount_t idCashAccount;
    money_t mnyCredit;
    money_t mnyDebit;
    std::string sDescription;

    TableRowDef( void ): idCashTransaction( 0 ), idCashAccount( 0 ), 
      mnyCredit( money_t( 0 ) ), mnyDebit( money_t( 0 ) ) {};
    TableRowDef( const TableRowDef& row ) 
      : idCashTransaction( row.idCashTransaction ), idCashAccount( row.idCashAccount ),
        mnyCredit( row.mnyCredit ), mnyDebit( row.mnyDebit ),
        sDescription( row.sDescription ) {};
    TableRowDef( idCashTransaction_t idCashTransaction_, idCashAccount_t idCashAccount_, 
      const money_t& mnyCredit_, const money_t& mnyDebit_, const std::string& sDescription_ ) 
      : idCashTransaction( idCashTransaction_ ), idCashAccount( idCashAccount_ ),
        mnyCredit( mnyCredit_ ), mnyDebit( mnyDebit_ ), sDescription( sDescription_ ) {};
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "cashtransactionid" );
      ou::db::Constraint( a, "cashaccountid", tablenames::sCashTransaction, "cashaccountid" );
    }
  };

  CashTransaction( // db version
    idCashTransaction_t idCashTransaction, idCashAccount_t idCashAccount,
    const money_t& mnyCredit, const money_t& mnyDebit,
    const std::string& sDescription 
    );
  CashTransaction( // non db version
    idCashTransaction_t idCashTransaction,
    const money_t& mnyCredit, const money_t& mnyDebit,
    const std::string& sDescription 
    );
  ~CashTransaction(void) {};

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:
private:
  TableRowDef m_row;
};

} // namespace tf
} // namespace ou

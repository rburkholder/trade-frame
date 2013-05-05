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
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

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

  typedef keytypes::idAccount_t idAccount_t;
  typedef keytypes::idCurrency_t idCurrency_t;

  typedef keytypes::idCashTransaction_t idCashTransaction_t;
  typedef dec::decimal6 money_t;
  typedef boost::shared_ptr<CashTransaction> pCashTransaction_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "cashtransactionid", idCashTransaction );
      ou::db::Field( a, "accountid", idAccount );
      ou::db::Field( a, "currencyid", idCurrency );
      ou::db::Field( a, "timestamp", dtTimeStamp );
      ou::db::Field( a, "credit", mnyCredit );
      ou::db::Field( a, "debit", mnyDebit );
      ou::db::Field( a, "code", sCode );
      ou::db::Field( a, "description", sDescription );
    }

    idCashTransaction_t idCashTransaction;
    idAccount_t idAccount;
    idCurrency_t idCurrency;
    ptime dtTimeStamp;
    money_t mnyCredit;
    money_t mnyDebit;
    std::string sCode;
    std::string sDescription;

    TableRowDef( void ): idCashTransaction( 0 ), idCurrency( Currency::Name[ Currency::USD ] ), 
      mnyCredit( money_t( 0 ) ), mnyDebit( money_t( 0 ) ) {};
    TableRowDef( const TableRowDef& row ) 
      : idCashTransaction( row.idCashTransaction ), idAccount( row.idAccount ), idCurrency( row.idCurrency ),
        dtTimeStamp( row.dtTimeStamp ), 
        mnyCredit( row.mnyCredit ), mnyDebit( row.mnyDebit ),
        sCode( row.sCode ), sDescription( row.sDescription ) {};
    TableRowDef( idCashTransaction_t idCashTransaction_, idAccount_t idAccount_, idCurrency_t idCurrency_, 
      ptime dtTimeStamp_,
      const money_t& mnyCredit_, const money_t& mnyDebit_, 
      const std::string& sCode_, const std::string& sDescription_ ) 
      : idCashTransaction( idCashTransaction_ ), idAccount( idAccount_ ), idCurrency( idCurrency_ ),
        dtTimeStamp( dtTimeStamp_ ),
        mnyCredit( mnyCredit_ ), mnyDebit( mnyDebit_ ), 
        sCode( sCode_ ), sDescription( sDescription_ ) {};
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "cashtransactionid" );
      ou::db::Constraint( a, "accountid", tablenames::sAccount, "accountid" );
      // build constraint based upon combo of idAccount, idCashAccount?
    }
  };

  CashTransaction(
    idCashTransaction_t idCashTransaction, const idAccount_t& idAccount, const idCurrency_t& idCurrency,
    const ptime& dtTimeStamp,
    const money_t& mnyCredit, const money_t& mnyDebit,
    const std::string& sCode, const std::string& sDescription 
    );
  CashTransaction( const TableRowDef& row ) : m_row( row ) {};
  ~CashTransaction(void) {};

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:
private:
  TableRowDef m_row;
};

} // namespace tf
} // namespace ou

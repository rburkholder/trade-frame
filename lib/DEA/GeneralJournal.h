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

// started 2013-05-05

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "KeyTypes.h"

namespace ou { // One Unified
namespace dea { // double entry accounting

class GeneralJournal {
public:

  typedef keytypes::idTransaction_t idTransaction_t;
  typedef keytypes::idReference_t idReference_t;
  typedef keytypes::idAccount_t idAccount_t;
  typedef keytypes::money_t money_t;

  typedef boost::shared_ptr<GeneralJournal> pGeneralJournal_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "transactionid", idTransaction );  // unique key
      ou::db::Field( a, "referenceid", idReference );  // ties related entries together
      ou::db::Field( a, "accountid", idAccount );
      ou::db::Field( a, "debit", mnyDebit );
      ou::db::Field( a, "credit", mnyCredit );
      ou::db::Field( a, "code", sCode );
      ou::db::Field( a, "timestamp", dtTimeStamp );
      ou::db::Field( a, "description", sDescription );
    }

    idTransaction_t idTransaction;
    idReference_t idReference;
    idAccount_t idAccount;
    money_t mnyDebit;
    money_t mnyCredit;
    std::string sCode;
    ptime dtTimeStamp;
    std::string sDescription;

    TableRowDef( void ): idTransaction( 0 ), idReference( 0 ), mnyDebit( 0 ), mnyCredit( 0 ), 
      dtTimeStamp( boost::date_time::special_values::not_a_date_time ) {};
    TableRowDef( const TableRowDef& row ): 
      idTransaction( row.idTransaction ), idReference( row.idReference ), idAccount( row.idAccount),
      sCode( row.sCode ), dtTimeStamp( row.dtTimeStamp ),
      mnyDebit( row.mnyDebit ), mnyCredit( row.mnyCredit ), 
      sDescription( row.sDescription ) {};

  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "transactionid" );
      ou::db::Constraint( a, "accountid", "chartofaccounts", "accountid" );
    }
  };

  GeneralJournal(void);
  ~GeneralJournal(void);
protected:
private:
};

} // namespace dea
} // namespace ou

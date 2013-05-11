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

#include <OUCommon/Decimal.h>

namespace ou { // One Unified
namespace dea { // double entry accounting

class GeneralJournal {
public:

  typedef boost::int64_t idGeneralJournal_t;
  typedef boost::shared_ptr<GeneralJournal> pGeneralJournal_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "transactionid", idTransaction );  // unique key
      ou::db::Field( a, "referenceid", idReference );  // ties related entries together
      ou::db::Field( a, "accountid", idAccount );
      ou::db::Field( a, "debit", mnyDebit );
      ou::db::Field( a, "credit", mnyCredit );
      ou::db::Field( a, "description", sDescription );
    }
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

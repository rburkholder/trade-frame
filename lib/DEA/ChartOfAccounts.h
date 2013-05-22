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

#include <string>

#include <boost/shared_ptr.hpp>

#include "KeyTypes.h"

//#include "KeyTypes.h"

/*  Wiley Accounting Reference
1xxx Assets
11xx - 1499  Current
1500 - 1899  Fixed
1900 - Other 
2xxx Liabilities
21xx - 2499  Current
2500 - 2999  Long Term
3000 - Equity
4000 - Retained Earnings
5000 - Revenue
6000 -
7000 - Expenses
7100 -   Administrative
7200 -   General
7300 -   Selling
7500 -   Cost of Goods Sold
8000 - Income Taxes
9000 - Extraordinary Items
*/

namespace ou { // One Unified
namespace dea { // double entry accounting

class ChartOfAccounts {
public:

  typedef keytypes::idAccount_t idAccount_t;
  typedef keytypes::idCurrency_t idCurrency_t;
  typedef boost::shared_ptr<ChartOfAccounts> pChartOfAccounts_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "accountid", idAccount );
      ou::db::Field( a, "currencyid", idCurrency );  // different account id per currency
      ou::db::Field( a, "location", sLocation );
      ou::db::Field( a, "department", sDepartment );
      ou::db::Field( a, "category", sCategory );
      ou::db::Field( a, "subcategory", sSubCategory );
      ou::db::Field( a, "description", sDescription );
    }

    idAccount_t idAccount;
    idCurrency_t idCurrency;
    std::string sLocation;
    std::string sDepartment;
    std::string sCategory;
    std::string sSubCategory;
    std::string sDescription;
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "accountid" );
    }
  };

  ChartOfAccounts(void);
  ~ChartOfAccounts(void);

protected:
private:
  TableRowDef m_row;
};

} // namespace dea
} // namespace ou
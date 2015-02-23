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

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <OUCommon/CurrencyCode.h>
#include <OUCommon/Decimal.h>

#include <OUSQL/Functions.h>

#include "KeyTypes.h"

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

// 20130524  make a user settable account code mask for idAccount validation
// 20130524 make TLV table for (location), (department), (category, subcategory)

namespace ou { // One Unified
namespace dea { // double entry accounting

class Account{
public:

  typedef keytypes::idAccount_t idAccount_t;
  typedef ou::tables::CurrencyCode::idCurrency_t idCurrency_t;
  typedef boost::shared_ptr<Account> pAccount_t;
  typedef keytypes::money_t money_t;

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
      ou::db::Field( a, "periodendtimestamp", dtPeriodEnd );
      ou::db::Field( a, "periodendtotal", mnyPeriodEnd );
    }

    idAccount_t idAccount;
    idCurrency_t idCurrency;
    std::string sLocation;
    std::string sDepartment;
    std::string sCategory;
    std::string sSubCategory;
    std::string sDescription;
    ptime dtPeriodEnd;
    money_t mnyPeriodEnd;

    TableRowDef( void ): dtPeriodEnd( boost::date_time::special_values::not_a_date_time ), mnyPeriodEnd( 0 ) {};
    TableRowDef( const TableRowDef& row ) 
      : idAccount( row.idAccount ), idCurrency( row.idCurrency ), sLocation( row.sLocation ),
      sDepartment( row.sDepartment ), sCategory( row.sCategory ), sSubCategory( row.sSubCategory ), 
      sDescription( row.sDescription ), dtPeriodEnd( row.dtPeriodEnd ), mnyPeriodEnd( row.mnyPeriodEnd ) {};
    TableRowDef( idAccount_t idAccount_,  idCurrency_t idCurrency_, std::string sLocation_, std::string sDepartment_,
      std::string sCategory_, std::string sSubCategory_, std::string sDescription_ ) 
      : idAccount( idAccount_ ), idCurrency( idCurrency_ ), sLocation( sLocation_ ), sDepartment( sDepartment_ ), 
      sCategory( sCategory_ ), sSubCategory( sSubCategory_ ), sDescription( sDescription_ ),
      dtPeriodEnd( boost::date_time::special_values::not_a_date_time ), mnyPeriodEnd( 0 ) {
        if ( !ou::tables::CurrencyCode::IsValid( idCurrency_ ) ) throw std::runtime_error( "bad currency id" );
    };
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "accountid" );
    }
  };

  Account( const TableRowDef& row ): m_row( row ) {};
  ~Account(void);

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:
private:
  TableRowDef m_row;
};

} // namespace dea
} // namespace ou
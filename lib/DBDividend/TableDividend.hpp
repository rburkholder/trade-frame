/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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

/*
 * File:    TableDividend.hpp
 * Author:  raymond@burkholder.net
 * Project: DBDividend
 * Created: April 12, 2025  09:00:13
 */

/*
  Todo: maintain the name of the symbol for readability
  Todo: track notes in a table rather than the dividend config file
  Todo: manage the queries here rather than in Process
*/

#pragma once

#include <string>

#include <boost/date_time/gregorian/greg_date.hpp>

#include <OUSQL/Functions.h>

namespace db {
namespace table {

class Dividend {
public:

  static const std::string c_TableName;

  struct TableRowDef {

    std::string sSymbol;
    boost::gregorian::date dateRun;
    boost::gregorian::date dateExDividend;
    double dblAmountPayed;
    boost::gregorian::date datePayed;
    double dblRate;

    template<typename A>
    void Fields( A& a ) {
      ou::db::Field( a, "symbol_name", sSymbol );
      ou::db::Field( a, "date_run", dateRun );
      ou::db::Field( a, "date_exdividend", dateExDividend );
      ou::db::Field( a, "amount_payed", dblAmountPayed );
      ou::db::Field( a, "date_payed", datePayed );
      ou::db::Field( a, "rate", dblRate );
    }

    TableRowDef() // default constructor
    : dblRate {},  dblAmountPayed {}
    {}

    TableRowDef(
      std::string sSymbol_
    , boost::gregorian::date dateRun_
    , boost::gregorian::date dateExDividend_
    , double dblAmountPayed_
    , boost::gregorian::date datePayed_
    , double dblRate_
    )
    : sSymbol( sSymbol_ ), dateRun( dateRun_ )
    , dblRate( dblRate_ ), dblAmountPayed( dblAmountPayed_ )
    , dateExDividend( dateExDividend_ ), datePayed( datePayed_ )
    {}
  };

  struct TableCreateDef: TableRowDef {
    template<typename A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "symbol_name" );
      ou::db::Key( a, "date_exdividend" );
      ou::db::Key( a, "date_payed" );
    }
  };

  Dividend() {}
  ~Dividend() {}

  //const TableRowDef& GetRow() const { return m_row; };

protected:
private:

  //TableRowDef m_row;
};

} // namespace table
} // namespace db

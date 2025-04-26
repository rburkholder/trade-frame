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
 * File:    Dividend.hpp
 * Author:  raymond@burkholder.net
 * Project: Dividend
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
namespace record {

class Dividend {
public:

  struct TableRowDef {

    std::string sSymbol;
    boost::gregorian::date dateRun;
    double dblRate;
    double dblYield;
    double dblAmountPayed;
    int nAverageVolume;
    double dblLastTrade;
    boost::gregorian::date datePayed;
    boost::gregorian::date dateExDividend;
    int nSharesOutstanding;

    template<typename A>
    void Fields( A& a ) {
      ou::db::Field( a, "symbol_name", sSymbol );
      ou::db::Field( a, "date_run", dateRun );
      ou::db::Field( a, "last_trade", dblLastTrade );
      ou::db::Field( a, "rate", dblRate );
      ou::db::Field( a, "yield", dblYield );
      ou::db::Field( a, "date_payed", datePayed );
      ou::db::Field( a, "amount_payed", dblAmountPayed );
      ou::db::Field( a, "date_exdividend", dateExDividend );
      ou::db::Field( a, "average_volume", nAverageVolume );
      ou::db::Field( a, "shares_outstanding", nSharesOutstanding );
    }

    TableRowDef() // default constructor
    : dblLastTrade {}, dblRate {}, dblYield {}, dblAmountPayed {}
    , nAverageVolume {}, nSharesOutstanding {}
    {}

    TableRowDef(
      std::string sSymbol_
    , boost::gregorian::date dateRun_
    , double dblLastTrade_
    , double dblRate_
    , double dblYield_
    , double dblAmountPayed_
    , int nAverageVolume_
    , int nSharesOutstanding_
    , boost::gregorian::date datePayed_
    , boost::gregorian::date dateExDividend_
    )
    : sSymbol( sSymbol_ ), dateRun( dateRun_ )
    , dblRate( dblRate_ ), dblYield( dblYield_ ), dblAmountPayed( dblAmountPayed_ )
    , nAverageVolume( nAverageVolume_ ), dblLastTrade( dblLastTrade_ )
    , datePayed( datePayed_ ), dateExDividend( dateExDividend_ )
    , nSharesOutstanding( nSharesOutstanding_ )
    {}
  };

  struct TableCreateDef: TableRowDef {
    template<typename A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "symbol_name" );
      ou::db::Key( a, "date_run" );
    }
  };

  Dividend() {}
  ~Dividend() {}

  //const TableRowDef& GetRow() const { return m_row; };

protected:
private:

  //TableRowDef m_row;
};

} // namespace record
} // namespace db

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
 * File:    TableDaily.hpp
 * Author:  raymond@burkholder.net
 * Project: DBDividend
 * Created: August 2, 2025 15:28:21
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

class Daily {
public:

  static const std::string c_TableName;

  struct TableRowDef {

    std::string sSymbol;
    boost::gregorian::date dateRun;
    double dblYield;
    int nAverageVolume;
    double dblLastTrade;
    int nSharesOutstanding;

    template<typename A>
    void Fields( A& a ) {
      ou::db::Field( a, "symbol_name", sSymbol );
      ou::db::Field( a, "date_run", dateRun );
      ou::db::Field( a, "last_trade", dblLastTrade );
      ou::db::Field( a, "yield", dblYield );
      ou::db::Field( a, "average_volume", nAverageVolume );
      ou::db::Field( a, "shares_outstanding", nSharesOutstanding );
    }

    TableRowDef() // default constructor
    : dblLastTrade {}, dblYield {}
    , nAverageVolume {}, nSharesOutstanding {}
    {}

    TableRowDef(
      std::string sSymbol_
    , boost::gregorian::date dateRun_
    , double dblLastTrade_
    , double dblYield_
    , int nAverageVolume_
    , int nSharesOutstanding_
    )
    : sSymbol( sSymbol_ ), dateRun( dateRun_ )
    , dblYield( dblYield_ )
    , nAverageVolume( nAverageVolume_ ), dblLastTrade( dblLastTrade_ )
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

  Daily() {}
  ~Daily() {}

  //const TableRowDef& GetRow() const { return m_row; };

protected:
private:

  //TableRowDef m_row;
};

} // namespace table
} // namespace db

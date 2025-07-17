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
 * File:    TableSymbol.hpp
 * Author:  raymond@burkholder.net
 * Project: Dividend
 * Created: April 26, 2025 10:46:03
 */

#pragma once

#include <string>

#include <boost/date_time/gregorian/greg_date.hpp>

#include <OUSQL/Functions.h>

namespace db {
namespace table {

class Symbol {
public:

  static const std::string c_TableName;

  struct TableRowDef {

    std::string sSymbol;
    std::string sName;
    boost::gregorian::date dateCreated;
    boost::gregorian::date dateUpdated;
    std::string sNotes;

    template<typename A>
    void Fields( A& a ) {
      ou::db::Field( a, "symbol_name", sSymbol );
      ou::db::Field( a, "symbol_fullname", sName );
      ou::db::Field( a, "date_created", dateCreated );
      ou::db::Field( a, "date_updated", dateUpdated );
      ou::db::Field( a, "notes", sNotes );
    }

    TableRowDef() // default constructor
    {}

    TableRowDef(
      std::string sSymbol_
    , std::string sName_
    , boost::gregorian::date dateCreated_
    , boost::gregorian::date dateUpdated_
    , std::string sNotes_
    )
    : sSymbol( sSymbol_ ), sName( sName_ )
    , dateCreated( dateCreated_ ), dateUpdated( dateUpdated_ )
    , sNotes( sNotes_ )
    {}
  };

  struct TableCreateDef: TableRowDef {
    template<typename A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "symbol_name" );
    }
  };

  Symbol() {}
  ~Symbol() {}

  //const TableRowDef& GetRow() const { return m_row; };

protected:
private:

  //TableRowDef m_row;
};

} // namespace table
} // namespace db

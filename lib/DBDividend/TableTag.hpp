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
 * File:    TableTag.hpp
 * Author:  raymond@burkholder.net
 * Project: Dividend
 * Created: April 26, 2025 10:38:26
 */

#pragma once

#include <string>

#include <OUSQL/Functions.h>

namespace db {
namespace table {

class Tag {
public:

  static const std::string c_TableName;

  struct TableRowDef {

    std::string sTag;
    std::string sSymbol;

    template<typename A>
    void Fields( A& a ) {
      ou::db::Field( a, "tag_name", sTag );
      ou::db::Field( a, "symbol_name", sSymbol );
    }

    TableRowDef() // default constructor
    {}

    TableRowDef(
      std::string sTag_
    , std::string sSymbol_
    )
    : sTag( sTag_ ), sSymbol( sSymbol_ )
    {}
  };

  struct TableCreateDef: TableRowDef {
    template<typename A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "tag_name" );
      ou::db::Key( a, "symbol_name" );
    }
  };

  Tag() {}
  ~Tag() {}

  //const TableRowDef& GetRow() const { return m_row; };

protected:
private:

  //TableRowDef m_row;
};

} // namespace table
} // namespace db

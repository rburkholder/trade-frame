/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#pragma once

#include <string>

#include <OUSQL/Functions.h>

#include "KeyTypes.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// 2013/08/30need to add in time zone reference into database record

class Exchange
{
public:

  typedef keytypes::idExchange_t idExchange_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "exchangeid", idExchange );
      ou::db::Field( a, "name", sName );
      ou::db::Field( a, "countryid", idCountry );
    }

    idExchange_t idExchange;
    std::string sName;
    std::string idCountry;

    TableRowDef( void ) {};
    TableRowDef( const idExchange_t& idExchange_, const std::string& sName_, const std::string& idCountry_ )
      : idExchange( idExchange_ ), sName( sName_ ), idCountry( idCountry_ ) {};
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "exchangeid" );
    }
  };

  Exchange( const idExchange_t& sExchangeId, const std::string& sName, const std::string& sCountryId );
  ~Exchange(void);

protected:
private:

  TableRowDef m_row;

};

} // namespace tf
} // namespace ou


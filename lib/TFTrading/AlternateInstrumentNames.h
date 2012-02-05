/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "TradingEnumerations.h"
#include "KeyTypes.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class AlternateInstrumentName {
public:
  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "providerid", idProvider ); // part of unique key
      ou::db::Field( a, "alternateid", idAlternate ); // part of unique key
      ou::db::Field( a, "instrumentid", idInstrument );  // can be used for secondary index
    }

    keytypes::eidProvider_t idProvider;
    keytypes::idInstrument_t idAlternate;
    keytypes::idInstrument_t idInstrument;

    TableRowDef( const keytypes::eidProvider_t& idProvider_, const keytypes::idInstrument_t& idAlternate_, const keytypes::idInstrument_t& idInstrument_ ):
      idProvider( idProvider_ ), idAlternate( idAlternate_ ), idInstrument( idInstrument_ ) {};
    TableRowDef( void ) {};
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "providerid" );  // unique key part
      //ou::db::Key( a, "instrumentid" );
      ou::db::Key( a, "alternateid" ); // unique key part
      ou::db::Constraint( a, "instrumentid", tablenames::sInstrument, "instrumentid" );
      //ou::db::Constraint( a, "alternateid", tablenames::sInstrument, "instrumentid" );  // don't think this one makes sense
      // set idInstrument as secondary index
    }
  };

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:
private:

  TableRowDef m_row;

};

} // namespace tf
} // namespace ou

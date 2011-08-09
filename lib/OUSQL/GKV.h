/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <boost/shared_ptr.hpp>

#include "Functions.h"

namespace ou {
namespace db {

template<class T=std::string>
class GKV { // Group Key Value
public:

  typedef typename boost::shared_ptr<GKV<T> > pGKV_t;

  struct TableRowDef {

    std::string sGroup;
    std::string sKey;
    T value;

    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "group", sGroup );
      ou::db::Field( a, "key", sKey );
      ou::db::Field( a, "value", value );
    }

    TableRowDef( void ) {};
    TableRowDef( const TableRowDef& trd ) 
      : sGroup( trd.sGroup ), sKey( trd.sKey ), sValue( trd.sValue ) {};
    TableRowDef( const std::string& sGroup_, const std::string& sKey_, const T& sValue_ )
      : sGroup( sGroup_ ), sKey( sKey_ ), sValue( sValue_ ) {};
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "group" );
      ou::db::Key( a, "key" );
    }
  };

  GKV( void ) {};
  GKV( const std::string& sGroup_, const std::string& sKey_, const T& sValue_ )
    : m_row( sGroup_, sKey_, sValue_ ) {};
  GKV( const TableRowDef& row ) : m_row( row ) {};
  ~GKV( void ) {};

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:
private:
  TableRowDef m_row;
};

} // db
} // ou

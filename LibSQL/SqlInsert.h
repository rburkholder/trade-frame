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

#include "FieldDef.h"
#include "Sql.h"

namespace ou {
namespace db {

class Action_FieldsForInsert: public Action_AddFields {
public:

  Action_FieldsForInsert( void );
  ~Action_FieldsForInsert( void );

  void ComposeStatement( const std::string& sTableName, std::string& sStatement );

protected:
private:
};

//
// CSqlInsert
//

template<class F>  // F: Field Defs
class CSqlInsert: public CSqlNamedTable<F> {
public:

  typedef boost::shared_ptr<CSqlInsert<F> > pCSqlInsert_t;

  CSqlInsert( IDatabase& db, const std::string& sTableName );
  ~CSqlInsert( void ) {};

protected:

  void ComposeStatement( std::string& sStatement );

private:
  CSqlInsert( void );  // no default constructor
  CSqlInsert( const CSqlInsert& );  // no default copy constructor
};

template<class F>
CSqlInsert<F>::CSqlInsert( IDatabase& db, const std::string& sTableName )
: CSqlNamedTable<F>( db, sTableName )
{
  PrepareStatement();
}

template<class F>
void CSqlInsert<F>::ComposeStatement( std::string& sStatement ) {

  Action_FieldsForInsert ffi;  // action structure maintenance

  F f;

  f.Fields( ffi );  // build structure from source definitions

  ffi.ComposeStatement( TableName(), sStatement );  // build statement from structures

}

} // db
} // ou

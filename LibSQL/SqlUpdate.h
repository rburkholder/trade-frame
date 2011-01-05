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

#include "Sql.h"

namespace ou {
namespace db {

//
// CSqlUpdate
//

template<class F>  // F: Field Defs
class CSqlUpdate: public CSqlNamedTable<F> {
public:

  typedef boost::shared_ptr<CSqlUpdate<F> > pCSqlUpdate_t;

  CSqlUpdate( IDatabase& db, const std::string& sTableName );
  ~CSqlUpdate( void ) {};

protected:

  void ComposeStatement( std::string& sStatement );

private:
  CSqlUpdate( void );  // no default constructor
  CSqlUpdate( const CSqlUpdate& );  // no default copy constructor
};

template<class F>
CSqlUpdate<F>::CSqlUpdate( IDatabase& db, const std::string& sTableName )
: CSqlNamedTable<F>( db, sTableName )
{
  PrepareStatement();
}

template<class F>
void CSqlUpdate<F>::ComposeStatement( std::string& sStatement ) {

  Action_FieldsForUpdate ffu;  // action structure maintenance

  F f;

  f.Fields( ffu );  // build structure from source definitions

  ffu.ComposeStatement( TableName(), sStatement );  // build statement from structures

}

} // db
} // ou

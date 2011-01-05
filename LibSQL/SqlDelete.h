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
// CSqlDelete
//

template<class F>  // F: Field Defs
class CSqlDelete: public CSqlNamedTable<F> {
public:

  typedef boost::shared_ptr<CSqlDelete<F> > pCSqlDelete_t;

  CSqlDelete( IDatabase& db, const std::string& sTableName );
  ~CSqlDelete( void ) {};

protected:

  void ComposeStatement( std::string& sStatement );

private:
  CSqlDelete( void );  // no default constructor
  CSqlDelete( const CSqlDelete& );  // no default copy constructor
};

template<class F>
CSqlDelete<F>::CSqlDelete( IDatabase& db, const std::string& sTableName )
: CSqlNamedTable<F>( db, sTableName )
{
  PrepareStatement();
}

template<class F>
void CSqlDelete<F>::ComposeStatement( std::string& sStatement ) {

  Action_FieldsForDelete ffd;  // action structure maintenance

  F f;

  f.Fields( ffd );  // build structure from source definitions

  ffd.ComposeStatement( TableName(), sStatement );  // build statement from structures

}

} // db
} // ou

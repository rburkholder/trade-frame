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

// 
// deals with a prepared statement
// once prepared, cannot be re-used with a different string
//

#include <string>

#include <boost\shared_ptr.hpp>

#include "IDatabase.h"

namespace ou {
namespace db {

//
// CSqlBase
//

class CSqlBase {
public:

  typedef boost::shared_ptr<CSqlBase> pCSqlBase_t;

  CSqlBase( IDatabase& db );
  virtual ~CSqlBase(void);  // sqlite requires 'finish', postgresql has no deletion 

  void ExecuteStatement( void );

protected:

  void PrepareStatement( void );  // automatically called by inheritor upon object instantiation 

private:

  IDatabase& m_db;
  IDatabase::structStatement* m_pStatement;

  CSqlBase(void); // no default constructor
};

//
// CSql
//

template<class F>  // F: Field Definitions
class CSql: public CSqlBase {
public:

  typedef typename boost::shared_ptr<CSql> pCSql_t;

  CSql( IDatabase& db );
  ~CSql(void);

protected:
private:
  F* m_f;
  CSql( void );
};

template<class F>
CSql<F>::CSql( IDatabase& db )
  : CSqlBase( db ), m_f( 0 )
{
}

template<class F>
CSql<F>::~CSql( void ) {
}

//
// CSqlNamedTable
//

template<class F> // F: Field Definitions
class CSqlNamedTable: public CSql<F> {
public:
  CSqlNamedTable( IDatabase& db, const std::string& sTableName );
  ~CSqlNamedTable( void ) {};
protected:
  const std::string& TableName( void ) { return m_sTableName; };
private:
  std::string m_sTableName;
  CSqlNamedTable( void );
  CSqlNamedTable( const CSqlNamedTable& );
};

template<class F>
CSqlNamedTable<F>::CSqlNamedTable( IDatabase& db, const std::string& sTableName ) 
  : CSql<F>( db ), m_sTableName( sTableName )
{
}

} // db
} // ou

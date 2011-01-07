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
#include <map>
#include <vector>
#include <stdexcept>
#include <typeinfo>

#include <boost\shared_ptr.hpp>

#include "IDatabase.h"
//#include "Sql.h"

//#include "TableDef.h"
//#include "SqlInsert.h"
//#include "SqlUpdate.h"
//#include "SqlDelete.h"
//#include "SqlGeneric.h"

namespace ou {
namespace db {


// CQuery contains:
// * DB: structures dedicated to the specific database
// * F: struct containing Fields function with ou::db::Field calls

class QueryBase {  // used as base representation for stowage in vectors and such
public:
  typedef boost::shared_ptr<QueryBase> pQueryBase_t;
  QueryBase( void ) {};
  virtual ~QueryBase( void ) {};
protected:
private:
};

// =====

template<class F>  // used for returning structures to queriers
class QueryFields: 
  public QueryBase, 
  public F
{
public:
  typedef boost::shared_ptr<QueryFields<F> > pQueryFields_t;
protected:
private:
};

// =====

template<class DB, class F>  // used for getting stuff in and out of the database via the session
class Query: 
  public QueryFields<F>,
  public DB
{
public:
  typedef typename boost::shared_ptr<Query<DB, F> > pQuery_t;
  std::string m_sQueryText;  // 'compose' results end up here
protected:
private:
};

//
// CSession
//   IDatabase needs typedef for structStatementState
//

template<class IDatabase>
class CSession {
public:

   CSession( void );
  ~CSession( void );

  void Open( const std::string& sDbFileName, enumOpenFlags flags = EOpenFlagsZero );
  void Close( void );

  template<class F> // T: Table Class with TableDef member function
  typename QueryFields<F>::pQueryFields_t RegisterTable( const std::string& sTableName ) {
    mapTableDefs_iter_t iter = m_mapTableDefs.find( sTableName );
    if ( m_mapTableDefs.end() != iter ) {
      throw std::runtime_error( "table name already has definition" );
    }

    typedef typename Query<IDatabase::structStatementState, F>::pQuery_t pQuery_t; 
    pQuery_t pQuery( new Query<IDatabase::structStatementState, F> );  // add empty table definition

    IDatabase::Action_Assemble_TableDef action( sTableName );
    pQuery->Fields( action );
    action.ComposeCreateStatement( pQuery->m_sQueryText );

    m_db.PrepareStatement( 
      *dynamic_cast<IDatabase::structStatementState*>( pQuery.get() ),
      pQuery->m_sQueryText );

    iter = m_mapTableDefs.insert( 
      m_mapTableDefs.begin(), 
      mapTableDefs_pair_t( sTableName, dynamic_cast<IDatabase::structStatementState*>( pQuery.get() ) ) );

    m_vQuery.push_back( pQuery );

    return pQuery;
  }

  void CreateTables( void );

  template<typename F>
  typename QueryFields<F>::pQueryFields_t RegisterInsert( const std::string& sTableName ) {
    typedef typename Query<IDatabase::structStatementState, F>::pQuery_t pQuery_t; 
    pQuery_t pQuery( new Query<IDatabase::structStatementState, F> );
    m_vQuery.push_back( pQuery );
    
    IDatabase::Action_Compose_Insert action( sTableName );
    pQuery->Fields( action );
    action.ComposeStatement( pQuery->m_sQueryText );

    m_db.PrepareStatement( 
      *dynamic_cast<IDatabase::structStatementState*>( pQuery.get() ),
      pQuery->m_sQueryText );

    return pQuery;
  }

  // need where clause
  template<typename F>
  typename QueryFields<F>::pQueryFields_t RegisterUpdate( const std::string& sTableName ) {
    typedef typename Query<IDatabase::structStatementState, F>::pQuery_t pQuery_t; 
    pQuery_t pQuery( new Query<IDatabase::structStatementState, F> );
    m_vQuery.push_back( pQuery );
    
    IDatabase::Action_Compose_Update action( sTableName );
    pQuery->Fields( action );
    action.ComposeStatement( pQuery->m_sQueryText );

    m_db.PrepareStatement( 
      *dynamic_cast<IDatabase::structStatementState*>( pQuery.get() ),
      pQuery->m_sQueryText );

    return pQuery;
  }

  // need where clause
  template<typename F>
  typename QueryFields<F>::pQueryFields_t RegisterDelete( const std::string& sTableName ) {
    typedef typename Query<IDatabase::structStatementState, F>::pQuery_t pQuery_t; 
    pQuery_t pQuery( new Query<IDatabase::structStatementState, F> );
    m_vQuery.push_back( pQuery );
    
    IDatabase::Action_Compose_Delete action( sTableName );
    pQuery->Fields( action );
    action.ComposeStatement( pQuery->m_sQueryText );

    m_db.PrepareStatement( 
      *dynamic_cast<IDatabase::structStatementState*>( pQuery.get() ),
      pQuery->m_sQueryText );

    return pQuery;
  }

  // also need non-F specialization as there may be no fields involved in some queries
  template<typename F>
  typename QueryFields<F>::pQueryFields_t RegisterQuery( const std::string& sSqlQuery ) {
    typedef typename Query<IDatabase::structStatementState, F>::pQuery_t pQuery_t; 
    pQuery_t pQuery( new Query<IDatabase::structStatementState, F> );
    m_vQuery.push_back( pQuery );

    pQuery->m_sQueryText = sSqlQuery;
    
    m_db.PrepareStatement( 
      *dynamic_cast<IDatabase::structStatementState*>( pQuery.get() ),
      pQuery->m_sQueryText );

    return pQuery;
  }

protected:
private:
  
  IDatabase m_db;

  typedef typename IDatabase::structStatementState* pDBStatementState_t;
  typedef QueryBase::pQueryBase_t pQueryBase_t;

  typedef std::map<std::string, pDBStatementState_t> mapTableDefs_t;  // map table name to table definition
  typedef typename mapTableDefs_t::iterator mapTableDefs_iter_t;
  typedef std::pair<std::string, pDBStatementState_t> mapTableDefs_pair_t;
  mapTableDefs_t m_mapTableDefs;

  typedef std::vector<pQueryBase_t> vQuery_t;
  typedef vQuery_t::iterator vQuery_iter_t;
  vQuery_t m_vQuery;

};

// Constructor
template<class IDatabase>
CSession<IDatabase>::CSession( void ) {
}

// Destructor
template<class IDatabase>
CSession<IDatabase>::~CSession(void) {
  m_db.Close();
}

// Open
template<class IDatabase>
void CSession<IDatabase>::Open( const std::string& sDbFileName, enumOpenFlags flags ) {
  m_db.Open( sDbFileName, flags );
}

// Close
template<class IDatabase>
void CSession<IDatabase>::Close( void ) {
  m_vQuery.clear();
  m_mapTableDefs.clear();
  m_db.Close();
}

// CreateTables
template<class IDatabase>
void CSession<IDatabase>::CreateTables( void ) {
  // todo: need to add a transaction around this set of instructions
  for ( mapTableDefs_iter_t iter = m_mapTableDefs.begin(); m_mapTableDefs.end() != iter; ++iter ) {
    //iter->second->ExecuteStatement();  
  }
}



} // db
} // ou

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

#include <boost/intrusive_ptr.hpp>

#include "Database.h"

namespace ou {
namespace db {

// CQuery contains:
// * SS: statement state structures dedicated to the specific database
// * F: struct containing Fields function with ou::db::Field calls

class QueryBase {  // used as base representation for stowage in vectors and such
protected:
  enum enumClause { EClauseNone, EClauseQuery, EClauseWhere, EClauseOrderBy, EClauseGroupBy };
public:

  typedef boost::intrusive_ptr<QueryBase> pQueryBase_t;

  bool m_bPrepared;  // used by session.execute

  QueryBase( void ): m_bHasFields( false ), m_cntRef( 0 ), m_bPrepared( false ), m_clause( EClauseNone ) {};
  virtual ~QueryBase( void ) {};

  void SetHasFields( void ) { m_bHasFields = true; };
  bool HasFields( void ) { return m_bHasFields; };

  // instrusive reference counting
  size_t RefCnt( void ) { return m_cntRef; };
  void Ref( void ) { ++m_cntRef; };
  size_t UnRef( void ) { --m_cntRef; return m_cntRef; };

  std::string& UpdateQueryText( void ) { 
//    assert( EClauseQuery >= m_clause );
//    m_clause = EClauseQuery;
    return m_sQueryText; 
  };

  const std::string& QueryText( void ) { return m_sQueryText; };

protected:
  enumClause m_clause;
  std::string m_sQueryText;  // 'compose' results end up here
private:
  bool m_bHasFields;
  size_t m_cntRef;
};

// =====

template<class F>  // used for returning structures to queriers
class QueryFields: 
  public QueryBase, 
  public F
{
public:
  typedef boost::intrusive_ptr<QueryFields<F> > pQueryFields_t;
  QueryFields( void ): QueryBase() {};
  ~QueryFields( void ) {};
protected:
private:
};

// =====

template<class SS, class F>  // used for getting stuff in and out of the database via the session
class Query: 
  public QueryFields<F>,
  public SS // statement state
{
public:

  typedef boost::intrusive_ptr<Query<SS, F> > pQuery_t;

  Query( void ): QueryFields<F>() {};
  ~Query( void ) {};

  Query* Where( const std::string& sWhere ) { // todo: ensure sub clause ordering
    assert( EClauseWhere > m_clause );
    m_sQueryText += " WHERE " + sWhere;
    m_clause = EClauseWhere;
    return this; 
  };

  Query* OrderBy( const std::string& sOrderBy ) { // todo: ensure sub clause ordering
    assert( EClauseOrderBy > m_clause );
    m_sQueryText += " ORDERBY " + sOrderBy;
    m_clause = EClauseOrderBy;
    return this;
  }

  Query* GroupBy( const std::string& sGroupBy ) {
    assert( EClauseGroupBy > m_clause );
    m_sQueryText += " GROUPBY " + sGroupBy;
    m_clause = EClauseGroupBy;
    return this;
  }

protected:
private:
};

// functions for intrusive ptr

template<class Q>
void intrusive_ptr_add_ref( Q* pq ) {
  pq->Ref();
}

template<class Q>
void intrusive_ptr_release( Q* pq ) {
  if ( 0 == pq->UnRef() ) {
    delete pq;
  }
}

//
// CSession
//   IDatabase needs typedef for structStatementState
//

template<class IDatabase> // IDatabase is a the specific handler type:  sqlite3 or pg or ...
class CSession {
public:

   CSession( void );
  ~CSession( void );

  void Open( const std::string& sDbFileName, enumOpenFlags flags = EOpenFlagsZero );
  void Close( void );

  template<class F>
  void Bind( typename QueryFields<F>::pQueryFields_t pQuery ) {
    IDatabase::structStatementState& StatementState 
      = *dynamic_cast<IDatabase::structStatementState*>( pQuery.get() );
    if ( !pQuery->m_bPrepared ) {
      m_db.PrepareStatement( StatementState, pQuery->UpdateQueryText() );
      pQuery->m_bPrepared = true;
    }
    IDatabase::Action_Bind_Values action( StatementState );
    pQuery->Fields( action );
  }

  bool Execute( QueryBase::pQueryBase_t pQuery ) {
    IDatabase::structStatementState& StatementState 
      = *dynamic_cast<IDatabase::structStatementState*>( pQuery.get() );
    if ( !pQuery->m_bPrepared ) {
      m_db.PrepareStatement( StatementState, pQuery->UpdateQueryText() );
      pQuery->m_bPrepared = true;
    }
    return m_db.ExecuteStatement( StatementState );
  }

  template<class F, class C>
  void Columns( typename QueryFields<F>::pQueryFields_t pQuery, C& columns ) {
    IDatabase::structStatementState& StatementState 
      = *dynamic_cast<IDatabase::structStatementState*>( pQuery.get() );
    IDatabase::Action_Extract_Columns action( StatementState );
    columns.Fields( action );
  }

  void Reset( QueryBase::pQueryBase_t pQuery ) {
    IDatabase::structStatementState& StatementState 
      = *dynamic_cast<IDatabase::structStatementState*>( pQuery.get() );
    m_db.ResetStatement( StatementState );
  }

  template<class F> // T: Table Class with TableDef member function
  typename Query<typename IDatabase::structStatementState, F>::pQuery_t RegisterTable( const std::string& sTableName ) {
    mapTableDefs_iter_t iter = m_mapTableDefs.find( sTableName );
    if ( m_mapTableDefs.end() != iter ) {
      throw std::runtime_error( "table name already has definition" );
    }

    typedef typename Query<IDatabase::structStatementState, F>::pQuery_t pQuery_t; 
    pQuery_t pQuery( new Query<IDatabase::structStatementState, F> );  // add empty table definition

    IDatabase::Action_Assemble_TableDef action( sTableName );
    pQuery->Fields( action );
    if ( 0 < action.FieldCount() ) pQuery->SetHasFields();
    action.ComposeCreateStatement( pQuery->UpdateQueryText() );

    iter = m_mapTableDefs.insert( 
      m_mapTableDefs.begin(), 
      mapTableDefs_pair_t( sTableName, pQuery) );

    m_vQuery.push_back( pQuery );

    return pQuery;

  }

  void CreateTables( void );

  template<class F>  // do reset, auto bind when doing execute
  typename Query<typename IDatabase::structStatementState, F>::pQuery_t RegisterInsert( const std::string& sTableName ) {
    typedef typename Query<IDatabase::structStatementState, F>::pQuery_t pQuery_t; 
    pQuery_t pQuery( new Query<IDatabase::structStatementState, F> );
    m_vQuery.push_back( pQuery );
    
    IDatabase::Action_Compose_Insert action( sTableName );
    pQuery->Fields( action );
    if ( 0 < action.FieldCount() ) pQuery->SetHasFields();
    action.ComposeStatement( pQuery->UpdateQueryText() );

    return pQuery;
  }

  // need where clause
  template<class F>  // do reset, auto bind when doing execute
  typename Query<typename IDatabase::structStatementState, F>::pQuery_t RegisterUpdate( const std::string& sTableName ) {
    typedef typename Query<IDatabase::structStatementState, F>::pQuery_t pQuery_t; 
    pQuery_t pQuery( new Query<IDatabase::structStatementState, F> );
    m_vQuery.push_back( pQuery );
    
    IDatabase::Action_Compose_Update action( sTableName );
    pQuery->Fields( action );
    if ( 0 < action.FieldCount() ) pQuery->SetHasFields();
    action.ComposeStatement( pQuery->UpdateQueryText() );

    return pQuery;
  }

  // need where clause
  template<class F>  // do reset, auto bind when doing execute
  typename Query<typename IDatabase::structStatementState, F>::pQuery_t RegisterDelete( const std::string& sTableName ) {
    typedef typename Query<IDatabase::structStatementState, F>::pQuery_t pQuery_t; 
    pQuery_t pQuery( new Query<IDatabase::structStatementState, F> );
    m_vQuery.push_back( pQuery );
    
    IDatabase::Action_Compose_Delete action( sTableName );
    pQuery->Fields( action );
    if ( 0 < action.FieldCount() ) pQuery->SetHasFields();
    action.ComposeStatement( pQuery->UpdateQueryText() );

    return pQuery;
  }

  // also need non-F specialization as there may be no fields involved in some queries
  // also need two templates:  one for query parameters, one for column binding, or use a separate method for retrieval
  // that would be why they have a a different database object for the field delivery:
  //   one inbound, one outbound
  // todo:  need to do field processing, so can get field count, so need a processing action
  template<class F>  // do reset, auto bind if variables exist
  typename Query<typename IDatabase::structStatementState, F>::pQuery_t RegisterQuery( const std::string& sSqlQuery ) {
    typedef typename Query<IDatabase::structStatementState, F>::pQuery_t pQuery_t; 
    pQuery_t pQuery( new Query<IDatabase::structStatementState, F> );
    m_vQuery.push_back( pQuery );

    pQuery->UpdateQueryText() = sSqlQuery;
    
    return pQuery;
  }

protected:
private:

  bool m_bOpened;
  
  IDatabase m_db;

  typedef QueryBase::pQueryBase_t pQueryBase_t;

  typedef std::map<std::string, pQueryBase_t> mapTableDefs_t;  // map table name to table definition
  typedef typename mapTableDefs_t::iterator mapTableDefs_iter_t;
  typedef std::pair<std::string, pQueryBase_t> mapTableDefs_pair_t;
  mapTableDefs_t m_mapTableDefs;

  typedef std::vector<pQueryBase_t> vQuery_t;
  typedef vQuery_t::iterator vQuery_iter_t;
  vQuery_t m_vQuery;

};

// Constructor
template<class IDatabase>
CSession<IDatabase>::CSession( void ): m_bOpened( false ) {
}

// Destructor
template<class IDatabase>
CSession<IDatabase>::~CSession(void) {
  Close();
}

// Open
template<class IDatabase>
void CSession<IDatabase>::Open( const std::string& sDbFileName, enumOpenFlags flags ) {
  if ( m_bOpened ) {
    std::string sErr( "Session already opened" );
    throw std::runtime_error( sErr );
  }
  else {
    m_db.Open( sDbFileName, flags );
    m_bOpened = true;
  }
  
}

// Close
template<class IDatabase>
void CSession<IDatabase>::Close( void ) {
  if ( m_bOpened ) {
    m_mapTableDefs.clear();
    for ( vQuery_iter_t iter = m_vQuery.begin(); iter != m_vQuery.end(); ++iter ) {
      m_db.CloseStatement( *dynamic_cast<IDatabase::structStatementState*>( iter->get() ) );
    }
    m_vQuery.clear();
    m_db.Close();
    m_bOpened = false;
  }
}

// CreateTables
template<class IDatabase>
void CSession<IDatabase>::CreateTables( void ) {
  // todo: need to add a transaction around this set of instructions
  for ( mapTableDefs_iter_t iter = m_mapTableDefs.begin(); m_mapTableDefs.end() != iter; ++iter ) {
    Execute( iter->second );
  }
}

} // db
} // ou

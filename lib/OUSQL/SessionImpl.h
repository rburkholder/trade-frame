/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#pragma once

// generic header for session management
// header is designed so only one type of database backend can be linked in
// will need to work with namespace stuff in order to allow multiple number of database backends

// 2011-01-30
// need to work on:
//   getting key back when inserting new record with auto-increment
//   adding transactions with rollback and commit

// OUSqlite Library
// was going to be pimpl, but couldn't, as templates are in use, no 'export' keyword
// includes are a bit messy, a compendium of ousql/session, ousql/sessionimpl, and this session file
// maybe using Spirit::Phoenix tuplies will get me out of this mess?

// 2011/04/25
// need to add 'constraint unique ... ' clause during table creation (needs revaming of constraint symbology)
// need to add ''create index ... ' clause during table creation
// need to add transaction/commit/rollback capabilities

// 2012/10/13
// QueryFields has a problem.  Things have to be re-written so that a statement can be prepared
// independently of a supplied structure.  A new structure may be required on each execution of the statement.
// Currently, the same physical structure needs to be re-used.  Structure is provided during statement construction,
// not necessarily a good thing all the time.


#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <typeinfo>

#include <boost/cstdint.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "Constants.h"
#include "Actions.h"

namespace ou {
namespace db {

// dummy place holder for when no fields to bind
// will need to figure out a better way to handle this
struct NoBind {
  template<class A>
  void Fields( A& a ) { };
};

// CQuery contains:
// * SS: statement state structures dedicated to the specific database
// * F: struct containing Fields function with ou::db::Field calls

// 2011-01-30  need to modify the structures so they can handle instance when there is no F.

class QueryBase {  // used as base representation for stowage in vectors and such
protected:
  enum enumClause { EClauseNone, EClauseQuery, EClauseWhere, EClauseOrderBy, EClauseGroupBy, EClauseBind, EClauseNoExecute };
public:

  typedef boost::intrusive_ptr<QueryBase> pQueryBase_t;

  QueryBase( void ): m_clause( EClauseNone ), m_bHasFields( false ), m_cntRef( 0 ), m_bPrepared( false ) {};
  virtual ~QueryBase( void ) {};

  void SetHasFields( void ) { m_bHasFields = true; };
  inline bool HasFields( void ) { return m_bHasFields; };

  void SetPrepared( void ) { m_bPrepared = true; };
  inline bool IsPrepared( void ) { return m_bPrepared; };

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
  bool m_bPrepared;  // used by session.execute
};

// =====

template<class F>  // used for returning structures to queriers
class QueryFields:
  public QueryBase
{
public:
  typedef boost::intrusive_ptr<QueryFields<F> > pQueryFields_t;
  explicit QueryFields( F& f ): QueryBase(), var( f ) {};
  ~QueryFields( void ) {};
  F& var;  // 2011/03/07  I want to make this a reference to a constant var at some point
   // will require mods to Bind, and will need Fields( A& a ) const, hopefully will work with the Actions passed in
protected:
private:
};

// =====

template<class F>
class Query: public QueryFields<F> {  // rename to Query once SessionImpl Query has been renamed successfully
public:

  explicit Query( F& f ): m_bExecuteOneTime( false ), QueryFields<F>( f ) {};
  ~Query( void ) {};

  Query& Where( const std::string& sWhere ) { // todo: ensure sub clause ordering
    assert( QueryBase::EClauseWhere > QueryBase::m_clause );
    QueryBase::m_sQueryText += " WHERE " + sWhere;
    QueryBase::m_clause = QueryBase::EClauseWhere;
    return *this;
  };

  Query& OrderBy( const std::string& sOrderBy ) { // todo: ensure sub clause ordering
    assert( QueryBase::EClauseOrderBy > QueryBase::m_clause );
    QueryBase::m_sQueryText += " ORDER BY " + sOrderBy;
    QueryBase::m_clause = QueryBase::EClauseOrderBy;
    return *this;
  }

  Query& GroupBy( const std::string& sGroupBy ) {
    assert( QueryBase::EClauseGroupBy > QueryBase::m_clause );
    QueryBase::m_sQueryText += " GROUP BY " + sGroupBy;
    QueryBase::m_clause = QueryBase::EClauseGroupBy;
    return *this;
  }

  Query& NoExecute( void ) {
    assert( QueryBase::EClauseNoExecute > QueryBase::m_clause );
    m_bExecuteOneTime = false; // don't execute on the conversion
    QueryBase::m_clause = QueryBase::EClauseNoExecute;
    return *this;
  }

  operator QueryFields<F>() {
    return dynamic_cast<QueryFields<F> >( *this );
  }

  // conversion operator:  upon conversion from QueryState to QueryFields (upon assignment), execute the bind and execute
  // may need to add an auto-reset before the bind:  therefore need m_bBound member variable
/*
  operator QueryFields<F>*() {
    if ( m_bExecuteOneTime ) {
      ProcessInQueryState();
      m_bExecuteOneTime = false;
    }
    return dynamic_cast<QueryFields<F>* >( this );
  }
*/
/*
  operator QueryFields<F>&() {
    if ( m_bExecuteOneTime ) {
      ProcessInQueryState();
      m_bExecuteOneTime = false;
    }
    return dynamic_cast<QueryFields<F>&>( *this );
  }
*/
  operator typename QueryFields<F>::pQueryFields_t() { // this one is actually used
    if ( m_bExecuteOneTime ) {
      ProcessInQueryState();
      m_bExecuteOneTime = false;
    }
    typename QueryFields<F>::pQueryFields_t p( this );
    return p;
  }

  void SetExecuteOneTime( void ) { m_bExecuteOneTime = true; };

protected:
  virtual void ProcessInQueryState( void ) {};
  bool m_bExecuteOneTime;
private:
};

// functions for intrusive ptr of the query structures

template<class Q>  // Q = Query
void intrusive_ptr_add_ref( Q* pq ) {
  pq->Ref();
}

template<class Q>
void intrusive_ptr_release( Q* pq ) {
  if ( 0 == pq->UnRef() ) {
    delete pq;
  }
}

// =====

template<class SS, class F, class S>  // SS statement state, F fields, S session
class QueryState:
  public Query<F>,
  public SS // statement state
{
//  friend S;  // ** g++ does not like this, need to provide specific function?
public:

  using pQueryState_t = boost::intrusive_ptr<QueryState<SS, F, S> >;

  QueryState( S& session, F& f ): Query<F>( f ), m_session( session ) {};
  virtual ~QueryState( void ) {};

protected:

  void ProcessInQueryState( void ) {
    if ( QueryBase::HasFields() ) {  // who calls this, there fore had to virtual the destructor
      m_session.Bind( *this );
    }
    m_session.Execute( *this );
  }

private:

  S& m_session;

};


//
// SessionImpl

template<class IDatabase> // IDatabase is a the specific handler type:  sqlite3 or pg or ...
class SessionImpl: boost::noncopyable {
public:

  typedef SessionImpl<IDatabase> session_t;
  typedef boost::shared_ptr<session_t> pSession_t;

  SessionImpl( void );
  virtual ~SessionImpl( void );

  void ImplOpen( const std::string& sDbFileName, enumOpenFlags flags = EOpenFlagsZero );
  void ImplClose( void );

  void CreateTables( void );

  boost::int64_t GetLastRowId( void ) { return m_db.GetLastRowId(); };

  template<class F>
  void Bind( QueryFields<F>& qf ) {
    typename IDatabase::structStatementState& StatementState
      = dynamic_cast<typename IDatabase::structStatementState&>( qf );
    if ( !qf.IsPrepared() ) {
      m_db.PrepareStatement( StatementState, qf.UpdateQueryText() );
      qf.SetPrepared();
    }
    typename IDatabase::Action_Bind_Values action( StatementState );
    qf.var.Fields( action );
  }

  template<class F>
  void Bind( typename QueryFields<F>::pQueryFields_t pQuery ) {
    Bind( *pQuery.get() );
  }

  void Bind( QueryFields<NoBind>& qf ) {
  }

  bool Execute( QueryBase& qb ) {
    typename IDatabase::structStatementState& StatementState
      = dynamic_cast<typename IDatabase::structStatementState&>( qb );
    if ( !qb.IsPrepared() ) {
      m_db.PrepareStatement( StatementState, qb.UpdateQueryText() );
      qb.SetPrepared();
    }
    return m_db.ExecuteStatement( StatementState );
  }

  bool Execute( QueryBase::pQueryBase_t pQuery ) {
    return Execute( *pQuery.get() );
  }

  template<class F, class C>
  void Columns( typename QueryFields<F>::pQueryFields_t pQuery, C& columns ) {
    typename IDatabase::structStatementState& StatementState
      = *dynamic_cast<typename IDatabase::structStatementState*>( pQuery.get() );
    typename IDatabase::Action_Extract_Columns action( StatementState );
    columns.Fields( action );
  }

  void Reset( QueryBase::pQueryBase_t pQuery ) {
    typename IDatabase::structStatementState& StatementState
      = *dynamic_cast<typename IDatabase::structStatementState*>( pQuery.get() );
    m_db.ResetStatement( StatementState );
  }

  template<class F> // T: Table Class with TableDef member function
  QueryState<typename IDatabase::structStatementState, F, session_t>& RegisterTable( const std::string& sTableName ) {

    MapRowDefToTableName<F>( sTableName );

    mapTableDefs_iter_t iter = m_mapTableDefs.find( sTableName );
    if ( m_mapTableDefs.end() != iter ) {
      throw std::runtime_error( "table name already has definition" );
    }

    // test template getting at type without instantiating variable: complains about static call to non static function
    // use full specialization or partial specialization

    typedef QueryState<typename IDatabase::structStatementState, F, session_t> query_t;

    F f;  // warning, this variable goes out of scope before the query is destroyed
    query_t* pQuery = new query_t( *this, f );

    typename IDatabase::Action_Assemble_TableDef action( sTableName );
    f.Fields( action );
    if ( 0 < action.FieldCount() ) pQuery->SetHasFields();
    action.ComposeCreateStatement( pQuery->UpdateQueryText() );

    iter = m_mapTableDefs.insert(
      m_mapTableDefs.begin(),
      mapTableDefs_pair_t( sTableName, pQuery) );

    m_vQuery.push_back( pQuery );

    return *pQuery;

  }

  template<class F>  // do reset, auto bind when doing execute
  QueryState<typename IDatabase::structStatementState, F, session_t>& Insert( F& f ) {
    return ComposeSql<F, typename IDatabase::Action_Compose_Insert>( f );
  }

  template<class F>  // do reset, auto bind when doing execute
  QueryState<typename IDatabase::structStatementState, F, session_t>& Update( F& f ) {
    return ComposeSql<F, typename IDatabase::Action_Compose_Update>( f );
  }

  template<class F>  // do reset, auto bind when doing execute
  QueryState<typename IDatabase::structStatementState, F, session_t>& Delete( F& f ) {
    return ComposeSql<F, typename IDatabase::Action_Compose_Delete>( f );
  }

  // also need non-F specialization as there may be no fields involved in some queries
  // todo:  need to do field processing, so can get field count, so need a processing action
  template<class F>  // do reset, auto bind if variables exist
  QueryState<typename IDatabase::structStatementState, F, session_t>& SQL( const std::string& sSqlQuery, F& f ) {

    typedef QueryState<typename IDatabase::structStatementState, F, session_t> query_t;

    query_t* pQuery = new query_t( *this, f );

    Action_Compose action;
    f.Fields( action );
    if ( 0 < action.FieldCount() ) pQuery->SetHasFields();

    pQuery->UpdateQueryText() = sSqlQuery;

    pQuery->SetExecuteOneTime();

    m_vQuery.push_back( pQuery );

    return *pQuery;
  }

  // query with no parameters
  template<class F>
  QueryState<typename IDatabase::structStatementState, F, session_t>& SQL( const std::string& sSqlQuery ) {
    F f;  // warning, this variable goes out of scope before the query is destroyed
    return SQL( sSqlQuery, f );
  }

  template<class F>
  void MapRowDefToTableName( const std::string& sTableName ) {
    std::string sF( typeid( F ).name() );
    mapFieldsToTable_iter_t iter = m_mapFieldsToTable.find( sF );
    if ( m_mapFieldsToTable.end() == iter ) {
      m_mapFieldsToTable[ sF ] = sTableName;
    }
    else {
      if ( iter->second != sTableName ) {
        throw std::runtime_error( "type already defined" );
      }
    }
  }

protected:

  template<class F>
  //const std::string& MapFieldsToTable( void ) {
  const std::string& GetTableName( void ) {
    std::string t( typeid( F ).name() );
    mapFieldsToTable_iter_t iter = m_mapFieldsToTable.find( t );
    if ( m_mapFieldsToTable.end() == iter ) {
      throw std::runtime_error( "type not found" );
    }
    return iter->second;
  }

  template<class F, class Action>  // do reset, auto bind when doing execute
  QueryState<typename IDatabase::structStatementState, F, session_t>& ComposeSql( F& f ) {

    using query_t = QueryState<typename IDatabase::structStatementState, F, session_t>;

    query_t* pQuery = new query_t( *this, f );

    Action action( GetTableName<F>() );
    f.Fields( action );
    if ( 0 < action.FieldCount() ) pQuery->SetHasFields();
    action.ComposeStatement( pQuery->UpdateQueryText() );

    pQuery->SetExecuteOneTime();

    m_vQuery.push_back( pQuery );

    return *pQuery;
  }

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
  vQuery_t m_vQuery;  // 2013/08/26
  // this is a bad thing to have around, statements should be closed after use.
  // don't worry about re-use, cross that bridge later.
  // in one application there are 32,000 queries, and none are closed,
  // resulting in very long times to reclaim space at the end

  typedef std::map<std::string, std::string> mapFieldsToTable_t;
  typedef mapFieldsToTable_t::iterator mapFieldsToTable_iter_t;
  typedef std::pair<std::string, std::string> mapFieldsToTable_pair_t;
  mapFieldsToTable_t m_mapFieldsToTable;

};

// Constructor
template<class IDatabase>
SessionImpl<IDatabase>::SessionImpl( void ): m_bOpened( false ) {
}

// Destructor
template<class IDatabase>
SessionImpl<IDatabase>::~SessionImpl(void) {
  ImplClose();
}

// Open
template<class IDatabase>
void SessionImpl<IDatabase>::ImplOpen( const std::string& sDbFileName, enumOpenFlags flags ) {
  if ( m_bOpened ) {
    std::string sErr( "Session already opened" );
    throw std::runtime_error( sErr );
  }
  else {
    m_db.SessionOpen( sDbFileName, flags );
    m_bOpened = true;
  }
}

// Close
template<class IDatabase>
void SessionImpl<IDatabase>::ImplClose( void ) {
  if ( m_bOpened ) {
    m_bOpened = false;
    m_mapTableDefs.clear();
    for ( vQuery_iter_t iter = m_vQuery.begin(); iter != m_vQuery.end(); iter++ ) {
      if ( nullptr != iter->get() ) { // TODO: investigate why is zero
        m_db.CloseStatement( *dynamic_cast<typename IDatabase::structStatementState*>( iter->get() ) );
        iter->reset();
      }
    }
    m_vQuery.clear();
    m_db.SessionClose();
    // 2013/08/26 process memory doesn't appear to be relaimed after this
    //   trying again with addition of reset();
  }
}

// CreateTables
template<class IDatabase>
void SessionImpl<IDatabase>::CreateTables( void ) {
  // todo: need to add a transaction around this set of instructions
  for ( mapTableDefs_iter_t iter = m_mapTableDefs.begin(); m_mapTableDefs.end() != iter; ++iter ) {
    Execute( iter->second );
  }
}

} // db
} // ou

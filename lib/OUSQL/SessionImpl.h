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

// 2011-01-30
// need to work on:
//   getting key back when inserting new record with auto-increment
//   adding transactions with rollback and commit

// Not usuable by itself, meant as inclusion in pimpl construction

#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <typeinfo>

#include <boost/noncopyable.hpp>

namespace ou {
namespace db {

// =====

template<class SS, class F, class S>  // SS statement state, F fields, S session
class QueryState: 
  public Query<F>,
  public SS // statement state
{
//  friend S;  // ** g++ does not like this, need to provide specific function?
public:

  typedef boost::intrusive_ptr<QueryState<SS, F, S> > pQueryState_t;

  QueryState( S& session, F& f ): Query<F>( f ), m_session( session ) {};
  ~QueryState( void ) {};

protected:

  void ProcessInQueryState( void ) {
    if ( QueryBase::HasFields() ) {
      m_session.Bind( *this );
    }
    m_session.Execute( *this );
  }

private:

  S& m_session;

};

//
// CSessionImpl
//   IDatabase needs typedef for structStatementState
//

template<class IDatabase> // IDatabase is a the specific handler type:  sqlite3 or pg or ...
class CSessionImpl: boost::noncopyable {
public:

  typedef CSessionImpl<IDatabase> session_t;

  CSessionImpl( void );
  virtual ~CSessionImpl( void );

  void Open( const std::string& sDbFileName, enumOpenFlags flags = EOpenFlagsZero );
  void Close( void );

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

  void CreateTables( void );

  boost::int64_t GetLastRowId( void ) { return m_db.GetLastRowId(); };

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

    typedef QueryState<typename IDatabase::structStatementState, F, session_t> query_t;

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
  vQuery_t m_vQuery;

  typedef std::map<std::string, std::string> mapFieldsToTable_t;
  typedef mapFieldsToTable_t::iterator mapFieldsToTable_iter_t;
  typedef std::pair<std::string, std::string> mapFieldsToTable_pair_t;
  mapFieldsToTable_t m_mapFieldsToTable;

};


// Constructor
template<class IDatabase>
CSessionImpl<IDatabase>::CSessionImpl( void ): m_bOpened( false ) {
}

// Destructor
template<class IDatabase>
CSessionImpl<IDatabase>::~CSessionImpl(void) {
  Close();
}

// Open
template<class IDatabase>
void CSessionImpl<IDatabase>::Open( const std::string& sDbFileName, enumOpenFlags flags ) {
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
void CSessionImpl<IDatabase>::Close( void ) {
  if ( m_bOpened ) {
    m_mapTableDefs.clear();
    for ( vQuery_iter_t iter = m_vQuery.begin(); iter != m_vQuery.end(); ++iter ) {
      m_db.CloseStatement( *dynamic_cast<typename IDatabase::structStatementState*>( iter->get() ) );
    }
    m_vQuery.clear();
    m_db.Close();
    m_bOpened = false;
  }
}

// CreateTables
template<class IDatabase>
void CSessionImpl<IDatabase>::CreateTables( void ) {
  // todo: need to add a transaction around this set of instructions
  for ( mapTableDefs_iter_t iter = m_mapTableDefs.begin(); m_mapTableDefs.end() != iter; ++iter ) {
    Execute( iter->second );
  }
}

} // db
} // ou

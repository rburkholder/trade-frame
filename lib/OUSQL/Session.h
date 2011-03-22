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

// generic header for session management
// header is designed so only one type of database backend can be linked in
// will need to work with namespace stuff in order to allow multiple number of database backends

#include <string>

#include <boost/cstdint.hpp>
#include <boost/intrusive_ptr.hpp>

#include "Database.h"

namespace ou {
namespace db {

// dummy place holder for when no fields to bind
// will need to figure out a better way to handle this
struct NoBind {
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

  QueryBase( void ): m_bHasFields( false ), m_cntRef( 0 ), m_bPrepared( false ), m_clause( EClauseNone ) {};
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
  bool m_bPrepared;  // used by session.execute
  bool m_bHasFields;
  size_t m_cntRef;
};

// =====

template<class F>  // used for returning structures to queriers
class QueryFields: 
  public QueryBase
{
public:
  typedef boost::intrusive_ptr<QueryFields<F> > pQueryFields_t;
  QueryFields( F& f ): QueryBase(), var( f ) {};
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

  Query( F& f ): m_bExecuteOneTime( false ), QueryFields<F>( f ) {};
  ~Query( void ) {};

  Query& Where( const std::string& sWhere ) { // todo: ensure sub clause ordering
    assert( EClauseWhere > m_clause );
    m_sQueryText += " WHERE " + sWhere;
    m_clause = EClauseWhere;
    return *this; 
  };

  Query& OrderBy( const std::string& sOrderBy ) { // todo: ensure sub clause ordering
    assert( EClauseOrderBy > m_clause );
    m_sQueryText += " ORDER BY " + sOrderBy;
    m_clause = EClauseOrderBy;
    return *this;
  }

  Query& GroupBy( const std::string& sGroupBy ) {
    assert( EClauseGroupBy > m_clause );
    m_sQueryText += " GROUP BY " + sGroupBy;
    m_clause = EClauseGroupBy;
    return *this;
  }

  Query& NoExecute( void ) {
    assert( EClauseNoExecute > m_clause );
    m_bExecuteOneTime = false; // don't execute on the conversion
    m_clause = EClauseNoExecute;
    return *this;
  }

  template<class F>
  operator QueryFields<F>() { 
    return dynamic_cast<QueryFields<F> >( *this ); 
  }

  // conversion operator:  upon conversion from QueryState to QueryFields (upon assignment), execute the bind and execute
  // may need to add an auto-reset before the bind:  therefore need m_bBound member variable
  template<class F>
  operator QueryFields<F>*() { 
    if ( m_bExecuteOneTime ) {
      ProcessInQueryState();
      m_bExecuteOneTime = false;
    }
    return dynamic_cast<QueryFields<F>* >( this ); 
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

// pimpl class
class CSessionBase;

//
// CSessionBase
//   independent of IDatabase / structStatementState stuff
//

class CSession {
public:

  CSession( void );
  ~CSession( void );

  void Open( const std::string& sDbFileName, enumOpenFlags flags = EOpenFlagsZero );
  void Close( void );

  template<class F> void Bind( QueryFields<F>& qf );
  template<class F> void Bind( typename QueryFields<F>::pQueryFields_t pQuery );

  bool Execute( QueryBase& qb );
  bool Execute( QueryBase::pQueryBase_t pQuery );

  template<class F, class C> void Columns( typename QueryFields<F>::pQueryFields_t pQuery, C& columns );

  void Reset( QueryBase::pQueryBase_t pQuery );

  template<class F> typename QueryFields<F>& RegisterTable( const std::string& sTableName );

  void CreateTables( void );

  template<class F> typename Query<F>& Insert( F& f );
  template<class F> typename Query<F>& Update( F& f );
  template<class F> typename Query<F>& Delete( F& f );

  template<class F> typename Query< F>& SQL( const std::string& sSqlQuery, F& f );
  template<class F> typename Query< F>& SQL( const std::string& sSqlQuery );

  template<class F> void MapRowDefToTableName( const std::string& sTableName );

protected:
private:
  CSessionBase* pImpl;
};

} // db
} // ou

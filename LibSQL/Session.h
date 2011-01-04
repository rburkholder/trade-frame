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

#include "IDatabase.h"
#include "Sql.h"

#include "TableDef.h"
#include "SqlInsert.h"
#include "SqlUpdate.h"
#include "SqlDelete.h"
#include "SqlGeneric.h"

namespace ou {
namespace db {

//
// CSession
//

template<class IDatabase>
class CSession {
public:

   CSession( void );
  ~CSession( void );

  void Open( const std::string& sDbFileName, enumOpenFlags flags = EOpenFlagsZero );
  void Close( void );

  template<typename T> // T: Table Class with TableDef member function
  typename CTableDef<T>::pCTableDef_t RegisterTable( const std::string& sTableName ) {
    typedef typename CTableDef<T>::pCTableDef_t pCTableDef_t;
    mapTableDefs_iter_t iter = m_mapTableDefs.find( sTableName );
    if ( m_mapTableDefs.end() != iter ) {
      throw std::runtime_error( "table name already has definition" );
    }
    pCTableDef_t pTableDef;
    pTableDef.reset( new CTableDef<T>( m_db, sTableName ) );  // add empty table definition
    iter = m_mapTableDefs.insert( m_mapTableDefs.begin(), mapTableDefs_pair_t( sTableName, pTableDef ) );
    return pTableDef;
  }

  void CreateTables( void );

  template<typename F>
  typename ou::db::CSqlInsert<F>::pCSqlInsert_t RegisterInsert( const std::string& sTableName ) {
    typename ou::db::CSqlInsert<F>::pCSqlInsert_t pCSql;
    m_vSql.push_back( pCSql );
    pCSql.reset( new CSqlInsert<F>( m_db, sTableName ) );
    return pCSql;
  }

  template<typename F>
  typename ou::db::CSqlUpdate<F>::pCSqlUpdate_t RegisterUpdate( const std::string& sTableName ) {
    typename ou::db::CSqlUpdate<F>::pCSqlUpdate_t pCSql;
    m_vSql.push_back( pCSql );
    pCSql.reset( new CSqlUpdate<F>( m_db, sTableName ) );
    return pCSql;
  }

  template<typename F>
  typename ou::db::CSqlDelete<F>::pCSqlDelete_t RegisterDelete( const std::string& sTableName ) {
    typename ou::db::CSqlDelete<F>::pCSqlDelete_t pCSql;
    m_vSql.push_back( pCSql );
    pCSql.reset( new CSqlDelete<F>( m_db, sTableName ) );
    return pCSql;
  }

  template<typename F>
  typename ou::db::CSqlQuery<F>::pCSqlQuery_t RegisterQuery( const std::string& sSqlQuery ) {
    typename ou::db::CSqlQuery<F>::pCSqlQuery_t pCSql;
    m_vSql.push_back( pCSql );
    pCSql.reset( new CSqlQuery<F>( m_db, sSqlQuery ) );
    return pCSql;
  }

protected:
private:
  
  IDatabase m_db;

  typedef ou::db::CSqlBase::pCSqlBase_t pCSqlBase_t;  // track use_count on exit to ensure all removed properly

  typedef std::map<std::string, pCSqlBase_t> mapTableDefs_t;  // map table name to table definition
  typedef mapTableDefs_t::iterator mapTableDefs_iter_t;
  typedef std::pair<std::string, pCSqlBase_t> mapTableDefs_pair_t;
  mapTableDefs_t m_mapTableDefs;

  typedef std::vector<pCSqlBase_t> vSql_t;
  typedef vSql_t::iterator vSql_iter_t;
  vSql_t m_vSql;

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
  m_vSql.clear();
  m_mapTableDefs.clear();
  m_db.Close();
}

// CreateTables
template<class IDatabase>
void CSession<IDatabase>::CreateTables( void ) {
  // todo: need to add a transaction around this set of instructions
  for ( mapTableDefs_iter_t iter = m_mapTableDefs.begin(); m_mapTableDefs.end() != iter; ++iter ) {
    iter->second->ExecuteStatement();  
  }
}



} // db
} // ou

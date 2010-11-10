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

#include "sqlite3.h"

#include "Statement.h"
#include "TableDef.h"

template<typename K, typename M, typename P, typename F>
void LoadObject(
  const std::string& sErrPrefix, const std::string& sSqlSelect,
  K& key,
  sqlite3* pDb,
  sqlite3_stmt** pStmt,
  M& map,  // map
  P& p, // shared ptr
  F fConstructInstance  // function to construct instance of class
  )
  {

  M::iterator iter = map.find( key );
  if ( map.end() != iter ) {
    p = iter->second;
  }
  else {
    if ( NULL == pDb ) {
      std::string sErr( sErrPrefix );
      sErr += ":  no record available";
      throw std::runtime_error( sErr );
    }
    else {
      int rtn;

//      PrepareStatement( sErrPrefix, sSqlSelect, pStmt );

      rtn = p->BindDbKey( *pStmt );
      if ( SQLITE_OK != rtn ) {
        std::string sErr( sErrPrefix );
        sErr += ":  error in bind";
        throw std::runtime_error( sErr );
      }
      rtn = sqlite3_step( *pStmt );
      if ( SQLITE_ROW == rtn ) {
        p.reset( fConstructInstance( key, *pStmt ) );
      }
      else {
        std::string sErr( sErrPrefix );
        sErr += ":  error in load";
        throw std::runtime_error( sErr );
      }

      map.insert( std::pair<K,P>( key, p ) );
    }
  }
}

template<typename K, typename P>
void SqlOpOnObject(
  const std::string& sErrPrefix, const std::string& sSqlOp,
  sqlite3* pDb,
  sqlite3_stmt** pStmt,
  K& key,
  P& p
  )
{
  if ( NULL != pDb ) {
    int rtn;

//    PrepareStatement( sErrPrefix, sSqlOp, pDb, pStmt );

    rtn = p->BindDbKey( *pStmt );
    if ( SQLITE_OK != rtn ) {
      std::string sErr( sErrPrefix );
      sErr += ": error in BindDbKey";
      throw std::runtime_error( sErr );
    }
    rtn = p->BindDbVariables( *pStmt );
    if ( SQLITE_OK != rtn ) {
      std::string sErr( sErrPrefix );
      sErr += ": error in BindDbVariables";
      throw std::runtime_error( sErr );
    }
    rtn = sqlite3_step( *pStmt );
    if ( SQLITE_DONE != rtn ) {
      std::string sErr( sErrPrefix );
      sErr += ": error in step";
      throw std::runtime_error( sErr );
    }
  }

}

template<typename K, typename P>
void DeleteObject(
  const std::string& sErrPrefix, const std::string& sSqlOp,
  sqlite3* pDb,
  sqlite3_stmt** pStmt,
  K& key,
  P& p
  )
{
  if ( NULL != pDb ) {

    int rtn;

//    PrepareStatement( sErrPrefix, sSqlOp, pDb, pStmt );

    rtn = p->BindDbKey( *pStmt );
    if ( SQLITE_OK != rtn ) {
      std::string sErr( sErrPrefix );
      sErr += ": error in BindDbKey";
      throw std::runtime_error( sErr );
    }

    rtn = sqlite3_step( *pStmt );
    if ( SQLITE_DONE != rtn ) {
      std::string sErr( sErrPrefix );
      sErr += ": error in step";
      throw std::runtime_error( sErr );
    }

  }
}

//
// CDbSession
//

class CSession {
public:

  typedef CTableDefBase::pCTableDefBase_t pCTableDefBase_t;
  
  CSession(void);
  CSession( const char* szDbFileName );
  virtual ~CSession(void);

  void Open( const char* szDbFileName );
  void Close( void );

  sqlite3* GetDb( void ) { return m_db; };

  void PrepareStatement(
    const std::string& sErrPrefix, const std::string& sSqlOp, sqlite3_stmt** pStmt );

  template<typename T> // T derived from CStatementBase
  void Prepare( T& stmt ) {
    stmt.Prepare( m_db );
  }

  template<typename TD> // TD = Table Definition ( supplied class )
  void RegisterTableDef( const std::string& sTableName );

protected:
private:

  bool m_bDbOpened;

  sqlite3* m_db;

  std::string m_sDbFileName;

  typedef std::map<std::string, pCTableDefBase_t> mapTableDefs_t;
  typedef mapTableDefs_t::iterator mapTableDefs_iter_t;
  typedef std::pair<std::string, pCTableDefBase_t&> mapTableDefs_pair_t;
  mapTableDefs_t m_mapTableDefs;

};

template<typename TD>
void CSession::RegisterTableDef( const std::string& sTableName ) {
  mapTableDefs_iter_t iter = m_mapTableDefs.find( sTableName );
  if ( m_mapTableDefs.end() != iter ) {
    throw std::runtime_error( "table name already has definition" );
  }
  CTableDefBase pDef.reset( new CTableDef<TD>() );
  iter = m_mapTableDefs.insert( m_mapTableDefs.begin(), mapTableDefs_pair_t( sTableName, pDef ) );
}

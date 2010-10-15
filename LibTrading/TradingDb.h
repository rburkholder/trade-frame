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
#include <stdexcept>

#include <LibSqlite/sqlite3.h>

class CTradingDb
{
public:

  CTradingDb( const char* szDbFileName );
  ~CTradingDb(void);

  sqlite3* GetDb( void ) { return m_pdbTrading; };

  template<typename K, typename M, typename P, typename F1, typename F2> 
  void LoadObject( const std::string& sErrPrefix, const std::string& sSelect, 
    K& key,
    sqlite3_stmt** pStmt,
    M& map, 
    P& p, 
    F1 fBindStatement, 
    F2 fConstructInstance );

  template<typename K, typename M, typename P>
  void InsertObject( const std::string& sErrPrefix, const std::string& sSqlInsert, 
    sqlite3_stmt** pStmt, 
    K& key, 
    M& map,
    P& p
    );

protected:
private:
  sqlite3* m_pdbTrading;
};

template<typename K, typename M, typename P, typename F1, typename F2> 
void CTradingDb::LoadObject( 
  const std::string& sErrPrefix, const std::string& sSqlSelect, 
  K& key, 
  sqlite3_stmt** pStmt, 
  M& map,  // map
  P& p, // shared ptr
  F1 fBindStatement, // function to bind key to statement
  F2 fConstructInstance  // function to construct instance of class
  ) 
  {

  M::iterator iter = map.find( key );
  if ( map.end() != iter ) {
    p = iter->second;
  }
  else {
    if ( NULL == m_pDb ) {
      std::string sErr( sErrPrefix );
      sErr += ":  no record available";
      throw std::runtime_error( sErr );
    }
    else {
      int rtn;
      if ( NULL == *pStmt ) {
        rtn = sqlite3_prepare_v2( m_pDb, sSqlSelect.c_str(), -1, pStmt, NULL );
        if ( SQLITE_OK != rtn ) {
          std::string sErr( sErrPrefix );
          sErr += ":  error in prepare";
          throw std::runtime_error( sErr );
        }
      }
      else {
        rtn = sqlite3_reset( *pStmt );
        if ( SQLITE_OK != rtn ) {
          std::string sErr( sErrPrefix );
          sErr += ":  error in reset";
          throw std::runtime_error( sErr );
        }
      }
    
      rtn = fBindStatement( key, *pStmt );
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

template<typename K, typename M, typename P>
void CTradingDb::InsertObject( 
  const std::string& sErrPrefix, const std::string& sSqlInsert, 
  sqlite3_stmt** pStmt, 
  K& key, 
  M& map,
  P& p
  )
{
  if ( NULL != m_pDb ) {
    int rtn;
    if ( NULL == pStmt ) {
      rtn = sqlite3_prepare_v2( m_pDb, sSqlInsert.c_str(), -1, pStmt, NULL );
      if ( SQLITE_OK != rtn ) {
        p.reset();   // get rid of class instance
        std::string sErr( sErrPrefix );
        sErr += ": error in prepare";
        throw std::runtime_error( sErr );
      }
    }
    else {
      rtn = sqlite3_reset( *pStmt );
      if ( SQLITE_OK != rtn ) {
        p.reset();   // get rid of class instance
        std::string sErr( sErrPrefix );
        sErr += ": error in reset";
        throw std::runtime_error( sErr );
      }
    }
    rtn = p->BindDbKey( *pStmt );
    if ( SQLITE_OK != rtn ) {
      p.reset();   // get rid of class instance
      std::string sErr( sErrPrefix );
      sErr += ": error in BindDbKey";
      throw std::runtime_error( sErr );
    }
    rtn = p->BindDbVariables( *pStmt );
    if ( SQLITE_OK != rtn ) {
      p.reset();   // get rid of class instance
      std::string sErr( sErrPrefix );
      sErr += ": error in BindDbVariables";
      throw std::runtime_error( sErr );
    }
    rtn = sqlite3_step( *pStmt );
    if ( SQLITE_DONE != rtn ) {
      p.reset();   // get rid of class instance
      std::string sErr( sErrPrefix );
      sErr += ": error in step";
      throw std::runtime_error( sErr );
    }
  }

  // if everything is fine, can add record to map
  map.insert( std::pair<K,P>( key, p ) );

}


/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <LibSqlite/sqlite3.h>

#include "ManagerBase.h"
#include "AccountAdvisor.h"
#include "AccountOwner.h"
#include "Account.h"

class CAccountManager: public ManagerBase<CAccountManager, std::string, CAccountAdvisor> {
public:

  typedef CAccountAdvisor::pAccountAdvisor_t pAccountAdvisor_t;

  CAccountManager( void );
  CAccountManager( sqlite3* pDb );
  ~CAccountManager(void);

  void CreateDbTables( void );

  pAccountAdvisor_t AddAccountAdvisor( const std::string& sAccountAdvisorId, const std::string& sAccountAdvisorName );
  pAccountAdvisor_t GetAccountAdvisor( const std::string& sAccountAdvisorId );

  template<typename K, typename M, typename P, typename F1, typename F2> 
  void LoadObject( const std::string& sErrPrefix, const std::string& sSelect, 
    K& key,
    sqlite3_stmt** pStmt,
    M& map, 
    P& p, 
    F1 fBindStatement, F2 fConstructInstance );

protected:


private:

  sqlite3* m_pDb;

  sqlite3_stmt* pStmtLoadAccountAdvisor;
  sqlite3_stmt* pStmtAddAccountAdvisor;

  typedef std::pair<std::string, pAccountAdvisor_t> pairAccountAdvisor_t;
  typedef std::map<std::string, pAccountAdvisor_t> mapAccountAdvisor_t;
  typedef mapAccountAdvisor_t::iterator iterAccountAdvisor_t;
  mapAccountAdvisor_t m_mapAccountAdvisor;

};

template<typename K, typename M, typename P, typename F1, typename F2> 
void CAccountManager::LoadObject( 
  const std::string& sErrPrefix, const std::string& sSelect, 
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
        rtn = sqlite3_prepare_v2( m_pDb, sSelect.c_str(), -1, pStmt, NULL );
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


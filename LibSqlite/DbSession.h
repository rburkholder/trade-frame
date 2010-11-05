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

#include <boost/shared_ptr.hpp>

#include "sqlite3.h"

#include "Statement.h"

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
// various Objects used for processing the TableDef
//

// TableDef_CreateTables

// TableDef_BuildStatement

// TableDef_BindForWrite

// TableDef_ColumnForRead

//
// CDbFieldDefBase
//

class CDbFieldDefBase {
public:
  CDbFieldDefBase( const std::string& sDbFieldName, const std::string& sDbFieldType )
  : m_sDbFieldName( sDbFieldName ), m_sDbFieldType( sDbFieldType )
  {};
  virtual ~CDbFieldDefBase( void ) {};
protected:
  std::string m_sDbFieldName;
  std::string m_sDbFieldType;
private:
};

//
// CDbFieldDef
//

template<typename V> // V: variable for data
class CDbFieldDef: public CDbFieldDefBase {
public:
  CDbFieldDef( const std::string& sDbFieldName, const std::string& sDbFieldType, V& var );
  ~CDbFieldDef( void ) {};
protected:
private:
  V* m_var;
};

template<typename V>
CDbFieldDef::CDbFieldDef( const std::string& sDbFieldName, const std::string& sDbFieldType, V& v )
: CDbFieldDefBase( sDbFieldName, sDbFieldType ),
  m_v( v )
{
}

//
// CDbTableDefBase
//

class CDbTableDefBase {
public:

  boost::shared_ptr<CDbTableDefBase> pCDbTableDefBase_t;

  CDbTableDefBase( void ) {};
  virtual ~CDbTableDefBase( void ) {};
protected:
  // definition of fields

  typedef std::vector<CDbFieldDefBase*> vFields_t;
  typedef vFields_t::iterator vFields_iter_t;
  vFields_t m_vFields;

  // also need to keep table of active records?
private:
};

//
// CDbTableDef
//

template<class TD>  // TD: TableDef
class CDbTableDef: public CDbTableDefBase {
public:
  CDbTableDef( void ): CDbTableDefBase() {};
  ~CDbTableDef( void ) {};
protected:
private:
};

//
// CDbSession
//

class CDbSession {
public:

  typedef CDbTableDefBase::pCDbTableDefBase_t pCDbTableDefBase_t;


  CDbSession(void);
  CDbSession( const char* szDbFileName );
  virtual ~CDbSession(void);

  void Open( const char* szDbFileName );
  void Close( void );

  sqlite3* GetDb( void ) { return m_db; };

  void PrepareStatement(
    const std::string& sErrPrefix, const std::string& sSqlOp, sqlite3_stmt** pStmt );

  template<typename T> // T derived from CStatementBase
  void Prepare( T& stmt ) {
    stmt.Prepare( m_db );
  }

  template<typename TD>
  void RegisterTableDef( const std::string& sTableName );

protected:
private:

  bool m_bDbOpened;

  sqlite3* m_db;

  std::string m_sDbFileName;

  typedef std::map<std::string, pCDbTableDefBase_t> mapTableDefs_t;
  typedef mapTableDefs_t::iterator mapTableDefs_iter_t;
  typedef std::pair<std::string, pCDbTableDefBase_t&> mapTableDefs_pair_t;
  mapTableDefs_t m_mapTableDefs;

};

template<typename TD>
void CDbSession::RegisterTableDef( const std::string& sTableName ) {
  mapTableDefs_iter_t iter = m_mapTableDefs.find( sTableName );
  if ( m_mapTableDefs.end() != iter ) {
    throw std::runtime_error( "table name already has definition" );
  }
  CDbTableDefBase pDef.reset( new CDbTableDef<TD>() );
  iter = m_mapTableDefs.insert( m_mapTableDefs.begin(), mapTableDefs_pair_t( sTableName, pDef ) );
}

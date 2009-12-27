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
#include <cassert>

#include "EnvManager.h"

template<typename T> class CCommonDatabaseFunctions {
public:
  struct structValue {
    u_int32_t nSize;  // number of bytes
    void *pAddr;  // address of beginning of array of bytes
    structValue( void ) : pAddr( NULL ), nSize( 0 ) {};
    structValue( void* addr, u_int32_t size ) : pAddr( addr ), nSize( size ) {};
  };

  explicit CCommonDatabaseFunctions<T>( const std::string &Name );
  ~CCommonDatabaseFunctions<T>(void);

  void Save( Dbt *pKey, Dbt *pValue );
  void Get( Dbt *pKey, Dbt *pValue );
  void Truncate( void );
  void OpenCursor( void );
  void CloseCursor( void );
  void ResetCursor( void );
protected:
  static Db *m_pdb;
  static Dbc *mpdbc;
private:
  static size_t m_nReferences;
};

template<typename T> size_t CCommonDatabaseFunctions<T>::m_nReferences = 0;
template<typename T> Db *CCommonDatabaseFunctions<T>::m_pdb = NULL;

template<typename T> CCommonDatabaseFunctions<T>::CCommonDatabaseFunctions( const std::string &Name ) {
  ++m_nReferences;
  if ( 1 == m_nReferences ) {
    CBerkeleyDBEnvManagerSingleton dms;
    CBerkeleyDBEnvManager mgrEnv = dms.Instance();
    DbEnv *pDbEnv = mgrEnv.GetDbEnv();
    m_pdb = new Db( pDbEnv, 0 );
    assert( NULL != m_pdb );
    m_pdb->open( NULL, mgrEnv.GetBDBFileName(), Name.c_str(), DB_BTREE, DB_CREATE, 0 );
  }
}

template<typename T> CCommonDatabaseFunctions<T>::~CCommonDatabaseFunctions(void) {
  --m_nReferences;
  if ( 0 == m_nReferences ) {
    m_pdb->close(0);
  }
}

template<typename T> void CCommonDatabaseFunctions<T>::Save( Dbt *pKey, Dbt *pValue ) {
  int ret;
  try {
    ret = m_pdb->put( 0, pKey, pValue, 0 ); // overwrite existing value, or create new one
  }
  catch ( DbException e ) {
    std::string err( "CCommonDatabaseFunctions::Save: DbException error, " );
    err.append( e.what() );
    throw std::domain_error( err );
  }
  if ( 0 != ret ) throw std::runtime_error( "CCommonDatabaseFunctions::Save put had error" );
}

template<typename T> void CCommonDatabaseFunctions<T>::Get( Dbt *pKey, Dbt *pValue ) {
  int ret;
  try {
    ret = m_pdb->get( 0, pKey, pValue, 0 );
  }
  catch ( DbException e ) {
    std::string err( "CCommonDatabaseFunctions::Get: DbException error, " );
    err.append( e.what() );
    throw std::domain_error( err );
  }
  if ( DB_NOTFOUND == ret ) {
    throw std::out_of_range( "CCommonDatabaseFunctions::Get key not found" );
  }
  if ( 0 != ret ) throw std::runtime_error( "CCommonDatabaseFunctions::Get get had error" );
}

template<typename T> void CCommonDatabaseFunctions<T>::Truncate( void ) {
  u_int32_t countp = 0;
  m_pdb->truncate( NULL, &countp, 0 );
}

template<typename T> void CCommonDatabaseFunctions<T>::OpenCursor( void ) {
}

template<typename T> void CCommonDatabaseFunctions<T>::ResetCursor( void ) {
}

template<typename T> void CCommonDatabaseFunctions<T>::CloseCursor( void ) {
}


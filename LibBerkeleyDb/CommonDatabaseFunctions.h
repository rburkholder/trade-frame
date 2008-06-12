#pragma once

#include <string>
#include <assert.h>

#include "BerkeleyDb.h"
#include "BerkeleyDBDataManager.h"

template<class T> class CCommonDatabaseFunctions {
public:
  explicit CCommonDatabaseFunctions<T>( const std::string &Name );
  virtual ~CCommonDatabaseFunctions<T>(void);
  struct structValue {
    void *pAddr;  // address of beginning of array of bytes
    u_int32_t nSize;  // number of bytes
    structValue( void ) : pAddr( NULL ), nSize( 0 ) {};
    structValue( void* addr, u_int32_t size ) : pAddr( addr ), nSize( size ) {};
  };
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

template<class T> size_t CCommonDatabaseFunctions<T>::m_nReferences = 0;
template<class T> Db *CCommonDatabaseFunctions<T>::m_pdb = NULL;

template<class T> CCommonDatabaseFunctions<T>::CCommonDatabaseFunctions( const std::string &Name ) {
  ++m_nReferences;
  if ( 1 == m_nReferences ) {
    CBerkeleyDBDataManager dm;
    DbEnv *pDbEnv = dm.GetDbEnv();
    m_pdb = new Db( pDbEnv, 0 );
    assert( NULL != m_pdb );
    m_pdb->open( NULL, dm.GetBDBFileName(), Name.c_str(), DB_BTREE, DB_CREATE, 0 );
  }
}

template<class T> CCommonDatabaseFunctions<T>::~CCommonDatabaseFunctions(void) {
  --m_nReferences;
  if ( 0 == m_nReferences ) {
    m_pdb->close(0);
  }
}

template<class T> void CCommonDatabaseFunctions<T>::Save( Dbt *pKey, Dbt *pValue ) {
  int ret;
  try {
    ret = m_pdb->put( 0, pKey, pValue, 0 ); // overwrite existing value, or create new one
  }
  catch ( DbException e ) {
    string err( "CCommonDatabaseFunctions::Save: DbException error, " );
    err.append( e.what() );
    throw std::domain_error( err );
  }
  if ( 0 != ret ) throw std::runtime_error( "CCommonDatabaseFunctions::Save put had error" );
}

template<class T> void CCommonDatabaseFunctions<T>::Get( Dbt *pKey, Dbt *pValue ) {
  int ret;
  try {
    ret = m_pdb->get( 0, pKey, pValue, 0 );
  }
  catch ( DbException e ) {
    string err( "CCommonDatabaseFunctions::Get: DbException error, " );
    err.append( e.what() );
    throw std::domain_error( err );
  }
  if ( DB_NOTFOUND == ret ) {
    throw std::out_of_range( "CCommonDatabaseFunctions::Get key not found" );
  }
  if ( 0 != ret ) throw std::runtime_error( "CCommonDatabaseFunctions::Get get had error" );
}

template<class T> void CCommonDatabaseFunctions<T>::Truncate( void ) {
  u_int32_t countp = 0;
  m_pdb->truncate( NULL, &countp, 0 );
}

template<class T> void CCommonDatabaseFunctions<T>::OpenCursor( void ) {
}

template<class T> void CCommonDatabaseFunctions<T>::ResetCursor( void ) {
}

template<class T> void CCommonDatabaseFunctions<T>::CloseCursor( void ) {
}

/*
short CKeyValuePair::GetShort(const std::string &key) {
  structKey k( Int16, key.size(), key.c_str() );
  Dbt v;
  short value;
  v.set_flags( DB_DBT_USERMEM );
  v.set_ulen( sizeof( short ) );
  v.set_size( sizeof( short ) );
  v.set_data( &value );  
  Get( k, &v );
  return value;
}
*/
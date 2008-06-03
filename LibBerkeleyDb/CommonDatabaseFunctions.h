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
protected:
  static Db *m_pdb;
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
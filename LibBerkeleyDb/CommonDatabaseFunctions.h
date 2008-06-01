#pragma once

#include <string>

#include "BerkeleyDb.h"

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

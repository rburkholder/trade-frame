#pragma once

#include "CommonDatabaseFunctions.h"

#include <iostream>

class CDbValueStream : 
  public std::streambuf, 
  public CCommonDatabaseFunctions<CDbValueStream> {
public:
  CDbValueStream(void);
  ~CDbValueStream(void);
  //char *GetBuf( void ) { return pbase(); };
  //unsigned long GetLength( void ) { return pptr() - pbase(); };
  void Save( void *pKey, size_t nKeyLength );
protected:
  static const unsigned short BufSize = 1024;
  char buf[ BufSize ]; // arbitrary length sized 
  virtual int_type overflow( int_type meta );
  virtual int sync( void );
private:
};

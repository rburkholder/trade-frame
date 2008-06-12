#pragma once

#include <iostream>

class CDbValueStream : public std::streambuf {
public:
  CDbValueStream(void);
  ~CDbValueStream(void);
  char *GetBuf( void ) { return pbase(); };
  unsigned long GetLength( void ) { return pptr() - pbase(); };
protected:
  static const unsigned short BufSize = 1024;
  char buf[ BufSize ]; // arbitrary length sized 
  virtual int_type overflow( int_type meta );
private:
};

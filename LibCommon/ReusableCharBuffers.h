#pragma once

#include <queue>
using namespace std;

class CCharBuffer {
public:
  CCharBuffer(void);
  CCharBuffer( int nChars, const char *szBuf ); // nchars includes terminator
  virtual ~CCharBuffer(void);
  //CCharBuffer *Duplicate( int nChars, const char *szBuf );
  CCharBuffer *Assign( int nChars, const char *szBuf );

  int Size( void ) { return m_nChar; };
  const char *Buffer( void ) { return m_szBuffer; };
  
protected:
  static const int m_nBasicBufferSize = 100;
  int m_nBufferSize;  // number of characters including terminator
  int m_nChar; // number of char actually in buffer, including terminator
  char *m_szBuffer;
private:
};

class CReusableCharBuffers {
public:
  CReusableCharBuffers(void);
  ~CReusableCharBuffers(void);
  void CheckIn( CCharBuffer *pBuf );
  CCharBuffer *CheckOut( int nChars, const char *szBuf );
protected:
  CRITICAL_SECTION csLockQueue;
  queue<CCharBuffer *> m_qBuffer;
private:
};

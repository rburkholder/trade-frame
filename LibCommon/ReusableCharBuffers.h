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

#include <queue>
using namespace std;

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

// mechanism of re-usable buffers, removes the execution overhead of new/delete

// has some thread safety

// replace critical sections with boost::lock

class CCharBuffer {
public:
  CCharBuffer(void);  // allocate default sized buffer
  CCharBuffer( int nChars, const char *szBuf ); // allocate with nchars, nchars includes terminator, if used
  ~CCharBuffer(void);
  //CCharBuffer *Duplicate( int nChars, const char *szBuf );
  CCharBuffer *Assign( int nChars, const char *szBuf );  // assigns zero terminated string, nchars includes space for terminator

  int Size( void ) { return m_nChar; };
  const char *Buffer( void ) { return m_szBuffer; };
  
protected:
  static const int m_nBasicBufferSize = 100;  // default size of buffer
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
  //CRITICAL_SECTION csLockQueue;
  boost::mutex m_mutex;
  queue<CCharBuffer *> m_qBuffer;
private:
};

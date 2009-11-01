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
#include <sstream>
#include <typeinfo.h>
#include <cassert>
//using namespace std;

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

// mechanism of re-usable buffers, removes the execution overhead of new/delete

// has some thread safety

// Use template style so that various types can be used
// replace CCharBuffer with std::vector, as it auto-grows, and takes care of 
//    various sizing management issues, and has iterators built in

// issue statistics at destruction stage as to how many buffers were allocated (max size queue reached)

// this whole thing may be obsolete as CCharBuffer can be a vector<>, 
//   and CReusableCharBuffers is only need when running with multiple threads

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

// T is the type of buffer to be used
// Thread safe

// stats:  #check ins, #check outs, #created, #destroyed, maxqsize

// may be able to use LockFreeQueues:
//  http://www.ddj.com/hpc-high-performance-computing/208801974

// might use auto_ptr for this

template<class TBuffer> class CBufferRepository {
public:
  CBufferRepository(void);
  ~CBufferRepository(void);
  inline void CheckIn( TBuffer* Buffer );
  inline TBuffer* CheckOut();  
  void CheckInL( TBuffer* Buffer );  // locked version
  TBuffer* CheckOutL();  // locked version
  bool Outstanding( void ) { return ( cntCheckins != cntCheckouts ); };
protected:
  boost::mutex m_mutex;
  std::queue<TBuffer*> m_qBuffer;
private:
  typedef unsigned int stats_pod_t;
  stats_pod_t cntCheckins, cntCheckouts;
#ifdef _DEBUG
  stats_pod_t cntCreated, cntDestroyed, maxQsize;
  bool m_bCheckingOut;
  bool m_bCheckingIn;
  std::string m_sType;
#endif
};


// CBufferRepository

// can the mutex be made compile-time conditional?
// most usage may be single thread mode now, as buffers are being returned to the original
//   thread for storage

template<class TBuffer> CBufferRepository<TBuffer>::CBufferRepository(void) 
: cntCheckins( 0 ), cntCheckouts( 0 )
#ifdef _DEBUG
  , cntCreated( 0 ), cntDestroyed( 0 ), maxQsize( 0 ),
  m_bCheckingOut( false ), m_bCheckingIn( false )
#endif
{
#ifdef _DEBUG
  m_sType = typeid( this ).name();
#endif
}

template<class TBuffer> CBufferRepository<TBuffer>::~CBufferRepository(void) {
  TBuffer* pBuffer;
  boost::mutex::scoped_lock lock(m_mutex);  // for the methods requiring a locki
  while ( !m_qBuffer.empty() ) {
    pBuffer = m_qBuffer.front();
    m_qBuffer.pop();
    delete pBuffer;
#ifdef _DEBUG
    ++cntDestroyed;
#endif
  }
#ifdef _DEBUG
  std::stringstream ss;
  ss << typeid( this ).name()
    << " Created: " << cntCreated
    << " Destroyed: " << cntDestroyed
    << " Checkins: " << cntCheckins
    << " Checkouts: " << cntCheckouts
    << " Max Q Size: " << maxQsize
    << std::endl;
  OutputDebugString( ss.str().c_str() );
  if ( cntCreated != cntDestroyed ) {
    OutputDebugString( "  ** Created != Destroyed\n" );
  }
  if ( cntCheckins != cntCheckouts ) {
    OutputDebugString( "  ** Checkins != Checkouts\n" );
  }
#endif
}

template<class TBuffer> void CBufferRepository<TBuffer>::CheckInL(TBuffer* pBuffer) {
  boost::mutex::scoped_lock lock(m_mutex);
  CheckIn( pBuffer );
}

template<class TBuffer> inline void CBufferRepository<TBuffer>::CheckIn(TBuffer* pBuffer) {
#ifdef _DEBUG
  assert( !m_bCheckingIn && !m_bCheckingOut );
  m_bCheckingIn = true;
#endif
  m_qBuffer.push( pBuffer );
  ++cntCheckins;
#ifdef _DEBUG
  maxQsize = std::max<stats_pod_t>( maxQsize, m_qBuffer.size() );
  m_bCheckingIn = false;
#endif
}

template<class TBuffer> inline TBuffer* CBufferRepository<TBuffer>::CheckOutL() {
  boost::mutex::scoped_lock lock(m_mutex);
  return CheckOut();
}

template<class TBuffer> inline TBuffer* CBufferRepository<TBuffer>::CheckOut() {
  TBuffer* pBuffer;
#ifdef _DEBUG
  assert( !m_bCheckingIn && !m_bCheckingOut );
  m_bCheckingOut = true;
#endif
  if ( m_qBuffer.empty() ) {
    pBuffer = new TBuffer();
#ifdef _DEBUG
    ++cntCreated;
#endif
  }
  else {
    pBuffer = m_qBuffer.front();
    m_qBuffer.pop();
  }
  ++cntCheckouts;
#ifdef _DEBUG
  m_bCheckingOut = false;
#endif
  return pBuffer;
}

 
// check threading id's between checkin, checkout  (is there overlapping thread?)
// Unhandled exception at 0x00548d0d in TapeReader.exe: 0xC0000005: Access violation reading location 0x00000014.
// in 235863, out 235886, created 893 destro 0 max 892 cur 870
// _DEBUG_ERROR("deque iterator not dereferencable");
// set flag to test in out?
// upstream processing isn't keeping up, need to do 1/5 sec screen updates

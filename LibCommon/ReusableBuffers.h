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

//#include <queue>
#include <vector>
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

// use a stack, may help to optimize speed

template<typename bufferT> class CBufferRepository {
public:
  CBufferRepository(void);
  ~CBufferRepository(void);
  inline void CheckIn( bufferT* Buffer );
  inline bufferT* CheckOut();  
  void CheckInL( bufferT* Buffer );  // locked version
  bufferT* CheckOutL();  // locked version
  bool Outstanding( void ) { return ( cntCheckins != cntCheckouts ); };
protected:
  boost::mutex m_mutex;
  //std::queue<bufferT*> m_qBuffer;
  std::vector<bufferT*> m_vStack;
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

template<typename bufferT> CBufferRepository<bufferT>::CBufferRepository(void) 
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

template<typename bufferT> CBufferRepository<bufferT>::~CBufferRepository(void) {
  bufferT* pBuffer;
  boost::mutex::scoped_lock lock(m_mutex);  // for the methods requiring a lock
  while ( !m_vStack.empty() ) {
    pBuffer = m_vStack.back();
    m_vStack.pop_back();
    delete pBuffer;
#ifdef _DEBUG
    ++cntDestroyed;
#endif
  }
#ifdef _DEBUG
  std::stringstream ss;
  ss << typeid( this ).name() << ": "
    << cntCreated << " Created, " 
    << cntDestroyed << " Destroyed, "
    << cntCheckouts << " Checkouts, " 
    << cntCheckins << " Checkins, " 
    << maxQsize << " Max Q Size" 
    << std::endl;
  OutputDebugString( ss.str().c_str() );
  ss.str() = "";
  if ( cntCreated != cntDestroyed ) {
    OutputDebugString( "  ** Created != Destroyed\n" );
  }
  if ( cntCheckins != cntCheckouts ) {
    OutputDebugString( "  ** Checkins != Checkouts\n" );
  }
#endif
}

template<typename bufferT> inline void CBufferRepository<bufferT>::CheckInL(bufferT* pBuffer) {
  boost::mutex::scoped_lock lock(m_mutex);
  CheckIn( pBuffer );
}

template<typename bufferT> inline void CBufferRepository<bufferT>::CheckIn(bufferT* pBuffer) {
#ifdef _DEBUG
  assert( !m_bCheckingIn && !m_bCheckingOut );
  m_bCheckingIn = true;
#endif
  m_vStack.push_back( pBuffer );
  ++cntCheckins;
#ifdef _DEBUG
  maxQsize = std::max<stats_pod_t>( maxQsize, m_vStack.size() );
  m_bCheckingIn = false;
#endif
}

template<typename bufferT> inline bufferT* CBufferRepository<bufferT>::CheckOutL() {
  boost::mutex::scoped_lock lock(m_mutex);
  return CheckOut();
}

template<typename bufferT> inline bufferT* CBufferRepository<bufferT>::CheckOut() {
  bufferT* pBuffer;
#ifdef _DEBUG
  assert( !m_bCheckingIn && !m_bCheckingOut );
  m_bCheckingOut = true;
#endif
  if ( m_vStack.empty() ) {
    pBuffer = new bufferT();
#ifdef _DEBUG
    ++cntCreated;
#endif
  }
  else {
    pBuffer = m_vStack.back();
    m_vStack.pop_back();
  }
  ++cntCheckouts;
#ifdef _DEBUG
  m_bCheckingOut = false;
#endif
  return pBuffer;
}

 
// upstream processing isn't keeping up, need to do 1/5 sec screen updates

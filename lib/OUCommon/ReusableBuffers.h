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


#include <mutex>
#include <vector>
//#include <sstream>
#include <cassert>
#include <functional>

// mechanism of re-usable buffers, removes the execution overhead of new/delete

// has some thread safety

// Use template style so that various types can be used

// issue statistics at destruction stage as to how many buffers were allocated (max size queue reached)

// this whole thing may be obsolete as CCharBuffer can be a vector<>,
//   and CReusableCharBuffers is only need when running with multiple threads


// ======

// T is the type of buffer to be used
// Thread safe

// stats:  #check ins, #check outs, #created, #destroyed, maxqsize

// may be able to use LockFreeQueues:
//  http://www.ddj.com/hpc-high-performance-computing/208801974

// might use auto_ptr for this

// uses a stack to optimize some re-use speed

// BufferRepository

// can the mutex be made compile-time conditional?
// most usage may be single thread mode now, as buffers are being returned to the original
//   thread for storage (actually possibly no, cross thread returns are used)

namespace ou {

template<typename bufferT>
class BufferRepository {
public:
  using pBuffer_t =  bufferT*;
  using fLocked_t = std::function<void()>;
  BufferRepository();
  ~BufferRepository();
  inline void CheckIn( pBuffer_t Buffer );
  inline pBuffer_t CheckOut();
  void CheckInL( pBuffer_t Buffer );  // locked version
  pBuffer_t CheckOutL();  // locked version
  bool Outstanding() { return ( cntCheckins != cntCheckouts ); };
  void ScopedLock( fLocked_t&& fLocked ) {
    if ( fLocked ) {
      std::scoped_lock<std::mutex> lock(m_mutex);
      fLocked();
    }
  }

protected:
  std::mutex m_mutex;
  std::vector<pBuffer_t> m_vStack;
private:
  std::size_t cntCheckins, cntCheckouts;
#ifdef _DEBUG
  std::size_t cntCreated, cntDestroyed, maxQsize;
  bool m_bCheckingOut;
  bool m_bCheckingIn;
  std::string m_sType;
#endif
};


template<typename bufferT> BufferRepository<bufferT>::BufferRepository()
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

template<typename bufferT> BufferRepository<bufferT>::~BufferRepository() {
  pBuffer_t pBuffer;
  std::scoped_lock<std::mutex> lock(m_mutex);  // for the methods requiring a lock
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
//  OutputDebugString( ss.str().c_str() );
  ss.str() = "";
  if ( cntCreated != cntDestroyed ) {
//    OutputDebugString( "  ** Created != Destroyed\n" );
  }
  if ( cntCheckins != cntCheckouts ) {
//    OutputDebugString( "  ** Checkins != Checkouts\n" );
  }
#endif
}

template<typename bufferT> inline void BufferRepository<bufferT>::CheckInL(bufferT* pBuffer) {
  std::scoped_lock<std::mutex> lock(m_mutex);
  CheckIn( pBuffer );
}

template<typename bufferT> inline void BufferRepository<bufferT>::CheckIn(bufferT* pBuffer) {
#ifdef _DEBUG
  assert( !m_bCheckingIn && !m_bCheckingOut );
  m_bCheckingIn = true;
#endif
  assert( pBuffer );
  m_vStack.push_back( pBuffer );
  ++cntCheckins;
#ifdef _DEBUG
  maxQsize = std::max<std::size_t>( maxQsize, m_vStack.size() );
  m_bCheckingIn = false;
#endif
}

template<typename bufferT> inline bufferT* BufferRepository<bufferT>::CheckOutL() {
  std::scoped_lock<std::mutex> lock(m_mutex);
  return CheckOut();
}

template<typename bufferT> inline bufferT* BufferRepository<bufferT>::CheckOut() {
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

} // ou
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

#include <atomic>
#include <vector>

//#include <boost/atomic.hpp>
#include <boost/scope_exit.hpp>

#include <OUCommon/SpinLock.h>

// 2018/07/22 TODO: change the vector manipulation to std::move?

// 2014/09/30 something to verify with existing code
// http://preshing.com/20140709/the-purpose-of-memory_order_consume-in-cpp11/

#include "FastDelegate.h"
// http://www.codeproject.com/cpp/FastDelegate.asp
using namespace fastdelegate;

namespace ou {

template<typename T>  // T: object used in operator(), normally const&
class Delegate {
// Example:
//   Declaration:
//     Delegate<double> OnValueChanged;
//   Add delegate:
//     OnValueChanged.Add( MakeDelegate( this, &MyClass::HandleValueChanged ) );
//   Make use of delegate:
//     OnValueChanged( 3.14 );
//   Remove Delegate:
//    OnValueChanged.Remove( MakeDelegate( this, &MyClass::HandleValueChanged ) );
public:

  using OnDispatchHandler = fastdelegate::FastDelegate1<T>;
  using vDispatch_t = std::vector<OnDispatchHandler>;
  using vsize_t = typename vDispatch_t::size_type;

  Delegate();
  Delegate( const Delegate& );
  Delegate( Delegate&& ); // used during initial emplace only
  ~Delegate();

  void operator()( T );

  void Add( OnDispatchHandler function );
  void Remove( OnDispatchHandler function );

  bool IsEmpty() const { return ( 0 == m_vDispatchMaster.size() ); };
  vsize_t Size() const { return m_vDispatchMaster.size(); };

protected:
private:

  using const_iterator = typename vDispatch_t::const_iterator;

  std::atomic<int> m_cntDispatchProcesses;
  std::atomic<int> m_cntChanges;
  ou::SpinLock m_spinlockVectorUpdate;   // lock against Add/Remove
  ou::SpinLock m_spinlockVectorReplace;  // lock against operator()

  vDispatch_t m_vDispatchMaster;  // master vector used for Add/Remove
  vDispatch_t m_vDispatch;  // used by operator() for dispatch, copied from m_vDispatchMaster

  void VectorReplace();  // handles the copy of m_vDispatchMaster to m_vDispatch

};

template<class T>
Delegate<T>::Delegate()
  : m_cntDispatchProcesses {}, m_cntChanges {}
{
}

template<class T>
Delegate<T>::Delegate( const Delegate<T>& rhs )
  : m_cntDispatchProcesses {}, m_cntChanges {}
  // don't carry over any of the stuff, just re-initialize it.
  // boost::atomic is non-copyable
//  : m_cntDispatchProcesses( rhs.m_cntDispatchProcesses ), m_cntChanges( rhs.m_cntChanges ),
//  m_spinlockVectorUpdate( rhs.m_spinlockVectorUpdate ), m_spinlockVectorReplace( rhs.m_spinlockVectorReplace ),
//  m_vDispatchMaster( rhs.m_vDispatchMaster ), m_vDispatch( rhs.m_vDispatch )
{
//  m_vDispatchMaster.clear();
//  m_vDispatch.clear();
}

template<class T>
Delegate<T>::Delegate( Delegate<T>&& rhs )
: m_cntDispatchProcesses {}, m_cntChanges {}
{
  assert( 0 == rhs.m_cntChanges );
  assert( 0 == rhs.m_cntDispatchProcesses );
  assert( 0 == rhs.m_vDispatch.size() );
  assert( 0 == rhs.m_vDispatchMaster.size() );
}

template<class T>
Delegate<T>::~Delegate() {
  // this object should be deleted in same thread in which it was created
//  int n = m_cntDispatchProcesses.load( boost::memory_order_acquire );
//  if ( 10 < n ) {
//    std::cerr << "~Delegate=" << n << std::endl;
//  }
//  else {
    while (m_cntDispatchProcesses.load( std::memory_order_acquire ) != 0 ); // wait for dispatch to finish
//  }

  m_vDispatch.clear();
  m_vDispatchMaster.clear();
}

template<class T>
void Delegate<T>::operator()( T t ) {

  const_iterator iter;

  m_cntDispatchProcesses.fetch_add( 1, std::memory_order_acquire );
  m_spinlockVectorReplace.wait();  // wait for any running replacement to finish

  { // ensure things get cleared up in the case of exception in delegated function
    BOOST_SCOPE_EXIT_TPL(&m_cntDispatchProcesses) {
      m_cntDispatchProcesses.fetch_sub( 1, std::memory_order_release );
    } BOOST_SCOPE_EXIT_END

    iter = m_vDispatch.begin();  // start dispatching
    while ( m_vDispatch.end() != iter ) {
      (*iter)( t );
      ++iter;
    }
  } // end scope

  // update dispatch with queued changes
  if ( 0 != m_cntChanges.load( std::memory_order_acquire ) ) {
    m_spinlockVectorUpdate.lock();
    VectorReplace();
    m_spinlockVectorUpdate.unlock();
  }

}

template<class T>
void Delegate<T>::Add( OnDispatchHandler function ) {

  m_spinlockVectorUpdate.lock();

  m_vDispatchMaster.push_back( function );

  m_cntChanges.fetch_add( 1, std::memory_order_release );

  VectorReplace();

  m_spinlockVectorUpdate.unlock();

}

template<class T>
void Delegate<T>::Remove( OnDispatchHandler function ) {

  m_spinlockVectorUpdate.lock();

  const_iterator iter = m_vDispatchMaster.begin();
  while ( m_vDispatchMaster.end() != iter ) {
    if ( function == *iter ) {
      m_vDispatchMaster.erase( iter );
      break;  // allow only one deletion
    }
    ++iter;
  }

  m_cntChanges.fetch_add( 1, std::memory_order_release );

  VectorReplace();

  m_spinlockVectorUpdate.unlock();

}

template<class T>
void Delegate<T>::VectorReplace() {

  // perform VectorReplace only outside of operator() method influence
  if ( 0 == m_cntDispatchProcesses.load( std::memory_order_acquire ) ) {
    m_spinlockVectorReplace.lock();
//    m_spinlockVectorUpdate.lock();

    // ensure we are not in process
    if ( 0 == m_cntDispatchProcesses.load( std::memory_order_acquire ) ) {
      m_vDispatch.resize( m_vDispatchMaster.size() );
      typedef typename vDispatch_t::iterator iterator;
      const_iterator ixSrc = m_vDispatchMaster.begin();
      iterator ixDst = m_vDispatch.begin();
      for (
        ;
        ixSrc != m_vDispatchMaster.end();
        ++ixSrc, ++ixDst ) {
          *ixDst = *ixSrc;
      }
    }

    m_cntChanges.store( 0, std::memory_order_release );

//    m_spinlockVectorUpdate.unlock();
    m_spinlockVectorReplace.unlock();

  }

}

} // ou
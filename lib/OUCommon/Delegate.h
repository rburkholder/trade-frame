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

#include <vector>
#include <algorithm>

#include <assert.h>

#include <boost/atomic.hpp>

#include <OUCommon/SpinLock.h>

#include "FastDelegate.h"
// http://www.codeproject.com/cpp/FastDelegate.asp
using namespace fastdelegate;

namespace ou {

template<class RO> 
class Delegate {
// RO: Return Object in call
// example:  m_mgrPortfolio.OnPortfolioAdded.Add( MakeDelegate( this, &ModelPortfolio::AddPortfolioToModel ) );
public:

  typedef FastDelegate1<RO> OnMessageHandler;
  typedef std::vector<OnMessageHandler> vDispatch_t;
  typedef typename vDispatch_t::size_type vsize_t;

  Delegate( void );
  Delegate( const Delegate& );
  ~Delegate( void );

  void operator()( RO );

  void Add( OnMessageHandler function );
  void Remove( OnMessageHandler function );

  bool IsEmpty() const { return ( 0 == m_vDispatchMaster.size() ); };
  vsize_t Size( void ) const { return m_vDispatchMaster.size(); };

protected:
private:

  typedef typename vDispatch_t::const_iterator const_iterator;
  typedef typename vDispatch_t::iterator iterator;

  boost::atomic<int> m_cntDispatchProcesses;
  boost::atomic<int> m_cntChanges;
  ou::SpinLock m_spinlockVectorUpdate;
  ou::SpinLock m_spinlockVectorReplace;

  vDispatch_t m_vDispatchMaster;
  vDispatch_t m_vDispatch;

  void VectorReplace( void );

};

template<class RO> 
Delegate<RO>::Delegate(void) 
  : m_cntDispatchProcesses( 0 ), m_cntChanges( 0 )
{
}

template<class RO>
Delegate<RO>::Delegate( const Delegate& rhs ) 
  : m_cntDispatchProcesses( 0 ), m_cntChanges( 0 )
  // don't carry over any of the stuff, just re-initialize it.
  // boost::atomic has private members which are non-copyable
//  : m_cntDispatchProcesses( rhs.m_cntDispatchProcesses ), m_cntChanges( rhs.m_cntChanges ),
//  m_spinlockVectorUpdate( rhs.m_spinlockVectorUpdate ), m_spinlockVectorReplace( rhs.m_spinlockVectorReplace ),
//  m_vDispatchMaster( rhs.m_vDispatchMaster ), m_vDispatch( rhs.m_vDispatch )
{
}

template<class RO>
Delegate<RO>::~Delegate(void) {
  m_vDispatch.clear();
  m_vDispatchMaster.clear();
}

template<class RO> 
void Delegate<RO>::operator()( RO ro ) {

  const_iterator iter;

  m_cntDispatchProcesses.fetch_add( 1, boost::memory_order_acquire );
  m_spinlockVectorReplace.wait();  // wait for any in process replacement to finish

  iter = m_vDispatch.begin();  // start dispatching
  while ( m_vDispatch.end() != iter ) {
    (*iter)( ro );
    ++iter;
  }

  m_cntDispatchProcesses.fetch_sub( 1, boost::memory_order_release );

  if ( 0 != m_cntChanges.load( boost::memory_order_relaxed ) ) {
    VectorReplace();
  }

}

template<class RO> 
void Delegate<RO>::Add( OnMessageHandler function ) {

  m_spinlockVectorUpdate.lock();

  m_vDispatchMaster.push_back( function );

  m_cntChanges.fetch_add( 1, boost::memory_order_relaxed );

  m_spinlockVectorUpdate.unlock();

  VectorReplace();

}

template<class RO> 
void Delegate<RO>::Remove( OnMessageHandler function ) {

  m_spinlockVectorUpdate.lock();

  iterator iter = m_vDispatchMaster.begin();
  while ( m_vDispatchMaster.end() != iter ) {
    if ( function == *iter ) {
      m_vDispatchMaster.erase( iter );
      break;  // allow only one deletion
    }
    ++iter;
  }

  m_cntChanges.fetch_add( 1, boost::memory_order_relaxed );

  m_spinlockVectorUpdate.unlock();

  VectorReplace();

}

template<class RO>
void Delegate<RO>::VectorReplace( void ) {

  if ( 0 == m_cntDispatchProcesses.load( boost::memory_order_relaxed ) ) {
    m_spinlockVectorReplace.lock();
    m_spinlockVectorUpdate.lock(); 

    // ensure we are not in process
    if ( 0 == m_cntDispatchProcesses.load( boost::memory_order_relaxed ) ) {
      m_vDispatch.clear();
      m_vDispatch = m_vDispatchMaster;
    }

    m_cntChanges.fetch_sub( m_cntChanges.load( boost::memory_order_relaxed ), boost::memory_order_relaxed );

    m_spinlockVectorUpdate.unlock();
    m_spinlockVectorReplace.unlock();
  }

}

} // ou
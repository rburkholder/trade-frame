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

#include <boost/atomic.hpp>

#include <OUCommon/SpinLock.h>

#include "FastDelegate.h"
// http://www.codeproject.com/cpp/FastDelegate.asp
using namespace fastdelegate;

namespace ou {

template<class RO>  // RO: Return Object in operator()
class Delegate {
// example:  
//  Delegate<ClassA> OnPortfolioAdded;
//  OnPortfolioAdded.Add( MakeDelegate( this, &ModelPortfolio::AddPortfolioToModel ) );
// for some object portfolio_model of type RO:
//  OnPortfolioAdded( portfolio_model );
//  OnPortfolioAdded.Remove( MakeDelegate( this, &ModelPortfolio::AddPortfolioToModel ) );
public:

  typedef fastdelegate::FastDelegate1<RO> OnMessageHandler;
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
  ou::SpinLock m_spinlockVectorUpdate;   // lock against Add/Remove
  ou::SpinLock m_spinlockVectorReplace;  // lock against operator()

  vDispatch_t m_vDispatchMaster;  // master vector used for Add/Remove   
  vDispatch_t m_vDispatch;  // used by operator() for dispatch, copied from m_vDispatchMaster

  void VectorReplace( void );  // handles the copy of m_vDispatchMaster to m_vDispatch

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
  // boost::atomic is non-copyable
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
  m_spinlockVectorReplace.wait();  // wait for any running replacement to finish

  iter = m_vDispatch.begin();  // start dispatching
  while ( m_vDispatch.end() != iter ) {
    (*iter)( ro );
    ++iter;
  }

  m_cntDispatchProcesses.fetch_sub( 1, boost::memory_order_release );

  // update dispatch with queued changes
  if ( 0 != m_cntChanges.load( boost::memory_order_acquire ) ) {
    VectorReplace();
  }

}

template<class RO> 
void Delegate<RO>::Add( OnMessageHandler function ) {

  m_spinlockVectorUpdate.lock(); 

  m_vDispatchMaster.push_back( function );

  m_cntChanges.fetch_add( 1, boost::memory_order_release );

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

  m_cntChanges.fetch_add( 1, boost::memory_order_release );

  m_spinlockVectorUpdate.unlock();

  VectorReplace();

}

template<class RO>
void Delegate<RO>::VectorReplace( void ) {

  // perform VectorReplace only outside of operator() method influence
  if ( 0 == m_cntDispatchProcesses.load( boost::memory_order_acquire ) ) {
    m_spinlockVectorReplace.lock();
    m_spinlockVectorUpdate.lock();

    // ensure we are not in process
    if ( 0 == m_cntDispatchProcesses.load( boost::memory_order_acquire ) ) {
      m_vDispatch.clear();
      m_vDispatch = m_vDispatchMaster;
    }

    m_cntChanges.store( 0, boost::memory_order_release );

    m_spinlockVectorUpdate.unlock();
    m_spinlockVectorReplace.unlock();

  }

}

} // ou
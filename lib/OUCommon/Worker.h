/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

// Started 2013/07/12
// Thread encapsulation, generates event upon completion
// Uses code from BaskeTrading class

#include <boost/thread/thread.hpp>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

namespace ou {

namespace action {

class Worker {
public:

  typedef FastDelegate0<> OnActionHandler;

  Worker( void ) : m_bRunning( false ), m_pThread( 0 ) {};
  Worker( OnActionHandler f ) : m_bRunning( true ), m_pThread( 0 ) {
    m_OnAction = f;
    m_pThread = new boost::thread( boost::ref( *this ) );
  }
  ~Worker(void) {
    delete m_pThread; 
  }

  void Run( OnActionHandler f ) {
    assert( !m_bRunning );
    m_bRunning = true;
    m_OnAction = f;
    if ( 0 != m_pThread ) delete m_pThread;
    m_pThread = 0;
    m_pThread = new boost::thread( boost::ref( *this ) );
  }
  void Join( void ) { if ( 0 != m_pThread ) m_pThread->join(); };

  void operator()( void ) {  // runs in alternate thread
    if ( 0 != m_OnAction ) m_OnAction();
    m_bRunning = false;
  }

protected:
private:

  bool m_bRunning;

  OnActionHandler m_OnAction;
  
  boost::thread* m_pThread;
  
};

} // namespace action

namespace function {

// having problems make this work

//1>Phi2.cpp(333): warning C4930: 'ou::function::Worker<Function> worker(AppPhi::HandleMenuAction1LoadIQFeedSymbolList::Run)': prototyped function not called (was a variable definition intended?)
//1>          with
//1>          [
//1>              Function=AppPhi::HandleMenuAction1LoadIQFeedSymbolList::Run
//1>          ]
//1>Phi2.cpp(333): warning C4101: 'worker' : unreferenced local variable
template<typename Function>
class Worker {
public:

  Worker( Function f ) {
    m_f = f;
    m_pThread = new boost::thread( boost::ref( *this ) );
  }
  ~Worker(void) {
    delete m_pThread; 
  }

  void Join( void ) { m_pThread->join(); };

  void operator()( void ) {  // runs in alternate thread
    m_f();
  }

protected:
private:

  Function m_f;

  boost::thread* m_pThread;
  
};

} // namespace function

} // namespace ou
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

// common code used in Clients of CNetwork.
// examples are CIQFeedNews, CIQFeed

// document what goes into each message.  Some are empty all the time.
// Once documented, then the CRTP calls could be turned into calls with fewer parameters for some speed gain

#include <boost/thread/barrier.hpp>

#include <codeproject/thread.h>  // class inbound messages

#include "Network.h"

template <typename T>
class CNetworkClientSkeleton: public CGuiThreadImpl<CNetworkClientSkeleton<T> > {
public:

  typedef typename CNetwork<CNetworkClientSkeleton<T> > network_t;
  typedef typename network_t::port_t port_t;
  typedef typename network_t::ipaddress_t ipaddress_t;
  typedef typename network_t::linebuffer_t linebuffer_t;

  enum enumSend {
    SEND_AND_FORWARD,
    SEND_AND_NO_FORWARD
  };

  CNetworkClientSkeleton( CAppModule* pModule );
  CNetworkClientSkeleton( CAppModule* pModule, const ipaddress_t&, port_t );
  ~CNetworkClientSkeleton( void );

  BOOL InitializeThread( void );
  void CleanupThread( DWORD );

  void Connect( void );
  void Disconnect( void );
  void Send( const std::string& send, enumSend eSend = SEND_AND_FORWARD );  // for internal origination, set to SEND_AND_NO_FORWARD
  void SetPort( port_t port ) { m_connParameters.nPort = port; };
  void SetAddress( const ipaddress_t& ipaddress ) { m_connParameters.sAddress = ipaddress; };

protected:

  enum enumMessages {
    // called by CNetwork
    WM_NETWORK_INITIALIZED = WM_USER + 1,
    WM_NETWORK_CLOSED,
    WM_NETWORK_CONNECTED,
    WM_NETWORK_DISCONNECTED,
    WM_NETWORK_PROCESS,  // network has provided a buffer to process
    WM_NETWORK_SENDDONE,  // when network has completed sending the command
    WM_NETWORK_ERROR,

    // called by derived method calls to cross the thread boundary
    WM_NCS_METHOD_CONNECT,
    WM_NCS_METHOD_DISCONNECT,
    WM_NCS_METHOD_SEND,

    // get everything closed down prior to PostQuitMessage
    WM_NCS_PRE_QUIT,

    // unused here, derived can use it for start of its messages
    WM_NCS_ENDMARKER  
  };

  BEGIN_MSG_MAP_EX(CNetworkClientSkeleton<T>)
    MESSAGE_HANDLER( WM_NETWORK_INITIALIZED, OnConnInitialized )
    MESSAGE_HANDLER( WM_NETWORK_CLOSED, OnConnClosed )
    MESSAGE_HANDLER( WM_NETWORK_CONNECTED, OnConnConnected )
    MESSAGE_HANDLER( WM_NETWORK_DISCONNECTED, OnConnDisconnected )
    MESSAGE_HANDLER( WM_NETWORK_PROCESS, OnConnProcess )
    MESSAGE_HANDLER( WM_NETWORK_SENDDONE, OnConnSendDone )
    MESSAGE_HANDLER( WM_NETWORK_ERROR, OnConnError )

    MESSAGE_HANDLER( WM_NCS_METHOD_CONNECT, OnMethodConnect )
    MESSAGE_HANDLER( WM_NCS_METHOD_DISCONNECT, OnMethodDisconnect )
    MESSAGE_HANDLER( WM_NCS_METHOD_SEND, OnMethodSend )

    MESSAGE_HANDLER( WM_NCS_PRE_QUIT, OnPreQuit )
  END_MSG_MAP()

  LRESULT OnConnInitialized( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnClosed( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnConnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnDisconnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnProcess( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnSendDone( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnError( UINT, WPARAM, LPARAM, BOOL &bHandled );

  LRESULT OnMethodConnect( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnMethodDisconnect( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnMethodSend( UINT, WPARAM, LPARAM, BOOL &bHandled );

  LRESULT OnPreQuit( UINT, WPARAM, LPARAM, BOOL &bHandled );

  void PostMessage( UINT id, WPARAM wparam = NULL, LPARAM lparam = NULL ) {
    PostThreadMessage( id, wparam, lparam );
  }
  void ReturnLineBuffer( WPARAM wParam ) {
    m_pNetworkConnection->PostThreadMessage( network_t::WM_NETWORK_PROCESSED, wParam );
  }

  typename network_t* m_pNetworkConnection;
  typename network_t::structConnection m_connParameters;
  typename network_t::structMessages m_NetworkMessageIDs;
  typename network_t::linerepository_t m_sendbuffers;

private:
  enum enumConnectionState {
    CS_INITIALIZING = 0,  // never used, just supplies a 0
    CS_QUIESCENT,  // no worker thread
    CS_CONNECTING,  // new connection, waiting for network initialization
    CS_CONNECTED,  // connected and ready for operation
    CS_DISCONNECTING  // initiated disconnect
  };
  volatile enumConnectionState m_stateConnection;

  CAppModule* m_pModule;

  boost::barrier m_barrier;  

};

template <typename T>
CNetworkClientSkeleton<T>::CNetworkClientSkeleton(WTL::CAppModule *pModule) 
: CGuiThreadImpl<CNetworkClientSkeleton<T> >( pModule, CREATE_SUSPENDED ),
  m_barrier( 2 ),
  m_pModule( pModule ),
  m_NetworkMessageIDs( this,
    WM_NETWORK_INITIALIZED, WM_NETWORK_CLOSED, WM_NETWORK_CONNECTED,
    WM_NETWORK_DISCONNECTED, WM_NETWORK_PROCESS, WM_NETWORK_SENDDONE, WM_NETWORK_ERROR ),
  m_connParameters( "127.0.0.1", 0 )  
{
  this->Resume();
  m_barrier.wait();  // sync up with InitializeThread
}

template <typename T>
CNetworkClientSkeleton<T>::CNetworkClientSkeleton(
  WTL::CAppModule *pModule, const ipaddress_t& ipaddress, port_t port
  ) 
: CGuiThreadImpl<CNetworkClientSkeleton<T> >( pModule, CREATE_SUSPENDED ),
  m_barrier( 2 ),
  m_pModule( pModule ),
  m_NetworkMessageIDs( this,
    WM_NETWORK_INITIALIZED, WM_NETWORK_CLOSED, WM_NETWORK_CONNECTED,
    WM_NETWORK_DISCONNECTED, WM_NETWORK_PROCESS, WM_NETWORK_SENDDONE, WM_NETWORK_ERROR ),
  m_connParameters( ipaddress, port )  
{
  this->Resume();
  m_barrier.wait();  // sync up with InitializeThread
}

template <typename T>
CNetworkClientSkeleton<T>::~CNetworkClientSkeleton() {
  assert( CS_CONNECTED != m_stateConnection );
  PostThreadMessage( WM_NCS_PRE_QUIT );
  Join();
}

template <typename T>
BOOL CNetworkClientSkeleton<T>::InitializeThread( void ) {

  BOOL b = TRUE;
  m_stateConnection = CS_QUIESCENT;  // thread has been initialized

  m_barrier.wait();  // sync up with constructor

  T* pT = static_cast<T*>( this );
  if ( &CNetworkClientSkeleton<T>::InitializeThread != &T::InitializeThread ) {
    b = pT->InitializeThread();
  }

  return b;
}

template <typename T>
LRESULT CNetworkClientSkeleton<T>::OnPreQuit( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
  if ( CS_QUIESCENT == m_stateConnection ) {
    PostQuitMessage();
  }
  else {
    Sleep(10);
    PostThreadMessage( WM_NCS_PRE_QUIT ); // dally for a bit
  }
  bHandled = true;
  return 1;
}

template <typename T>
void CNetworkClientSkeleton<T>::CleanupThread( DWORD dw ) {

  T* pT = static_cast<T*>( this );
  if ( &CNetworkClientSkeleton<T>::CleanupThread != &T::CleanupThread ) {
    pT->CleanupThread( dw );
  }

  m_stateConnection = CS_QUIESCENT;
}

template <typename T>
void CNetworkClientSkeleton<T>::Connect( void ) { // post a local message, register needs to be in other thread

#ifdef _DEBUG
  std::stringstream ss;
  ss << typeid( this ).name() << "::Connect:  " << std::endl;
  OutputDebugString( ss.str().c_str() );
  ss.str() = "";
#endif

  if ( CS_QUIESCENT == m_stateConnection ) {
    m_stateConnection = CS_CONNECTING;
    this->PostThreadMessage( WM_NCS_METHOD_CONNECT );
  }
  else {
    throw std::logic_error( "CNetworkClientSkeleton::Connect not in disconnected state" );
  }
}

template <typename T>
void CNetworkClientSkeleton<T>::Disconnect( void ) {  
  switch ( m_stateConnection ) {
    case CS_CONNECTED: 
      m_stateConnection = CS_DISCONNECTING;
      PostThreadMessage( WM_NCS_METHOD_DISCONNECT );
      break;
    case CS_QUIESCENT:
      // nothing has started so don't do anything
      // rather we need to send an 'all ok' message somewhere
      break;
    default:
      throw std::logic_error( "CNetworkClientSkeleton::Disconnect not in connected state" );
  }
}

template <typename T>
void CNetworkClientSkeleton<T>::Send( const std::string& send, enumSend eSend ) {

  // need to do this pause differently, maybe add WaitToSend message, but need message queue to be initialized first

#define COUNT 1000

#ifdef _DEBUG
  std::stringstream ss;
  ss << typeid( this ).name() << "::Send:  " << std::endl;
  OutputDebugString( ss.str().c_str() );
  ss.str() = "";
#endif

  unsigned int cnt = COUNT;  // wait up to a second
  while ( CS_CONNECTING == m_stateConnection ) {
    --cnt;
    if ( 0 == cnt ) {
      throw std::logic_error( "CNetworkClientSkeleton::Send time out" );
    }
    else {
      Sleep( 10 );  // wait for the connection to be made
    }
  }

#ifdef _DEBUG
  if ( COUNT != cnt ) {
    ss << typeid( this ).name() << "::Send Waited " << COUNT - cnt << " times." << std::endl;
    OutputDebugString( ss.str().c_str() );
    ss.str() = "";
  }
#endif

  if ( CS_CONNECTED == m_stateConnection ) {
    CNetwork<CNetworkClientSkeleton<T> >::linebuffer_t* psendbuffer = m_sendbuffers.CheckOut();
    psendbuffer->clear();
    BOOST_FOREACH( char ch, send ) {
      (*psendbuffer).push_back( ch );
    }
    PostThreadMessage( WM_NCS_METHOD_SEND, 
                       static_cast<WPARAM>( eSend ), 
                       reinterpret_cast<LPARAM>( psendbuffer ) );
  }
  else {
    throw std::logic_error( "CNetworkClientSkeleton::Send not in connected state" );
  }
}

template <typename T>
LRESULT CNetworkClientSkeleton<T>::OnConnInitialized( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {


#ifdef _DEBUG
  std::stringstream ss;
  ss << typeid( this ).name() << "::OnConnInitialized:  " << std::endl;
  OutputDebugString( ss.str().c_str() );
  ss.str() = "";
#endif

  m_pNetworkConnection->PostThreadMessage( 
    network_t::WM_NETWORK_CONNECT, reinterpret_cast<WPARAM>( &m_connParameters ) );

  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CNetworkClientSkeleton<T>::OnConnClosed( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CNetworkClientSkeleton<T>::OnConnConnected( UINT id, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {


#ifdef _DEBUG
  std::stringstream ss;
  ss << typeid( this ).name() << "::OnConnConnected:  " << std::endl;
  OutputDebugString( ss.str().c_str() );
  ss.str() = "";
#endif

  BOOL b = TRUE;
  m_stateConnection = CS_CONNECTED;

  T* pT = static_cast<T*>( this );
  if ( &CNetworkClientSkeleton<T>::OnConnConnected != &T::OnConnConnected ) {
    b = pT->OnConnConnected( id, wParam, lParam, bHandled );
  }

  bHandled = true;
  return b;
}

template <typename T>
LRESULT CNetworkClientSkeleton<T>::OnConnDisconnected( UINT id, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  BOOL b = TRUE;
  m_stateConnection = CS_QUIESCENT;

  T* pT = static_cast<T*>( this );
  if ( &CNetworkClientSkeleton<T>::OnConnDisconnected != &T::OnConnDisconnected ) {
    b = pT->OnConnDisconnected( id, wParam, lParam, bHandled );
  }

  delete m_pNetworkConnection;
  m_pNetworkConnection = NULL;

  bHandled = true;
  return b;
}

template <typename T>
LRESULT CNetworkClientSkeleton<T>::OnConnProcess( UINT id, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  BOOL b = TRUE;

  T* pT = static_cast<T*>( this );
  if ( &CNetworkClientSkeleton<T>::OnConnProcess != &T::OnConnProcess ) {
    b = pT->OnConnProcess( id, wParam, lParam, bHandled );
  }

  bHandled = true;
  return b;
}

template <typename T>
LRESULT CNetworkClientSkeleton<T>::OnConnSendDone( UINT id, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  BOOL b = TRUE;

  network_t::linebuffer_t* linebuffer 
    = reinterpret_cast<network_t::linebuffer_t*>( lParam );
  m_sendbuffers.CheckIn( linebuffer );

  enumSend eForwardSendDone = static_cast<enumSend>( wParam );
  if ( SEND_AND_FORWARD == eForwardSendDone ) {
    T* pT = static_cast<T*>( this );
    if ( &CNetworkClientSkeleton<T>::OnConnSendDone != &T::OnConnSendDone ) {
      b = pT->OnConnSendDone( id, wParam, lParam, bHandled );
    }
  }

  bHandled = true;
  return b;
}

template <typename T>
LRESULT CNetworkClientSkeleton<T>::OnConnError( UINT id, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  BOOL b = TRUE;

  OutputDebugString( "CNetworkClientSkeleton::OnConnError error" );

  T* pT = static_cast<T*>( this );
  if ( &CNetworkClientSkeleton<T>::OnConnError != &T::OnConnError ) {
    b = pT->OnConnError( id, wParam, lParam, bHandled );
  }

  bHandled = true;
  return b;
}

template <typename T>
LRESULT CNetworkClientSkeleton<T>::OnMethodConnect( UINT, WPARAM, LPARAM, BOOL &bHandled ) {


#ifdef _DEBUG
  std::stringstream ss;
  ss << typeid( this ).name() << "::OnMethodConnect:  " << std::endl;
  OutputDebugString( ss.str().c_str() );
  ss.str() = "";
#endif

  m_pNetworkConnection = new CNetwork<CNetworkClientSkeleton<T> >( m_pModule, m_NetworkMessageIDs );

  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CNetworkClientSkeleton<T>::OnMethodDisconnect( UINT, WPARAM, LPARAM, BOOL &bHandled ) {

  if ( m_sendbuffers.Outstanding() ) {  // wait to finish off what ever we were sending
    Sleep(10);
    PostThreadMessage( WM_NCS_METHOD_DISCONNECT );
  }
  else {
    m_pNetworkConnection->PostThreadMessage( CNetwork<CNetworkClientSkeleton<T> >::WM_NETWORK_DISCONNECT );
  }

  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CNetworkClientSkeleton<T>::OnMethodSend( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  m_pNetworkConnection->PostThreadMessage( network_t::WM_NETWORK_SEND, wParam, lParam );

  bHandled = true;
  return 1;
}


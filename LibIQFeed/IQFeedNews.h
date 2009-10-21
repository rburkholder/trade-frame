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

// accepts headlines
// matches up story text, calculates hashes to reduce redundant stories
// performs some emotional scanning
// handles refresh of 'sources' list

#include <codeproject/thread.h>  // class inbound messages

#include <LibWtlCommon/Network.h>

#include <LibCommon/ReusableBuffers.h>

template <class ownerT>
class CIQFeedNews: public CGuiThreadImpl<CIQFeedNews<ownerT> > {
public:
  CIQFeedNews(CAppModule* pModule);
  ~CIQFeedNews(void );
protected:

  enumPrivateMessageTypes { // messages from CNetwork
    // called by CNetwork
    WM_CONN_INITIALIZED = WM_USER + 1,
    WM_CONN_CLOSED,
    WM_CONN_CONNECTED,
    WM_CONN_DISCONNECTED,
    WM_CONN_PROCESS,
    WM_CONN_SENDDONE,
    WM_CONN_ERROR, 
    // called by derived method calls and cross thread boundary
    WM_METHOD_CONNECT,
    WM_METHOD_DISCONNECT,
    // get everything closed down prior to PostQuitMessage
    WM_PRE_QUIT
  };

  BEGIN_MSG_MAP_EX(CIQFeedNews<ownerT>)
    MESSAGE_HANDLER( WM_CONN_INITIALIZED, OnConnInitialized );
    MESSAGE_HANDLER( WM_CONN_CLOSED, OnConnClosed );
    MESSAGE_HANDLER( WM_CONN_CONNECTED, OnConnConnected );
    MESSAGE_HANDLER( WM_CONN_DISCONNECTED, OnConnDisconnected );
    MESSAGE_HANDLER( WM_CONN_PROCESS, OnConnProcess );
    MESSAGE_HANDLER( WM_CONN_SENDDONE, OnConnSendDone );
    MESSAGE_HANDLER( WM_CONN_ERROR, OnConnError );

    MESSAGE_HANDLER( WM_METHOD_CONNECT, OnMethodConnect );
    MESSAGE_HANDLER( WM_METHOD_DISCONNECT, OnMethodDisconnect );

    MESSAGE_HANDLER( WM_PRE_QUIT, OnPreQuit );
   END_MSG_MAP()

    
  BOOL InitializeThread( void );
  void CleanupThread( DWORD );

  LRESULT OnConnInitialized( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnClosed( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnConnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnDisconnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnProcess( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnSendDone( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnError( UINT, WPARAM, LPARAM, BOOL &bHandled );

  LRESULT OnMethodConnect( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnMethodDisconnect( UINT, WPARAM, LPARAM, BOOL &bHandled );

  LRESULT OnPreQuit( UINT, WPARAM, LPARAM, BOOL &bHandled );

private:

  enum enumConnectionState {
    CS_QUIESCENT,
    CS_INITIALIZING,
    CS_DISCONNECTED,
    CS_CONNECTING,
    CS_CONNECTED,
    CS_DISCONNECTING
  } m_stateConnection;

  CAppModule* m_pModule;

  typename CNetwork<CIQFeedNews<ownerT> >* m_pconn9100;  // news lookup port
  typename CNetwork<CIQFeedNews<ownerT> >::structConnection m_connParameters;
  typename CNetwork<CIQFeedNews<ownerT> >::linerepository_t m_sendbuffers;
  typename CNetwork<CIQFeedNews<ownerT> >::structMessages m_NetworkMessages;

}

template <class ownerT>
CIQFeedNews::CIQFeedNews(WTL::CAppModule *pModule) 
: m_pModule( pModule ),
  m_stateConnection( CS_QUIESCENT ),
  m_connParameters( "127.0.0.1", 9100 ),
  m_NetworkMessages( this,
    WM_CONN_INITIALIZED, WM_CONN_CLOSED, WM_CONN_CONNECTED,
    WM_CONN_DISCONNECTED, WM_CONN_PROCESS, WM_CONN_SENDDONE, WM_CONN_ERROR ),
  CGuiThreadImpl<CIQFeedNews<ownerT> >( pModule )
{
}

template <class ownerT>
CIQFeedNews::~CIQFeedNews() {
  assert( CS_CONNECTED != m_stateConnection );
  PostThreadMessage( WM_PRE_QUIT );
  Join();
}

template <class ownerT>
BOOL CIQFeedNews<ownerT>::InitializeThread( void ) {

  m_stateConnection = CS_DISCONNECTED;

  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgInitialized );

  return TRUE;
}

template <class ownerT>
LRESULT CIQFeedNews<ownerT>::OnPreQuit( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
  if ( CS_DISCONNECTED == m_stateConnection ) { // might also check all buffers have been emptied
    PostQuitMessage();
  }
  else {
    Sleep(10);
    PostThreadMessage( WM_PRE_QUIT ); // dally for a bit until network disconnects
  }
  bHandled = true;
  return 1;
}

template <class ownerT>
void CIQFeedNews<ownerT>::CleanupThread( DWORD dw ) {
  m_stateConnection = CS_QUIESCENT;
}

template <class ownerT>
void CIQFeedNews<ownerT>::Connect( void ) { // post a local message, register needs to be in other thread
  if ( CS_DISCONNECTED == m_stateConnection ) {
    m_stateConnection = CS_CONNECTING;
    PostThreadMessage( WM_IQFEED_METHOD_CONNECT );
  }
  else {
    throw std::logic_error( "CIQFeedNews::Connect not in disconnected state" );
  }
}

template <class ownerT>
void CIQFeedNews<ownerT>::Disconnect( void ) {  
  if ( CS_CONNECTED == m_stateConnection ) {
    m_stateConnection = CS_DISCONNECTING;
    PostThreadMessage( WM_IQFEED_METHOD_DISCONNECT );

  }
  else {
    throw std::logic_error( "CIQFeedNews::Disconnect not in connected state" );
  }
}

template <class ownerT>
LRESULT CIQFeedNews<ownerT>::OnMethodConnect( UINT, WPARAM, LPARAM, BOOL &bHandled ) {

  m_pconn9100 = new CNetwork<CIQFeedNews<ownerT> >( m_pModule, m_NetworkMessages );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeedNews<ownerT>::OnMethodDisconnect( UINT, WPARAM, LPARAM, BOOL &bHandled ) {

  m_pconn9100->PostThreadMessage( CNetwork<CIQFeedNews<ownerT> >::WM_NETWORK_DISCONNECT );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeedNews<ownerT>::OnConnInitialized( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  m_pconn9100->PostThreadMessage( 
    CNetwork<CIQFeedNews<ownerT> >::WM_NETWORK_CONNECT, reinterpret_cast<WPARAM>( &m_connParameters ) );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeedNews<ownerT>::OnConnClosed( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeedNews<ownerT>::OnConnConnected( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  m_stateConnection = CS_CONNECTED;

  //m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgConnected );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeedNews<ownerT>::OnConnDisconnected( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  delete m_pconn9100;
  m_pconn9100 = NULL;

  m_stateConnection = CS_DISCONNECTED;

  //m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgDisconnected );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeedNews<ownerT>::OnConnProcess( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  CNetwork<CIQFeedNews<ownerT> >::linebuffer_t* buf = reinterpret_cast<CNetwork<CIQFeedNews>::linebuffer_t*>( wParam );
  CNetwork<CIQFeedNews<ownerT> >::linebuffer_t::iterator iter = (*buf).begin();
  CNetwork<CIQFeedNews<ownerT> >::linebuffer_t::iterator end = (*buf).end();

#if defined _DEBUG
  std::string str( iter, end );
  str += '\n';
  OutputDebugString( str.c_str() );
#endif

  BOOST_ASSERT( iter != end );

  // code here

  m_pconn9100->PostThreadMessage( CNetwork<CIQFeedNews<ownerT> >::WM_NETWORK_PROCESSED, wParam );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeedNews<ownerT>::OnConnSendDone( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  CNetwork<CIQFeedNews<ownerT> >::linebuffer_t* linebuffer = reinterpret_cast<CNetwork<CIQFeedNews<ownerT> >::linebuffer_t*>( lParam );
  m_sendbuffers.CheckIn( linebuffer );

  enumSend eForwardSendDone = static_cast<enumSend>( wParam );
  if ( SEND_AND_FORWARD == eForwardSendDone ) {
//    m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgSendComplete );
  }

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeedNews<ownerT>::OnConnError( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  OutputDebugString( "CIQFeedNews::OnConnError error" );

  bHandled = true;
  return 1;
}


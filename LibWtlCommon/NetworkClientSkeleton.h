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

#include <codeproject/thread.h>  // class inbound messages

#include "Network.h"

template <class ownerT>
class CNetworkClientSkeleton: public CGuiThreadImpl<CNetworkClientSkeleton<ownerT> > {
public:

  CNetworkClientSkeleton( CAppModule* pModule );
  ~CNetworkClientSkeleton( void );

  BOOL InitializeThread( void );
  void CleanupThread( DWORD );

  void Connect( void );
  void Disconnect( void );
  void Send( const std::string& send, enumSend eSend = SEND_AND_FORWARD );  // for internal origination, set to false

protected:

  enum enumSend {
    SEND_AND_FORWARD,
    SEND_AND_NO_FORWARD
  };

  enum enumMessages {
    // called by CNetwork
    WM_NETWORK_INITIALIZED = WM_USER + 1,
    WM_NETWORK_CLOSED,
    WM_NETWORK_CONNECTED,
    WM_NETWORK_DISCONNECTED,
    WM_NETWORK_PROCESS,
    WM_NETWORK_SENDDONE,
    WM_NETWORK_ERROR,

    // called by derived method calls to cross the thread boundary
    WM_NCS_METHOD_CONNECT,
    WM_NCS_METHOD_DISCONNECT,
    WM_NCS_METHOD_SEND,

    // get everything closed down prior to PostQuitMessage
    WM_NCS_PRE_QUIT

    // unused here, derived can use it for start of its messages
    WM_NCS_ENDMARKER  
  };

  BEGIN_MSG_MAP_EX(CNetworkClientSkeleton<ownerT>)
    MESSAGE_HANDLER( WM_NETWORK_INITIALIZED, OnConnInitialized );
    MESSAGE_HANDLER( WM_NETWORK_CLOSED, OnConnClosed );
    MESSAGE_HANDLER( WM_NETWORK_CONNECTED, OnConnConnected );
    MESSAGE_HANDLER( WM_NETWORK_DISCONNECTED, OnConnDisconnected );
    MESSAGE_HANDLER( WM_NETWORK_PROCESS, OnConnProcess );
    MESSAGE_HANDLER( WM_NETWORK_SENDDONE, OnConnSendDone );
    MESSAGE_HANDLER( WM_NETWORK_ERROR, OnConnError );

    MESSAGE_HANDLER( WM_NCS_METHOD_CONNECT, OnMethodConnect );
    MESSAGE_HANDLER( WM_NCS_METHOD_DISCONNECT, OnMethodDisconnect );
    MESSAGE_HANDLER( WM_NCS_METHOD_SEND, OnMethodSend );

    MESSAGE_HANDLER( WM_NCS_PRE_QUIT, OnPreQuit );
  END_MSG_MAP

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

private:
  enum enumConnectionState {
    CS_QUIESCENT,  // no worker thread
    CS_INITIALIZING,
    CS_CONNECTING,  // new connection, waiting for networkinitialization
    CS_CONNECTED,  // connected and ready for operation
    CS_DISCONNECTING  // initiated disconnect
  } m_stateConnection;

  CAppModule* m_pModule;

  typename CNetwork<CNetworkClientSkeleton<ownerT> >* m_pNetworkConnection;
  typename CNetwork<CNetworkClientSkeleton<ownerT> >::structConnection m_connParameters;
  typename CNetwork<CNetworkClientSkeleton<ownerT> >::linerepository_t m_sendbuffers;
  typename CNetwork<CNetworkClientSkeleton<ownerT> >::structMessages m_NetworkMessageIDs;
};

template <class ownerT>
CNetworkClientSkeleton<ownerT>::CNetworkClientSkeleton(WTL::CAppModule *pModule) 
: m_pModule( pModule ),
  m_stateConnection( CS_QUIESCENT ),
  m_NetworkMessageIDs( this,
    WM_NETWORK_INITIALIZED, WM_NETWORK_CLOSED, WM_NETWORK_CONNECTED,
    WM_NETWORK_DISCONNECTED, WM_NETWORK_PROCESS, WM_NETWORK_SENDDONE, WM_NETWORK_ERROR ),
//  m_connParameters( "127.0.0.1", 9100 ),
  CGuiThreadImpl<CNetworkClientSkeleton<ownerT> >( pModule )
{
}

template <class ownerT>
CNetworkClientSkeleton<ownerT>::~CNetworkClientSkeleton() {
//  assert( CS_CONNECTED != m_stateConnection );
//  PostThreadMessage( WM_PRE_QUIT );
//  Join();
}

template <class ownerT>
BOOL CNetworkClientSkeleton<ownerT>::InitializeThread( void ) {

  static_cast<ownerT*>(this)->InitializeThread();

//  m_stateConnection = CS_DISCONNECTED;

//  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgInitialized );

  return TRUE;
}

template <class ownerT>
void CNetworkClientSkeleton<ownerT>::CleanupThread( DWORD dw ) {
  static_cast<ownerT*>(this)->CleanupThread(dw);
//  m_stateConnection = CS_QUIESCENT;
}



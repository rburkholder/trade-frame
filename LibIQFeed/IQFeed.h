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

#include <string>

#include <codeproject/thread.h>  // class inbound messages

#include <LibWtlCommon/Network.h>

class CIQFeed: public CGuiThreadImpl<CIQFeed> {
public:

  CIQFeed(CAppModule* pModule);
  ~CIQFeed(void);

  void Connect( void );
  void Disconnect( void );
  void Send( const std::string& send );

protected:

  enum enumPrivateMessageTypes {  // messages from CNetwork
    // called by CNetwork
    WM_IQFEED_CONN_INITIALIZED = WM_USER + 1,
    WM_IQFEED_CONN_CLOSED,
    WM_IQFEED_CONN_CONNECTED,
    WM_IQFEED_CONN_DISCONNECTED,
    WM_IQFEED_CONN_PROCESS,
    WM_IQFEED_CONN_SENDDONE,
    WM_IQFEED_CONN_ERROR, 
    // called by derived method calls and cross thread boundary
    WM_IQFEED_METHOD_CONNECT,
    WM_IQFEED_METHOD_DISCONNECT,
    WM_IQFEED_METHOD_SEND
  };

  BEGIN_MSG_MAP_EX(CIQFeed)
    MESSAGE_HANDLER( WM_IQFEED_CONN_INITIALIZED, OnConnInitialized );
    MESSAGE_HANDLER( WM_IQFEED_CONN_CLOSED, OnConnClosed );
    MESSAGE_HANDLER( WM_IQFEED_CONN_CONNECTED, OnConnConnected );
    MESSAGE_HANDLER( WM_IQFEED_CONN_DISCONNECTED, OnConnDisconnected );
    MESSAGE_HANDLER( WM_IQFEED_CONN_PROCESS, OnConnProcess );
    MESSAGE_HANDLER( WM_IQFEED_CONN_SENDDONE, OnConnSendDone );
    MESSAGE_HANDLER( WM_IQFEED_CONN_ERROR, OnConnError );
  END_MSG_MAP()

  LRESULT OnConnInitialized( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnClosed( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnConnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnDisconnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnProcess( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnSendDone( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnError( UINT, WPARAM, LPARAM, BOOL &bHandled );

  LRESULT OnMethodConnect( );
  LRESULT OnMethodDisconnect( );
  LRESULT OnMethodSend( );

  BOOL InitializeThread( void );
  void CleanupThread( DWORD );

  CNetwork<CIQFeed>::structMessages m_NetworkMessages;

  void PostMessage( UINT id, WPARAM wparam = NULL, LPARAM lparam = NULL ) {
    PostThreadMessage( id, wparam, lparam );
  }
private:
  CAppModule* m_pModule;
  CNetwork<CIQFeed>::structConnection m_connParameters;
  CNetwork<CIQFeed>* m_pconnIQFeed;

};

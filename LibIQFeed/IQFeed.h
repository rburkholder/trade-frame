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
#include <sstream>
#include <exception>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>

#include <codeproject/thread.h>  // class inbound messages

#include <LibWtlCommon/Network.h>

#include <LibCommon/ReusableBuffers.h>

#include "IQ32.H"
#include "IQFeedMessages.h"

void __stdcall IQFeedCallBack( int x, int y );

template <class ownerT>
class CIQFeed: public CGuiThreadImpl<CIQFeed<ownerT> > {
public:

  // pre-initialized message ids for messages delivered to and accepted by external caller
  struct structMessageDestinations { // 
    ownerT* owner;        // owner object to which message is sent (PostMessage needs to be implemented)
    UINT msgInitialized;
    UINT msgConnected;
    UINT msgSendComplete;
    UINT msgDisconnected;
    UINT msgError;
    structMessageDestinations( void )
      : owner( NULL ), msgInitialized( 0 ), msgConnected( 0 ), msgSendComplete( 0 ), msgDisconnected( 0 ), msgError( 0 )
      {};
    structMessageDestinations( ownerT* owner_, UINT msgInitialized_, UINT msgConnected_, UINT msgSendComplete_, UINT msgDisconnected_, UINT msgError_ ) 
      : owner( owner_ ), msgInitialized( msgInitialized_ ), msgConnected( msgConnected_ ), msgSendComplete( msgSendComplete_ ), msgDisconnected( msgDisconnected_ ), msgError( msgError_ )
      {};
  };

  CIQFeed(CAppModule* pModule, const structMessageDestinations&);
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

  BEGIN_MSG_MAP_EX(CIQFeed<ownerT>)
    MESSAGE_HANDLER( WM_IQFEED_CONN_INITIALIZED, OnConnInitialized );
    MESSAGE_HANDLER( WM_IQFEED_CONN_CLOSED, OnConnClosed );
    MESSAGE_HANDLER( WM_IQFEED_CONN_CONNECTED, OnConnConnected );
    MESSAGE_HANDLER( WM_IQFEED_CONN_DISCONNECTED, OnConnDisconnected );
    MESSAGE_HANDLER( WM_IQFEED_CONN_PROCESS, OnConnProcess );
    MESSAGE_HANDLER( WM_IQFEED_CONN_SENDDONE, OnConnSendDone );
    MESSAGE_HANDLER( WM_IQFEED_CONN_ERROR, OnConnError );

    MESSAGE_HANDLER( WM_IQFEED_METHOD_CONNECT, OnMethodConnect );
    MESSAGE_HANDLER( WM_IQFEED_METHOD_DISCONNECT, OnMethodDisconnect );
    MESSAGE_HANDLER( WM_IQFEED_METHOD_SEND, OnMethodSend );
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

  BOOL InitializeThread( void );
  void CleanupThread( DWORD );

  typename CNetwork<typename CIQFeed<ownerT> >::structMessages m_NetworkMessages;

  void PostMessage( UINT id, WPARAM wparam = NULL, LPARAM lparam = NULL ) {
    PostThreadMessage( id, wparam, lparam );
  }
private:

  enum enumConnectionState {
    CS_INITIALIZING,
    CS_DISCONNECTED,
    CS_CONNECTING,
    CS_CONNECTED,
    CS_DISCONNECTING
  };
  enumConnectionState m_stateConnection;

  structMessageDestinations m_structMessageDestinations;

  CAppModule* m_pModule;
  typename CNetwork<CIQFeed<ownerT> >::structConnection m_connParameters;
  typename CNetwork<CIQFeed<ownerT> >* m_pconnIQFeed;
  typename CNetwork<CIQFeed<ownerT> >::linerepository_t m_sendbuffers;

};

template <class ownerT>
CIQFeed<ownerT>::CIQFeed(CAppModule* pModule, const structMessageDestinations& MessageDestinations ) 
: CGuiThreadImpl<CIQFeed<ownerT> >( pModule ), m_pModule( pModule ),
  m_structMessageDestinations( MessageDestinations ),
  m_connParameters( "127.0.0.1", 5009 ),
  m_stateConnection( CS_INITIALIZING ),
  m_NetworkMessages( this,
    WM_IQFEED_CONN_INITIALIZED, WM_IQFEED_CONN_CLOSED, WM_IQFEED_CONN_CONNECTED,
    WM_IQFEED_CONN_DISCONNECTED, WM_IQFEED_CONN_PROCESS, WM_IQFEED_CONN_SENDDONE, WM_IQFEED_CONN_ERROR )
{
}

template <class ownerT>
CIQFeed<ownerT>::~CIQFeed(void) {
}

template <class ownerT>
BOOL CIQFeed<ownerT>::InitializeThread( void ) {

  m_stateConnection = CS_DISCONNECTED;

  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgInitialized );

  return TRUE;
}

template <class ownerT>
void CIQFeed<ownerT>::CleanupThread( DWORD dw ) {

}

template <class ownerT>
void CIQFeed<ownerT>::Connect( void ) { // post a local message, register needs to be in other thread
  if ( CS_DISCONNECTED == m_stateConnection ) {
    m_stateConnection = CS_CONNECTING;
    PostThreadMessage( WM_IQFEED_METHOD_CONNECT );
  }
  else {
    throw std::logic_error( "CIQFeed::Connect not in disconnected state" );
  }
}

template <class ownerT>
void CIQFeed<ownerT>::Send( const std::string& send ) {
  if ( CS_CONNECTED == m_stateConnection ) {
    CNetwork<CIQFeed<ownerT> >::linebuffer_t* psendbuffer = m_sendbuffers.CheckOut();
    psendbuffer->clear();
    BOOST_FOREACH( char ch, send ) {
      (*psendbuffer).push_back( ch );
    }
    PostThreadMessage( WM_IQFEED_METHOD_SEND, reinterpret_cast<WPARAM>( psendbuffer ) );
  }
  else {
    throw std::logic_error( "CIQFeed::Send not in connected state" );
  }
}

template <class ownerT>
void CIQFeed<ownerT>::Disconnect( void ) {  
  if ( CS_CONNECTED == m_stateConnection ) {
    m_stateConnection = CS_DISCONNECTING;
    PostThreadMessage( WM_IQFEED_METHOD_DISCONNECT );

  }
  else {
    throw std::logic_error( "CIQFeed::Disconnect not in connected state" );
  }
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnMethodConnect( UINT, WPARAM, LPARAM, BOOL &bHandled ) {

  SetCallbackFunction( &IQFeedCallBack );
  int i = RegisterClientApp( NULL, _T("ONE_UNIFIED"), _T("0.11111111"), _T("2.0") );

  m_pconnIQFeed = new CNetwork<CIQFeed<ownerT> >( m_pModule, m_NetworkMessages );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnMethodDisconnect( UINT, WPARAM, LPARAM, BOOL &bHandled ) {

  m_pconnIQFeed->PostThreadMessage( CNetwork<CIQFeed<ownerT> >::WM_NETWORK_DISCONNECT );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnMethodSend( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  m_pconnIQFeed->PostThreadMessage( CNetwork<CIQFeed<ownerT> >::WM_NETWORK_SEND, wParam );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnConnInitialized( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  m_pconnIQFeed->PostThreadMessage( 
  CNetwork<CIQFeed<ownerT> >::WM_NETWORK_CONNECT, reinterpret_cast<WPARAM>( &m_connParameters ) );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnConnClosed( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnConnConnected( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  m_stateConnection = CS_CONNECTED;

  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgConnected );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnConnDisconnected( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  delete m_pconnIQFeed;

  RemoveClientApp( NULL );

  m_stateConnection = CS_DISCONNECTED;

  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgDisconnected );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnConnProcess( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  CNetwork<CIQFeed<ownerT> >::linebuffer_t* buf = reinterpret_cast<CNetwork<CIQFeed>::linebuffer_t*>( wParam );
  CNetwork<CIQFeed<ownerT> >::linebuffer_t::iterator iter = (*buf).begin();
  CNetwork<CIQFeed<ownerT> >::linebuffer_t::iterator end = (*buf).end();

  BOOST_ASSERT( iter != end );

  switch ( *iter ) {
    case 'Q': 
      {
        CIQFUpdateMessage msg( iter, end );
//        HandleQMessage( &msg );
      }
      break;
    case 'P': 
      {
        CIQFSummaryMessage msg( iter, end );
//        HandlePMessage( &msg );
      }
      break;
    case 'F': 
      {
        CIQFFundamentalMessage msg( iter, end );
//        HandleFMessage( &msg );
      }
      break;
    case 'N': 
      {
        CIQFNewsMessage msg( iter, end );
//        NewsMessage( &msg );
//        HandleNMessage( &msg );

      }
      break;
    case 'T': 
      {
        CIQFTimeMessage msg( iter, end );
//        TimeMessage( &msg );
//        HandleTMessage( &msg );
      }
      break;
    case 'S': 
      {
        CIQFSystemMessage msg( iter, end );
//        msg.EmitLine();
        if ( _T( "KEY" ) == msg.Field( 2 ) ) {
//          CString s;
//          s.Format( "S,KEY,%s\n", msg.Field( 3 ) );
//          IQConnect.SendToSocket( (char*) LPCTSTR( s ) );
//          IQConnect.SendToSocket( "S,NEWSON\n" );
        }
        if ( _T( "CUST" ) == msg.Field( 2 ) ) {
          if ( _T( "4.3.0.3" ) > msg.Field( 7 ) ) {
//            cout << "Need IQFeed version of 4.3.0.3 or greater (" << msg.Field( 7 ) << ")" << endl;
            //throw s;  // can't throw exception, just accept it, as we are getting '2.5.3' as a return
          }
        }
//        HandleSMessage( &msg );
      }
      break;
    default:
      throw "Unknown message type in IQFeed"; // unknown message type
      break;
  }

  m_pconnIQFeed->PostThreadMessage( CNetwork<CIQFeed<ownerT> >::WM_NETWORK_PROCESSED, reinterpret_cast<WPARAM>( buf ) );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnConnSendDone( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  CNetwork<CIQFeed<ownerT> >::linebuffer_t* linebuffer = reinterpret_cast<CNetwork<CIQFeed<ownerT> >::linebuffer_t*>( wParam );
  m_sendbuffers.CheckIn( linebuffer );

  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgSendComplete );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnConnError( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  OutputDebugString( "CIQFeed::OnConnError error" );

  bHandled = true;
  return 1;
}


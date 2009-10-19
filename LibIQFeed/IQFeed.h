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

// custom on
// http://msdn.microsoft.com/en-us/library/e5ewb1h3.aspx
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
// custom off

void __stdcall IQFeedCallBack( int x, int y );

template <class ownerT>
class CIQFeed: public CGuiThreadImpl<CIQFeed<ownerT> > {
public:

  // pre-initialized message ids for messages delivered to and accepted by external caller
  struct structMessageDestinations { // 
    // possibly make use of Boost named parameters here.
    ownerT* owner;        // owner object to which message is sent (PostMessage needs to be implemented)
    UINT msgInitialized;
    UINT msgConnected;
    UINT msgSendComplete;
    UINT msgDisconnected;
    UINT msgError;
    UINT msgMessageUpdate;
    UINT msgMessageSummary;
    UINT msgMessageNews;
    UINT msgMessageFundamental;
    UINT msgMessageTime;
    UINT msgMessageSystem;

    structMessageDestinations( void )
      : owner( NULL ), msgInitialized( 0 ), msgConnected( 0 ), msgSendComplete( 0 ), msgDisconnected( 0 ), msgError( 0 ),
        msgMessageUpdate( 0 ), msgMessageSummary( 0 ), msgMessageNews( 0 ), msgMessageFundamental( 0 ), msgMessageTime( 0 ), msgMessageSystem( 0 )
      {};
    structMessageDestinations( ownerT* owner_, UINT msgInitialized_, UINT msgConnected_, UINT msgSendComplete_, UINT msgDisconnected_, UINT msgError_,
        UINT msgMessageUpdate_, UINT msgMessageSummary_, UINT msgMessageNews_, UINT msgMessageFundamental_, UINT msgMessageTime_, UINT msgMessageSystem_ ) 
      : owner( owner_ ), msgInitialized( msgInitialized_ ), msgConnected( msgConnected_ ), msgSendComplete( msgSendComplete_ ), msgDisconnected( msgDisconnected_ ), msgError( msgError_ ),
        msgMessageUpdate( msgMessageUpdate_ ), msgMessageSummary( msgMessageSummary_ ), msgMessageNews( msgMessageNews_ ),
        msgMessageFundamental( msgMessageFundamental_ ), msgMessageTime( msgMessageTime_ ), msgMessageSystem( msgMessageSystem_ )
      {
        BOOST_ASSERT( NULL != owner_ );
    };
  };

  enum enumSend {
    SEND_AND_FORWARD,
    SEND_AND_NO_FORWARD
  };

  CIQFeed(CAppModule* pModule, const structMessageDestinations&);
  ~CIQFeed(void);

  void Connect( void );
  void Disconnect( void );
  void Send( const std::string& send, enumSend eSend = SEND_AND_FORWARD );  // for internal origination, set to false
  void inline UpdateDone( WPARAM wParam, LPARAM lParam ) {
    PostThreadMessage( WM_IQFEED_DONE_UPDATE, wParam, lParam );
  }
  void inline SummaryDone( WPARAM wParam, LPARAM lParam ) {
    PostThreadMessage( WM_IQFEED_DONE_SUMMARY, wParam, lParam );
  }
  void inline NewsDone( WPARAM wParam, LPARAM lParam ) {
    PostThreadMessage( WM_IQFEED_DONE_NEWS, wParam, lParam );
  }
  void inline FundamentalDone( WPARAM wParam, LPARAM lParam ) {
    PostThreadMessage( WM_IQFEED_DONE_FUNDAMENTAL, wParam, lParam );
  }
  void inline TimeDone( WPARAM wParam, LPARAM lParam ) {
    PostThreadMessage( WM_IQFEED_DONE_TIME, wParam, lParam );
  }
  void inline SystemDone( WPARAM wParam, LPARAM lParam ) {
    PostThreadMessage( WM_IQFEED_DONE_SYSTEM, wParam, lParam );
  }

  void SetNewsOn( void );
  void SetNewsOff( void );

protected:

  enum enumNewsState {
    NEWSISON,
    NEWSISOFF
  } m_stateNews;

  enum enumPrivateMessageTypes {  // messages from CNetwork
    // called by CNetwork
    WM_CONN_INITIALIZED = WM_USER + 1,
    WM_CONN_CLOSED,
    WM_CONN_CONNECTED,
    WM_CONN_DISCONNECTED,
    WM_CONN_PROCESS,
    WM_CONN_SENDDONE,
    WM_CONN_ERROR, 
    // called when derived methods have finished with message and buffer
    WM_IQFEED_DONE_UPDATE,
    WM_IQFEED_DONE_SUMMARY,
    WM_IQFEED_DONE_NEWS,
    WM_IQFEED_DONE_FUNDAMENTAL,
    WM_IQFEED_DONE_TIME,
    WM_IQFEED_DONE_SYSTEM,
    // called by derived method calls and cross thread boundary
    WM_IQFEED_METHOD_CONNECT,
    WM_IQFEED_METHOD_DISCONNECT,
    WM_IQFEED_METHOD_SEND,
    // get everything closed down prior to PostQuitMessage
    WM_IQFEED_PRE_QUIT
  };

  BEGIN_MSG_MAP_EX(CIQFeed<ownerT>)
    MESSAGE_HANDLER( WM_CONN_INITIALIZED, OnConnInitialized );
    MESSAGE_HANDLER( WM_CONN_CLOSED, OnConnClosed );
    MESSAGE_HANDLER( WM_CONN_CONNECTED, OnConnConnected );
    MESSAGE_HANDLER( WM_CONN_DISCONNECTED, OnConnDisconnected );
    MESSAGE_HANDLER( WM_CONN_PROCESS, OnConnProcess );
    MESSAGE_HANDLER( WM_CONN_SENDDONE, OnConnSendDone );
    MESSAGE_HANDLER( WM_CONN_ERROR, OnConnError );

    MESSAGE_HANDLER( WM_IQFEED_DONE_UPDATE, OnDoneUpdate );
    MESSAGE_HANDLER( WM_IQFEED_DONE_SUMMARY, OnDoneSummary );
    MESSAGE_HANDLER( WM_IQFEED_DONE_NEWS, OnDoneNews );
    MESSAGE_HANDLER( WM_IQFEED_DONE_FUNDAMENTAL, OnDoneFundamental );
    MESSAGE_HANDLER( WM_IQFEED_DONE_TIME, OnDoneTime );
    MESSAGE_HANDLER( WM_IQFEED_DONE_SYSTEM, OnDoneSystem );

    MESSAGE_HANDLER( WM_IQFEED_METHOD_CONNECT, OnMethodConnect );
    MESSAGE_HANDLER( WM_IQFEED_METHOD_DISCONNECT, OnMethodDisconnect );
    MESSAGE_HANDLER( WM_IQFEED_METHOD_SEND, OnMethodSend );

    MESSAGE_HANDLER( WM_IQFEED_PRE_QUIT, OnPreQuit );
  END_MSG_MAP()

  LRESULT OnConnInitialized( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnClosed( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnConnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnDisconnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnProcess( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnSendDone( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnError( UINT, WPARAM, LPARAM, BOOL &bHandled );

  LRESULT OnDoneUpdate( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnDoneSummary( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnDoneNews( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnDoneFundamental( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnDoneTime( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnDoneSystem( UINT, WPARAM, LPARAM, BOOL &bHandled );

  LRESULT OnMethodConnect( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnMethodDisconnect( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnMethodSend( UINT, WPARAM, LPARAM, BOOL &bHandled );

  LRESULT OnPreQuit( UINT, WPARAM, LPARAM, BOOL &bHandled );

  BOOL InitializeThread( void );
  void CleanupThread( DWORD );

  void PostMessage( UINT id, WPARAM wparam = NULL, LPARAM lparam = NULL ) {
    PostThreadMessage( id, wparam, lparam );
  }
private:

  enum enumConnectionState {
    CS_QUIESCENT,  // no worker thread
    CS_DISCONNECTED,  // no connection created
    CS_CONNECTING,  // new connection, waiting for networkinitialization
    CS_CONNECTED,  // connected and ready for operation
    CS_DISCONNECTING  // initiated disconnect
  } m_stateConnection;

  structMessageDestinations m_structMessageDestinations;

  CAppModule* m_pModule;
  typename CNetwork<CIQFeed<ownerT> >* m_pconnIQFeed;
  typename CNetwork<CIQFeed<ownerT> >::structConnection m_connParameters;
  typename CNetwork<CIQFeed<ownerT> >::linerepository_t m_sendbuffers;
  typename CNetwork<CIQFeed<ownerT> >::structMessages m_NetworkMessages;

  typename CBufferRepository<CIQFUpdateMessage> m_reposUpdateMessages;
  typename CBufferRepository<CIQFSummaryMessage> m_reposSummaryMessages;
  typename CBufferRepository<CIQFNewsMessage> m_reposNewsMessages;
  typename CBufferRepository<CIQFFundamentalMessage> m_reposFundamentalMessages;
  typename CBufferRepository<CIQFTimeMessage> m_reposTimeMessages;
  typename CBufferRepository<CIQFSystemMessage> m_reposSystemMessages;

};

template <class ownerT>
CIQFeed<ownerT>::CIQFeed(CAppModule* pModule, const structMessageDestinations& MessageDestinations ) 
: m_pModule( pModule ),
  m_structMessageDestinations( MessageDestinations ),
  m_connParameters( "127.0.0.1", 5009 ),
  m_stateConnection( CS_QUIESCENT ), m_stateNews( NEWSISOFF ),
  m_NetworkMessages( this,
    WM_CONN_INITIALIZED, WM_CONN_CLOSED, WM_CONN_CONNECTED,
    WM_CONN_DISCONNECTED, WM_CONN_PROCESS, WM_CONN_SENDDONE, WM_CONN_ERROR ),
  CGuiThreadImpl<CIQFeed<ownerT> >( pModule )
{
}

template <class ownerT>
CIQFeed<ownerT>::~CIQFeed(void) {
  assert( CS_CONNECTED != m_stateConnection );
  PostThreadMessage( WM_IQFEED_PRE_QUIT );
  Join();
}

template <class ownerT>
BOOL CIQFeed<ownerT>::InitializeThread( void ) {

  m_stateConnection = CS_DISCONNECTED;

  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgInitialized );

  return TRUE;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnPreQuit( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
  if ( CS_DISCONNECTED == m_stateConnection ) {
    PostQuitMessage();
  }
  else {
    Sleep(10);
    PostThreadMessage( WM_IQFEED_PRE_QUIT ); // dally for a bit
  }
  bHandled = true;
  return 1;
}

template <class ownerT>
void CIQFeed<ownerT>::CleanupThread( DWORD dw ) {
  m_stateConnection = CS_QUIESCENT;
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
void CIQFeed<ownerT>::Send( const std::string& send, enumSend eSend ) {
  if ( CS_CONNECTED == m_stateConnection ) {
    CNetwork<CIQFeed<ownerT> >::linebuffer_t* psendbuffer = m_sendbuffers.CheckOut();
    psendbuffer->clear();
    BOOST_FOREACH( char ch, send ) {
      (*psendbuffer).push_back( ch );
    }
    PostThreadMessage( WM_IQFEED_METHOD_SEND, 
                       static_cast<WPARAM>( eSend ), 
                       reinterpret_cast<LPARAM>( psendbuffer ) );
  }
  else {
    throw std::logic_error( "CIQFeed::Send not in connected state" );
  }
}

template <class ownerT>
void CIQFeed<ownerT>::SetNewsOn( void ) {
  if ( NEWSISOFF == m_stateNews ) {
    m_stateNews = NEWSISON;
    std::stringstream ss;
    ss << "S,NEWSON" << std::endl;
    Send( ss.str(), SEND_AND_NO_FORWARD );
  }
}

template <class ownerT>
void CIQFeed<ownerT>::SetNewsOff( void ) {
  if ( NEWSISON == m_stateNews ) {
    m_stateNews = NEWSISOFF;
    std::stringstream ss;
    ss << "S,NEWSOFF" << std::endl;
    Send( ss.str(), SEND_AND_NO_FORWARD );
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
LRESULT CIQFeed<ownerT>::OnMethodSend( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  m_pconnIQFeed->PostThreadMessage( CNetwork<CIQFeed<ownerT> >::WM_NETWORK_SEND, wParam, lParam );

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
  m_pconnIQFeed = NULL;

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

#if defined _DEBUG
  std::string str( iter, end );
  str += '\n';
  OutputDebugString( str.c_str() );
#endif

  BOOST_ASSERT( iter != end );

  switch ( *iter ) {
    case 'Q': 
      {
        CIQFUpdateMessage* msg = m_reposUpdateMessages.CheckOut();
        msg->Assign( iter, end );
        if ( 0 != m_structMessageDestinations.msgMessageUpdate ) {
          m_structMessageDestinations.owner
            ->PostMessage( m_structMessageDestinations.msgMessageUpdate, 
              wParam, reinterpret_cast<LPARAM>( msg ) );
        }
        else {
          BOOL bHandled;
          OnDoneUpdate( m_structMessageDestinations.msgMessageUpdate, 
            wParam, reinterpret_cast<LPARAM>( msg ), bHandled );
        }
      }
      break;
    case 'P': 
      {
        CIQFSummaryMessage* msg = m_reposSummaryMessages.CheckOut();
        msg->Assign( iter, end );
        if ( 0 != m_structMessageDestinations.msgMessageSummary ) {
          m_structMessageDestinations.owner
            ->PostMessage( m_structMessageDestinations.msgMessageSummary, 
              wParam, reinterpret_cast<LPARAM>( msg ) );
        }
        else {
          BOOL bHandled;
          OnDoneSummary( m_structMessageDestinations.msgMessageSummary, 
            wParam, reinterpret_cast<LPARAM>( msg ), bHandled );
        }
      }
      break;
    case 'N': 
      {
        CIQFNewsMessage* msg = m_reposNewsMessages.CheckOut();
        msg->Assign( iter, end );
        if ( 0 != m_structMessageDestinations.msgMessageNews ) {
          m_structMessageDestinations.owner
            ->PostMessage( m_structMessageDestinations.msgMessageNews, 
              wParam, reinterpret_cast<LPARAM>( msg ) );
        }
        else {
          BOOL bHandled;
          OnDoneNews( m_structMessageDestinations.msgMessageNews, 
            wParam, reinterpret_cast<LPARAM>( msg ), bHandled );
        }
      }
      break;
    case 'F': 
      {
        CIQFFundamentalMessage* msg = m_reposFundamentalMessages.CheckOut();
        msg->Assign( iter, end );
        if ( 0 != m_structMessageDestinations.msgMessageFundamental ) {
          m_structMessageDestinations.owner
            ->PostMessage( m_structMessageDestinations.msgMessageFundamental, 
              wParam, reinterpret_cast<LPARAM>( msg ) );
        }
        else {
          BOOL bHandled;
          OnDoneFundamental( m_structMessageDestinations.msgMessageFundamental, 
            wParam, reinterpret_cast<LPARAM>( msg ), bHandled );
        }
      }
      break;
    case 'T': 
      {
        CIQFTimeMessage* msg = m_reposTimeMessages.CheckOut();
        msg->Assign( iter, end );
        if ( 0 != m_structMessageDestinations.msgMessageTime ) {
          m_structMessageDestinations.owner
            ->PostMessage( m_structMessageDestinations.msgMessageTime, 
              wParam, reinterpret_cast<LPARAM>( msg ) );
        }
        else {
          BOOL bHandled;
          OnDoneTime( m_structMessageDestinations.msgMessageTime, 
            wParam, reinterpret_cast<LPARAM>( msg ), bHandled );
        }
      }
      break;
    case 'S': 
      {
        CIQFSystemMessage* msg = m_reposSystemMessages.CheckOut();
        msg->Assign( iter, end );
        if ( _T( "KEY" ) == msg->Field( 2 ) ) {
          std::stringstream ss;
          ss << "S,KEY," << msg->Field( 3 ) << std::endl;
          Send( ss.str(), SEND_AND_NO_FORWARD );
        }
        if ( _T( "CUST" ) == msg->Field( 2 ) ) {
          if ( _T( "4.3.0.3" ) > msg->Field( 7 ) ) {
//            cout << "Need IQFeed version of 4.3.0.3 or greater (" << msg.Field( 7 ) << ")" << endl;
            //throw s;  // can't throw exception, just accept it, as we are getting '2.5.3' as a return
          }
        }
        if ( 0 != m_structMessageDestinations.msgMessageSystem ) {
          m_structMessageDestinations.owner
            ->PostMessage( m_structMessageDestinations.msgMessageSystem, 
            wParam, reinterpret_cast<LPARAM>( msg ) );
        }
        else {
          BOOL bHandled;
          OnDoneSystem( m_structMessageDestinations.msgMessageSystem, 
            wParam, reinterpret_cast<LPARAM>( msg ), bHandled );
        }
      }
      break;
    default:
      throw "Unknown message type in IQFeed"; // unknown message type
      break;
  }

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnConnSendDone( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  CNetwork<CIQFeed<ownerT> >::linebuffer_t* linebuffer = reinterpret_cast<CNetwork<CIQFeed<ownerT> >::linebuffer_t*>( lParam );
  m_sendbuffers.CheckIn( linebuffer );

  enumSend eForwardSendDone = static_cast<enumSend>( wParam );
  if ( SEND_AND_FORWARD == eForwardSendDone ) {
    m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgSendComplete );
  }

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnConnError( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  OutputDebugString( "CIQFeed::OnConnError error" );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnDoneUpdate( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  m_pconnIQFeed->PostThreadMessage( CNetwork<CIQFeed<ownerT> >::WM_NETWORK_PROCESSED, wParam );

  CIQFUpdateMessage* msg = reinterpret_cast<CIQFUpdateMessage*>( lParam );

  m_reposUpdateMessages.CheckIn( msg );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnDoneSummary( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  m_pconnIQFeed->PostThreadMessage( CNetwork<CIQFeed<ownerT> >::WM_NETWORK_PROCESSED, wParam );

  CIQFSummaryMessage* msg = reinterpret_cast<CIQFSummaryMessage*>( lParam );

  m_reposSummaryMessages.CheckIn( msg );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnDoneNews( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  m_pconnIQFeed->PostThreadMessage( CNetwork<CIQFeed<ownerT> >::WM_NETWORK_PROCESSED, wParam );

  CIQFNewsMessage* msg = reinterpret_cast<CIQFNewsMessage*>( lParam );

  m_reposNewsMessages.CheckIn( msg );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnDoneFundamental( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  m_pconnIQFeed->PostThreadMessage( CNetwork<CIQFeed<ownerT> >::WM_NETWORK_PROCESSED, wParam );

  CIQFFundamentalMessage* msg = reinterpret_cast<CIQFFundamentalMessage*>( lParam );

  m_reposFundamentalMessages.CheckIn( msg );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnDoneTime( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  m_pconnIQFeed->PostThreadMessage( CNetwork<CIQFeed<ownerT> >::WM_NETWORK_PROCESSED, wParam );

  CIQFTimeMessage* msg = reinterpret_cast<CIQFTimeMessage*>( lParam );

  m_reposTimeMessages.CheckIn( msg );

  bHandled = true;
  return 1;
}

template <class ownerT>
LRESULT CIQFeed<ownerT>::OnDoneSystem( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  m_pconnIQFeed->PostThreadMessage( CNetwork<CIQFeed<ownerT> >::WM_NETWORK_PROCESSED, wParam );

  CIQFSystemMessage* msg = reinterpret_cast<CIQFSystemMessage*>( lParam );

  m_reposSystemMessages.CheckIn( msg );

  bHandled = true;
  return 1;
}


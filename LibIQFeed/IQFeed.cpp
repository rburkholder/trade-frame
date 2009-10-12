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

#include "StdAfx.h"

#include <sstream>

#include <boost/assert.hpp>

#include "IQ32.H"
#include "IQFeed.h"
#include "IQFeedMessages.h"

// http://www.dtn.com/trading.cfm?sidenav=sn_trading&content=pr_nxcore

void __stdcall IQFeedCallBack( int x, int y ) {
  std::stringstream ss;
  ss << _T( "IQFeed Callback" ) << x << ", " << y;
}

CIQFeed::CIQFeed(CAppModule* pModule) 
: CGuiThreadImpl<CIQFeed>( pModule ), m_pModule( pModule ),
  m_connParameters( "127.0.0.1", 5009 ),
  m_NetworkMessages( this,
    WM_IQFEED_CONN_INITIALIZED, WM_IQFEED_CONN_CLOSED, WM_IQFEED_CONN_CONNECTED,
    WM_IQFEED_CONN_DISCONNECTED, WM_IQFEED_CONN_PROCESS, WM_IQFEED_CONN_SENDDONE, WM_IQFEED_CONN_ERROR )
{
}

CIQFeed::~CIQFeed(void) {
}

BOOL CIQFeed::InitializeThread( void ) {

  return TRUE;
}

void CIQFeed::CleanupThread( DWORD dw ) {

}

void CIQFeed::Connect( void ) { // post a local message, register needs to be in other thread

  SetCallbackFunction( &IQFeedCallBack );
  int i = RegisterClientApp( NULL, _T("ONE_UNIFIED"), _T("0.11111111"), _T("2.0") );

  m_pconnIQFeed = new CNetwork<CIQFeed>( m_pModule, m_NetworkMessages );

  m_pconnIQFeed->PostThreadMessage( 
    CNetwork<CIQFeed>::WM_NETWORK_CONNECT, reinterpret_cast<WPARAM>( &m_connParameters ) );

}

void CIQFeed::Send( const std::string& send ) {
}

void CIQFeed::Disconnect( void ) {  

  m_pconnIQFeed->PostThreadMessage( CNetwork<CIQFeed>::WM_NETWORK_DISCONNECT );

}

LRESULT CIQFeed::OnConnInitialized( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CIQFeed::OnConnClosed( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CIQFeed::OnConnConnected( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CIQFeed::OnConnDisconnected( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  delete m_pconnIQFeed;

  RemoveClientApp( NULL );

  bHandled = true;
  return 1;
}

LRESULT CIQFeed::OnConnProcess( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  CNetwork<CIQFeed>::linebuffer_t* buf = reinterpret_cast<CNetwork<CIQFeed>::linebuffer_t*>( wParam );
  CNetwork<CIQFeed>::linebuffer_t::iterator iter = (*buf).begin();
  CNetwork<CIQFeed>::linebuffer_t::iterator end = (*buf).end();

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


  bHandled = true;
  return 1;
}

LRESULT CIQFeed::OnConnSendDone( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CIQFeed::OnConnError( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}


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

#include <LibWtlCommon/NetworkClientSkeleton.h>

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

template <typename T>
class CIQFeed: public CNetworkClientSkeleton<CIQFeed<T> > {
public:

  typedef typename CNetworkClientSkeleton<CIQFeed<T> > inherited_t;

  // pre-initialized message ids for messages delivered to and accepted by external caller
  struct structMessageDestinations { // 
    // possibly make use of Boost named parameters here.
    T* owner;        // owner object to which message is sent (PostMessage needs to be implemented)
    UINT msgConnected;
    UINT msgSendComplete;
    UINT msgDisconnected;
    UINT msgError;  // not currently forwarded
    UINT msgMessageUpdate;
    UINT msgMessageSummary;
    UINT msgMessageNews;
    UINT msgMessageFundamental;
    UINT msgMessageTime;
    UINT msgMessageSystem;

    structMessageDestinations( void )
      : owner( NULL ), msgConnected( 0 ), msgSendComplete( 0 ), msgDisconnected( 0 ), msgError( 0 ),
        msgMessageUpdate( 0 ), msgMessageSummary( 0 ), msgMessageNews( 0 ), msgMessageFundamental( 0 ), msgMessageTime( 0 ), msgMessageSystem( 0 )
      {};
    structMessageDestinations( T* owner_, UINT msgConnected_, UINT msgSendComplete_, UINT msgDisconnected_, UINT msgError_,
        UINT msgMessageUpdate_, UINT msgMessageSummary_, UINT msgMessageNews_, UINT msgMessageFundamental_, UINT msgMessageTime_, UINT msgMessageSystem_ ) 
      : owner( owner_ ), msgConnected( msgConnected_ ), msgSendComplete( msgSendComplete_ ), msgDisconnected( msgDisconnected_ ), msgError( msgError_ ),
        msgMessageUpdate( msgMessageUpdate_ ), msgMessageSummary( msgMessageSummary_ ), msgMessageNews( msgMessageNews_ ),
        msgMessageFundamental( msgMessageFundamental_ ), msgMessageTime( msgMessageTime_ ), msgMessageSystem( msgMessageSystem_ )
      {
        BOOST_ASSERT( NULL != owner_ );
    };
  };

  CIQFeed(CAppModule* pModule, const structMessageDestinations&);
  ~CIQFeed(void);

  // used for returning message buffer
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
    // called when derived methods have finished with message and buffer
    WM_IQFEED_DONE_UPDATE = inherited_t::WM_NCS_ENDMARKER,
    WM_IQFEED_DONE_SUMMARY,
    WM_IQFEED_DONE_NEWS,
    WM_IQFEED_DONE_FUNDAMENTAL,
    WM_IQFEED_DONE_TIME,
    WM_IQFEED_DONE_SYSTEM,
  };

  BEGIN_MSG_MAP_EX(CIQFeed<T>)
    MESSAGE_HANDLER( WM_IQFEED_DONE_UPDATE, OnDoneUpdate )
    MESSAGE_HANDLER( WM_IQFEED_DONE_SUMMARY, OnDoneSummary )
    MESSAGE_HANDLER( WM_IQFEED_DONE_NEWS, OnDoneNews )
    MESSAGE_HANDLER( WM_IQFEED_DONE_FUNDAMENTAL, OnDoneFundamental )
    MESSAGE_HANDLER( WM_IQFEED_DONE_TIME, OnDoneTime )
    MESSAGE_HANDLER( WM_IQFEED_DONE_SYSTEM, OnDoneSystem )
    CHAIN_MSG_MAP(inherited_t)
  END_MSG_MAP()

  // overloads from CNetworkClientSkeleton
  LRESULT OnConnConnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnDisconnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnProcess( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnSendDone( UINT, WPARAM, LPARAM, BOOL &bHandled );

  // locally generated messages
  LRESULT OnDoneUpdate( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnDoneSummary( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnDoneNews( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnDoneFundamental( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnDoneTime( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnDoneSystem( UINT, WPARAM, LPARAM, BOOL &bHandled );

private:

  CAppModule* m_pModule;
  structMessageDestinations m_structMessageDestinations;

  typename CBufferRepository<CIQFUpdateMessage> m_reposUpdateMessages;
  typename CBufferRepository<CIQFSummaryMessage> m_reposSummaryMessages;
  typename CBufferRepository<CIQFNewsMessage> m_reposNewsMessages;
  typename CBufferRepository<CIQFFundamentalMessage> m_reposFundamentalMessages;
  typename CBufferRepository<CIQFTimeMessage> m_reposTimeMessages;
  typename CBufferRepository<CIQFSystemMessage> m_reposSystemMessages;

};

template <typename T>
CIQFeed<T>::CIQFeed(CAppModule* pModule, const structMessageDestinations& MessageDestinations ) 
: CNetworkClientSkeleton<CIQFeed<T> >( pModule, "127.0.0.1", 5009 ),
  m_stateNews( NEWSISOFF ),
  m_structMessageDestinations( MessageDestinations ),
  m_pModule( pModule )
  
{
  SetCallbackFunction( &IQFeedCallBack );
  int i = RegisterClientApp( NULL, _T("ONE_UNIFIED"), _T("0.11111111"), _T("2.0") );
}

template <typename T>
CIQFeed<T>::~CIQFeed(void) {
  RemoveClientApp( NULL );
}

template <typename T>
void CIQFeed<T>::SetNewsOn( void ) {
  if ( NEWSISOFF == m_stateNews ) {
    m_stateNews = NEWSISON;
    std::stringstream ss;
    ss << "S,NEWSON" << std::endl;
    Send( ss.str(), SEND_AND_NO_FORWARD );
  }
}

template <typename T>
void CIQFeed<T>::SetNewsOff( void ) {
  if ( NEWSISON == m_stateNews ) {
    m_stateNews = NEWSISOFF;
    std::stringstream ss;
    ss << "S,NEWSOFF" << std::endl;
    Send( ss.str(), SEND_AND_NO_FORWARD );
  }
}

template <typename T>
LRESULT CIQFeed<T>::OnConnConnected( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgConnected );

  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CIQFeed<T>::OnConnDisconnected( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgDisconnected );

  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CIQFeed<T>::OnConnSendDone( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgSendComplete );

  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CIQFeed<T>::OnConnProcess( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  inherited_t::linebuffer_t* buf = reinterpret_cast<inherited_t::linebuffer_t*>( wParam );
  inherited_t::linebuffer_t::iterator iter = (*buf).begin();
  inherited_t::linebuffer_t::iterator end = (*buf).end();

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

template <typename T>
LRESULT CIQFeed<T>::OnDoneUpdate( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  ReturnLineBuffer( wParam );

  CIQFUpdateMessage* msg = reinterpret_cast<CIQFUpdateMessage*>( lParam );

  m_reposUpdateMessages.CheckIn( msg );

  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CIQFeed<T>::OnDoneSummary( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  ReturnLineBuffer( wParam );

  CIQFSummaryMessage* msg = reinterpret_cast<CIQFSummaryMessage*>( lParam );

  m_reposSummaryMessages.CheckIn( msg );

  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CIQFeed<T>::OnDoneNews( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  ReturnLineBuffer( wParam );

  CIQFNewsMessage* msg = reinterpret_cast<CIQFNewsMessage*>( lParam );

  m_reposNewsMessages.CheckIn( msg );

  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CIQFeed<T>::OnDoneFundamental( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  ReturnLineBuffer( wParam );

  CIQFFundamentalMessage* msg = reinterpret_cast<CIQFFundamentalMessage*>( lParam );

  m_reposFundamentalMessages.CheckIn( msg );

  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CIQFeed<T>::OnDoneTime( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  ReturnLineBuffer( wParam );

  CIQFTimeMessage* msg = reinterpret_cast<CIQFTimeMessage*>( lParam );

  m_reposTimeMessages.CheckIn( msg );

  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CIQFeed<T>::OnDoneSystem( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  ReturnLineBuffer( wParam );

  CIQFSystemMessage* msg = reinterpret_cast<CIQFSystemMessage*>( lParam );

  m_reposSystemMessages.CheckIn( msg );

  bHandled = true;
  return 1;
}


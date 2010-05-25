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

template <typename T>
class CIQFeed: public CNetwork<CIQFeed<T> > {
  friend CNetwork<CIQFeed<T> >;
public:

  typedef typename CNetwork<CIQFeed<T> > inherited_t;
  typedef typename inherited_t::linebuffer_t linebuffer_t;

  // pre-initialized message ids for messages delivered to and accepted by external caller
  struct structMessageDestinations { // 
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

  CIQFeed(const structMessageDestinations&);
  ~CIQFeed(void);

  // used for returning message buffer
  void inline UpdateDone( linebuffer_t* p, CIQFUpdateMessage* msg ) {
    GiveBackBuffer( p );
    m_reposUpdateMessages.CheckInL( msg );
  }
  void inline SummaryDone( linebuffer_t* p, CIQFSummaryMessage* msg ) {
    GiveBackBuffer( p );
    m_reposSummaryMessages.CheckInL( msg );
  }
  void inline NewsDone( linebuffer_t* p, CIQFNewsMessage* msg ) {
    GiveBackBuffer( p );
    m_reposNewsMessages.CheckInL( msg );
  }
  void inline FundamentalDone( linebuffer_t* p, CIQFFundamentalMessage* msg ) {
    GiveBackBuffer( p );
    m_reposFundamentalMessages.CheckInL( msg );
  }
  void inline TimeDone( linebuffer_t* p, CIQFTimeMessage* msg ) {
    GiveBackBuffer( p );
    m_reposTimeMessages.CheckInL( msg );
  }
  void inline SystemDone( linebuffer_t* p, CIQFSystemMessage* msg ) {
    GiveBackBuffer( p );
    m_reposSystemMessages.CheckInL( msg );
  }

  void SetNewsOn( void );
  void SetNewsOff( void );

protected:

  enum enumNewsState {
    NEWSISON,
    NEWSISOFF
  } m_stateNews;

  // called by CNetwork via CRTP
  void OnNetworkConnected(void);
  void OnNetworkDisconnected(void);
  //void OnNetworkError( size_t );
  void OnNetworkLineBuffer( linebuffer_t* );  // new line available for processing
  void OnNetworkSendDone(void);

private:

  structMessageDestinations m_structMessageDestinations;

  typename CBufferRepository<CIQFUpdateMessage> m_reposUpdateMessages;
  typename CBufferRepository<CIQFSummaryMessage> m_reposSummaryMessages;
  typename CBufferRepository<CIQFNewsMessage> m_reposNewsMessages;
  typename CBufferRepository<CIQFFundamentalMessage> m_reposFundamentalMessages;
  typename CBufferRepository<CIQFTimeMessage> m_reposTimeMessages;
  typename CBufferRepository<CIQFSystemMessage> m_reposSystemMessages;

};

template <typename T>
CIQFeed<T>::CIQFeed(const structMessageDestinations& MessageDestinations ) 
: CNetwork<CIQFeed<T> >( "127.0.0.1", 5009 ),
  m_stateNews( NEWSISOFF ),
  m_structMessageDestinations( MessageDestinations )
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
    Send( ss.str() );
  }
}

template <typename T>
void CIQFeed<T>::SetNewsOff( void ) {
  if ( NEWSISON == m_stateNews ) {
    m_stateNews = NEWSISOFF;
    std::stringstream ss;
    ss << "S,NEWSOFF" << std::endl;
    Send( ss.str() );
  }
}

template <typename T>
void CIQFeed<T>::OnNetworkConnected( void ) {
  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgConnected );
}

template <typename T>
void CIQFeed<T>::OnNetworkDisconnected( void ) {
  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgDisconnected );
}

template <typename T>
void CIQFeed<T>::OnNetworkSendDone( void ) {
  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgSendComplete );
}

template <typename T>
void CIQFeed<T>::OnNetworkLineBuffer( linebuffer_t* pbuffer) {

  linebuffer_t::iterator iter = (*pbuffer).begin();
  linebuffer_t::iterator end = (*pbuffer).end();

  WPARAM wParam = reinterpret_cast<WPARAM>( pbuffer );

#if defined _DEBUG
  std::string str( iter, end );
  str += '\n';
  OutputDebugString( str.c_str() );
#endif

  BOOST_ASSERT( iter != end );

  switch ( *iter ) {
    case 'Q': 
      {
        CIQFUpdateMessage* msg = m_reposUpdateMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( 0 != m_structMessageDestinations.msgMessageUpdate ) {
          m_structMessageDestinations.owner
            ->PostMessage( m_structMessageDestinations.msgMessageUpdate, 
              wParam, reinterpret_cast<LPARAM>( msg ) );
        }
        else {
          UpdateDone( pbuffer, msg );
        }
      }
      break;
    case 'P': 
      {
        CIQFSummaryMessage* msg = m_reposSummaryMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( 0 != m_structMessageDestinations.msgMessageSummary ) {
          m_structMessageDestinations.owner
            ->PostMessage( m_structMessageDestinations.msgMessageSummary, 
              wParam, reinterpret_cast<LPARAM>( msg ) );
        }
        else {
          SummaryDone( pbuffer, msg );
        }
      }
      break;
    case 'N': 
      {
        CIQFNewsMessage* msg = m_reposNewsMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( 0 != m_structMessageDestinations.msgMessageNews ) {
          m_structMessageDestinations.owner
            ->PostMessage( m_structMessageDestinations.msgMessageNews, 
              wParam, reinterpret_cast<LPARAM>( msg ) );
        }
        else {
          NewsDone( pbuffer, msg );
        }
      }
      break;
    case 'F': 
      {
        CIQFFundamentalMessage* msg = m_reposFundamentalMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( 0 != m_structMessageDestinations.msgMessageFundamental ) {
          m_structMessageDestinations.owner
            ->PostMessage( m_structMessageDestinations.msgMessageFundamental, 
              wParam, reinterpret_cast<LPARAM>( msg ) );
        }
        else {
          FundamentalDone( pbuffer, msg );
        }
      }
      break;
    case 'T': 
      {
        CIQFTimeMessage* msg = m_reposTimeMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( 0 != m_structMessageDestinations.msgMessageTime ) {
          m_structMessageDestinations.owner
            ->PostMessage( m_structMessageDestinations.msgMessageTime, 
              wParam, reinterpret_cast<LPARAM>( msg ) );
        }
        else {
          TimeDone( pbuffer, msg );
        }
      }
      break;
    case 'S': 
      {
        CIQFSystemMessage* msg = m_reposSystemMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( _T( "KEY" ) == msg->Field( 2 ) ) {
          std::stringstream ss;
          ss << "S,KEY," << msg->Field( 3 ) << std::endl;
          Send( ss.str() );
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
          SystemDone( pbuffer, msg );
        }
      }
      break;
    default:
      throw "Unknown message type in IQFeed"; // unknown message type
      break;
  }

}


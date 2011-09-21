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

#include <OUCommon/Network.h>
#include <OUCommon/ReusableBuffers.h>

#include "IQ32.H"
#include "IQFeedMessages.h"

// custom on
// http://msdn.microsoft.com/en-us/library/e5ewb1h3.aspx
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
// custom off

void __stdcall IQFeedCallBack( int x, int y );

// In the future, for auxilliary routines making use of IQFeed, 
//   think about incorporating the following concept:
//     m_pPort = m_pIQFeedProvider->CheckOutLookupPort();
//     m_pIQFeedProvider->CheckInLookupPort( m_pPort );


namespace ou { // One Unified
namespace tf { // TradeFrame

template <typename T>
class CIQFeed: public ou::CNetwork<CIQFeed<T> > {
  friend ou::CNetwork<CIQFeed<T> >;
public:

  typedef typename ou::CNetwork<CIQFeed<T> > inherited_t;
  typedef typename inherited_t::linebuffer_t linebuffer_t;

  CIQFeed(void);
  ~CIQFeed(void);

  // used for returning message buffer
  // linebuffer_t needs to be kept with msg as there are dynamic accesses from it
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
  void OnNetworkConnected(void) {
    if ( &CIQFeed<T>::OnIQFeedConnected != &T::OnIQFeedConnected ) {
      static_cast<T*>( this )->OnIQFeedConnected();
    }
  };
  void OnNetworkDisconnected(void) {
    if ( &CIQFeed<T>::OnIQFeedDisConnected != &T::OnIQFeedDisConnected ) {
      static_cast<T*>( this )->OnIQFeedDisConnected();
    }
  };
  void OnNetworkError( size_t e ) {
    if ( &CIQFeed<T>::OnIQFeedError != &T::OnIQFeedError ) {
      static_cast<T*>( this )->OnIQFeedError(e);
    }
  };
  void OnNetworkSendDone(void) {
    if ( &CIQFeed<T>::OnIQFeedSendDone != &T::OnIQFeedSendDone ) {
      static_cast<T*>( this )->OnIQFeedSendDone();
    }
  };
  void OnNetworkLineBuffer( linebuffer_t* );  // new line available for processing

  // CRTP based dummy callbacks
  void OnIQFeedError( size_t ) {};
  void OnIQFeedConnected( void ) {};
  void OnIQFeedDisConnected( void ) {};
  void OnIQFeedSendDone( void ) {};
  void OnIQFeedFundamentalMessage( linebuffer_t* pBuffer, CIQFFundamentalMessage* msg) {};
  void OnIQFeedSummaryMessage( linebuffer_t* pBuffer, CIQFSummaryMessage* msg) {};
  void OnIQFeedUpdateMessage( linebuffer_t* pBuffer, CIQFUpdateMessage* msg) {};
  void OnIQFeedNewsMessage( linebuffer_t* pBuffer, CIQFNewsMessage* msg) {};
  void OnIQFeedTimeMessage( linebuffer_t* pBuffer, CIQFTimeMessage* msg) {};
  void OnIQFeedSystemMessage( linebuffer_t* pBuffer, CIQFSystemMessage* msg) {};

private:

  typename ou::CBufferRepository<CIQFUpdateMessage> m_reposUpdateMessages;
  typename ou::CBufferRepository<CIQFSummaryMessage> m_reposSummaryMessages;
  typename ou::CBufferRepository<CIQFNewsMessage> m_reposNewsMessages;
  typename ou::CBufferRepository<CIQFFundamentalMessage> m_reposFundamentalMessages;
  typename ou::CBufferRepository<CIQFTimeMessage> m_reposTimeMessages;
  typename ou::CBufferRepository<CIQFSystemMessage> m_reposSystemMessages;

};

template <typename T>
CIQFeed<T>::CIQFeed( void ) 
: ou::CNetwork<CIQFeed<T> >( "127.0.0.1", 5009 ),
  m_stateNews( NEWSISOFF )
{
  SetCallbackFunction( &IQFeedCallBack );
  int i = RegisterClientApp( NULL, "ONE_UNIFIED", "0.11111111", "2.0" );
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
void CIQFeed<T>::OnNetworkLineBuffer( linebuffer_t* pBuffer ) {

  linebuffer_t::iterator iter = (*pBuffer).begin();
  linebuffer_t::iterator end = (*pBuffer).end();

#if defined _DEBUG
//  std::string str( iter, end );
//  str += '\n';
//  OutputDebugString( str.c_str() );
#endif

  BOOST_ASSERT( iter != end );

  switch ( *iter ) {
    case 'Q': 
      {
        CIQFUpdateMessage* msg = m_reposUpdateMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( &CIQFeed<T>::OnIQFeedUpdateMessage != &T::OnIQFeedUpdateMessage ) {
          static_cast<T*>( this )->OnIQFeedUpdateMessage( pBuffer, msg);
        }
        else {
          UpdateDone( pBuffer, msg );
        }
      }
      break;
    case 'P': 
      {
        CIQFSummaryMessage* msg = m_reposSummaryMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( &CIQFeed<T>::OnIQFeedSummaryMessage != &T::OnIQFeedSummaryMessage ) {
          static_cast<T*>( this )->OnIQFeedSummaryMessage( pBuffer, msg);
        }
        else {
          SummaryDone( pBuffer, msg );
        }
      }
      break;
    case 'N': 
      {
        CIQFNewsMessage* msg = m_reposNewsMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( &CIQFeed<T>::OnIQFeedNewsMessage != &T::OnIQFeedNewsMessage ) {
          static_cast<T*>( this )->OnIQFeedNewsMessage( pBuffer, msg);
        }
        else {
          NewsDone( pBuffer, msg );
        }
      }
      break;
    case 'F': 
      {
        CIQFFundamentalMessage* msg = m_reposFundamentalMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( &CIQFeed<T>::OnIQFeedFundamentalMessage != &T::OnIQFeedFundamentalMessage ) {
          static_cast<T*>( this )->OnIQFeedFundamentalMessage( pBuffer, msg);
        }
        else {
          FundamentalDone( pBuffer, msg );
        }
      }
      break;
    case 'T': 
      {
        CIQFTimeMessage* msg = m_reposTimeMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( &CIQFeed<T>::OnIQFeedTimeMessage != &T::OnIQFeedTimeMessage ) {
          static_cast<T*>( this )->OnIQFeedTimeMessage( pBuffer, msg);
        }
        else {
          TimeDone( pBuffer, msg );
        }
      }
      break;
    case 'S': 
      {
        CIQFSystemMessage* msg = m_reposSystemMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( "KEY" == msg->Field( 2 ) ) {
          std::stringstream ss;
          ss << "S,KEY," << msg->Field( 3 ) << std::endl;
          Send( ss.str() );
        }
        if ( "CUST" == msg->Field( 2 ) ) {
          if ( "4.3.0.3" > msg->Field( 7 ) ) {
//            cout << "Need IQFeed version of 4.3.0.3 or greater (" << msg.Field( 7 ) << ")" << endl;
            //throw s;  // can't throw exception, just accept it, as we are getting '2.5.3' as a return
          }
        }
        if ( &CIQFeed<T>::OnIQFeedSystemMessage != &T::OnIQFeedSystemMessage ) {
          static_cast<T*>( this )->OnIQFeedSystemMessage( pBuffer, msg);
        }
        else {
          SystemDone( pBuffer, msg );
        }
      }
      break;
    default:
      throw "Unknown message type in IQFeed"; // unknown message type
      break;
  }

}

} // namespace tf
} // namespace ou

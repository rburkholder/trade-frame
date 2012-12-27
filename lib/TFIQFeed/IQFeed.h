/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include <OUCommon/Debug.h>
#include <OUCommon/Network.h>
#include <OUCommon/ReusableBuffers.h>

#include "IQFeedMessages.h"

// In the future, for auxilliary routines making use of IQFeed, 
//   think about incorporating the following concept:
//     m_pPort = m_pIQFeedProvider->CheckOutLookupPort();
//     m_pIQFeedProvider->CheckInLookupPort( m_pPort );


namespace ou { // One Unified
namespace tf { // TradeFrame

template <typename T>
class IQFeed: public ou::Network<IQFeed<T> > {
  friend ou::Network<IQFeed<T> >;
public:

  typedef typename ou::Network<IQFeed<T> > inherited_t;
  typedef typename inherited_t::linebuffer_t linebuffer_t;

  IQFeed(void);
  ~IQFeed(void);

  // used for returning message buffer
  // linebuffer_t needs to be kept with msg as there are dynamic accesses from it
  void inline UpdateDone( linebuffer_t* p, IQFUpdateMessage* msg ) {
    GiveBackBuffer( p );
    m_reposUpdateMessages.CheckInL( msg );
  }
  void inline SummaryDone( linebuffer_t* p, IQFSummaryMessage* msg ) {
    GiveBackBuffer( p );
    m_reposSummaryMessages.CheckInL( msg );
  }
  void inline NewsDone( linebuffer_t* p, IQFNewsMessage* msg ) {
    GiveBackBuffer( p );
    m_reposNewsMessages.CheckInL( msg );
  }
  void inline FundamentalDone( linebuffer_t* p, IQFFundamentalMessage* msg ) {
    GiveBackBuffer( p );
    m_reposFundamentalMessages.CheckInL( msg );
  }
  void inline TimeDone( linebuffer_t* p, IQFTimeMessage* msg ) {
    GiveBackBuffer( p );
    m_reposTimeMessages.CheckInL( msg );
  }
  void inline SystemDone( linebuffer_t* p, IQFSystemMessage* msg ) {
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
    if ( &IQFeed<T>::OnIQFeedConnected != &T::OnIQFeedConnected ) {
      static_cast<T*>( this )->OnIQFeedConnected();
    }
  };
  void OnNetworkDisconnected(void) {
    if ( &IQFeed<T>::OnIQFeedDisConnected != &T::OnIQFeedDisConnected ) {
      static_cast<T*>( this )->OnIQFeedDisConnected();
    }
  };
  void OnNetworkError( size_t e ) {
    if ( &IQFeed<T>::OnIQFeedError != &T::OnIQFeedError ) {
      static_cast<T*>( this )->OnIQFeedError(e);
    }
  };
  void OnNetworkSendDone(void) {
    if ( &IQFeed<T>::OnIQFeedSendDone != &T::OnIQFeedSendDone ) {
      static_cast<T*>( this )->OnIQFeedSendDone();
    }
  };
  void OnNetworkLineBuffer( linebuffer_t* );  // new line available for processing

  // CRTP based dummy callbacks
  void OnIQFeedError( size_t ) {};
  void OnIQFeedConnected( void ) {};
  void OnIQFeedDisConnected( void ) {};
  void OnIQFeedSendDone( void ) {};
  void OnIQFeedFundamentalMessage( linebuffer_t* pBuffer, IQFFundamentalMessage* msg) {};
  void OnIQFeedSummaryMessage( linebuffer_t* pBuffer, IQFSummaryMessage* msg) {};
  void OnIQFeedUpdateMessage( linebuffer_t* pBuffer, IQFUpdateMessage* msg) {};
  void OnIQFeedNewsMessage( linebuffer_t* pBuffer, IQFNewsMessage* msg) {};
  void OnIQFeedTimeMessage( linebuffer_t* pBuffer, IQFTimeMessage* msg) {};
  void OnIQFeedSystemMessage( linebuffer_t* pBuffer, IQFSystemMessage* msg) {};

private:

  typename ou::BufferRepository<IQFUpdateMessage> m_reposUpdateMessages;
  typename ou::BufferRepository<IQFSummaryMessage> m_reposSummaryMessages;
  typename ou::BufferRepository<IQFNewsMessage> m_reposNewsMessages;
  typename ou::BufferRepository<IQFFundamentalMessage> m_reposFundamentalMessages;
  typename ou::BufferRepository<IQFTimeMessage> m_reposTimeMessages;
  typename ou::BufferRepository<IQFSystemMessage> m_reposSystemMessages;

};

template <typename T>
IQFeed<T>::IQFeed( void ) 
: ou::Network<IQFeed<T> >( "127.0.0.1", 5009 ),
  m_stateNews( NEWSISOFF )
{
}

template <typename T>
IQFeed<T>::~IQFeed(void) {
}

template <typename T>
void IQFeed<T>::SetNewsOn( void ) {
  if ( NEWSISOFF == m_stateNews ) {
    m_stateNews = NEWSISON;
    std::stringstream ss;
    ss << "S,NEWSON" << std::endl;
    Send( ss.str() );
  }
}

template <typename T>
void IQFeed<T>::SetNewsOff( void ) {
  if ( NEWSISON == m_stateNews ) {
    m_stateNews = NEWSISOFF;
    std::stringstream ss;
    ss << "S,NEWSOFF" << std::endl;
    Send( ss.str() );
  }
}

template <typename T>
void IQFeed<T>::OnNetworkLineBuffer( linebuffer_t* pBuffer ) {

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
        IQFUpdateMessage* msg = m_reposUpdateMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( &IQFeed<T>::OnIQFeedUpdateMessage != &T::OnIQFeedUpdateMessage ) {
          static_cast<T*>( this )->OnIQFeedUpdateMessage( pBuffer, msg);
        }
        else {
          UpdateDone( pBuffer, msg );
        }
      }
      break;
    case 'P': 
      {
        IQFSummaryMessage* msg = m_reposSummaryMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( &IQFeed<T>::OnIQFeedSummaryMessage != &T::OnIQFeedSummaryMessage ) {
          static_cast<T*>( this )->OnIQFeedSummaryMessage( pBuffer, msg);
        }
        else {
          SummaryDone( pBuffer, msg );
        }
      }
      break;
    case 'N': 
      {
        IQFNewsMessage* msg = m_reposNewsMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( &IQFeed<T>::OnIQFeedNewsMessage != &T::OnIQFeedNewsMessage ) {
          static_cast<T*>( this )->OnIQFeedNewsMessage( pBuffer, msg);
        }
        else {
          NewsDone( pBuffer, msg );
        }
      }
      break;
    case 'F': 
      {
        IQFFundamentalMessage* msg = m_reposFundamentalMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( &IQFeed<T>::OnIQFeedFundamentalMessage != &T::OnIQFeedFundamentalMessage ) {
          static_cast<T*>( this )->OnIQFeedFundamentalMessage( pBuffer, msg);
        }
        else {
          FundamentalDone( pBuffer, msg );
        }
      }
      break;
    case 'T': 
      {
        IQFTimeMessage* msg = m_reposTimeMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( &IQFeed<T>::OnIQFeedTimeMessage != &T::OnIQFeedTimeMessage ) {
          static_cast<T*>( this )->OnIQFeedTimeMessage( pBuffer, msg);
        }
        else {
          TimeDone( pBuffer, msg );
        }
      }
      break;
    case 'S': 
      {
        IQFSystemMessage* msg = m_reposSystemMessages.CheckOutL();
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
        if ( &IQFeed<T>::OnIQFeedSystemMessage != &T::OnIQFeedSystemMessage ) {
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

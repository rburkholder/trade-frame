/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

// 2010/05/27 translates IQFeed events to Win API Messages for cross thread consumption

#include "IQfeed.h"

template <typename T>
class CIQFeedMsgShim: public CIQFeed<CIQFeedMsgShim<T> > {
  friend CIQFeed<CIQFeedMsgShim<T> >;
public:
  typedef typename CIQFeed<CIQFeedMsgShim<T> > inherited_t;
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
 
  CIQFeedMsgShim( const structMessageDestinations& MessageDestinations) 
    : m_structMessageDestinations( MessageDestinations ) {};
  ~CIQFeedMsgShim( void ) {};


protected:

  void OnIQFeedConnected( void ) {
    m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgConnected );
  }

  void OnNetworkDisconnected( void ) {
    m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgDisconnected );
  }

  void OnIQFeedError( size_t e ) {
    m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgError, e );
  }

  void OnIQFeedSendDone( void ) {
    m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgSendComplete );
  }

  void OnIQFeedUpdateMessage( linebuffer_t* pbuffer, CIQFUpdateMessage* msg ) {
    m_structMessageDestinations.owner->PostMessage( 
      m_structMessageDestinations.msgMessageUpdate, 
      reinterpret_cast<WPARAM>( pbuffer ),
      reinterpret_cast<LPARAM>( msg )
      );
  }

  void OnIQFeedSummaryMessage( linebuffer_t* pbuffer, CIQFSummaryMessage* msg ) {
    m_structMessageDestinations.owner->PostMessage( 
      m_structMessageDestinations.msgMessageSummary, 
      reinterpret_cast<WPARAM>( pbuffer ),
      reinterpret_cast<LPARAM>( msg )
      );
  }

  void OnIQFeedNewsMessage( linebuffer_t* pbuffer, CIQFNewsMessage* msg ) {
    m_structMessageDestinations.owner->PostMessage( 
      m_structMessageDestinations.msgMessageNews, 
      reinterpret_cast<WPARAM>( pbuffer ),
      reinterpret_cast<LPARAM>( msg )
      );
  }

  void OnIQFeedFundamentalMessage( linebuffer_t* pbuffer, CIQFFundamentalMessage* msg ) {
    m_structMessageDestinations.owner->PostMessage( 
      m_structMessageDestinations.msgMessageFundamental, 
      reinterpret_cast<WPARAM>( pbuffer ),
      reinterpret_cast<LPARAM>( msg )
      );
  }

  void OnIQFeedTimeMessage( linebuffer_t* pbuffer, CIQFTimeMessage* msg ) {
    m_structMessageDestinations.owner->PostMessage( 
      m_structMessageDestinations.msgMessageTime, 
      reinterpret_cast<WPARAM>( pbuffer ),
      reinterpret_cast<LPARAM>( msg )
      );
  }

  void OnIQFeedSystemMessage( linebuffer_t* pbuffer, CIQFSystemMessage* msg ) {
    m_structMessageDestinations.owner->PostMessage( 
      m_structMessageDestinations.msgMessageSystem, 
      reinterpret_cast<WPARAM>( pbuffer ),
      reinterpret_cast<LPARAM>( msg )
      );
  }

private:
    structMessageDestinations m_structMessageDestinations;
};


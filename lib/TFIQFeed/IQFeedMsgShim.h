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

namespace ou { // One Unified
namespace tf { // TradeFrame

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
    : m_structMessageDestinations( MessageDestinations ) {
      assert( NULL != MessageDestinations.owner );
  };
  ~CIQFeedMsgShim( void ) {};


protected:

  // CRTP callbacks

  void OnIQFeedConnected( void ) {
    if ( 0 != m_structMessageDestinations.msgConnected ) {
      m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgConnected );
    }
  }

  void OnIQFeedDisconnected( void ) {
    if ( 0 != m_structMessageDestinations.msgDisconnected ) {
      m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgDisconnected );
    }
  }

  void OnIQFeedError( size_t e ) {
    if ( 0 != m_structMessageDestinations.msgError ) {
      m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgError, e );
    }
  }

  void OnIQFeedSendDone( void ) {
    if ( 0 != m_structMessageDestinations.msgSendComplete ) {
      m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgSendComplete );
    }
  }

  void OnIQFeedUpdateMessage( linebuffer_t* pBuffer, CIQFUpdateMessage* pMsg ) {
    if ( 0 != m_structMessageDestinations.msgMessageUpdate ) {
      m_structMessageDestinations.owner->PostMessage( 
        m_structMessageDestinations.msgMessageUpdate, 
        reinterpret_cast<WPARAM>( pBuffer ),
        reinterpret_cast<LPARAM>( pMsg )
        );
    }
    else {
      UpdateDone( pBuffer, pMsg );
    }
  }

  void OnIQFeedSummaryMessage( linebuffer_t* pBuffer, CIQFSummaryMessage* pMsg ) {
    if ( 0 != m_structMessageDestinations.msgMessageSummary ) {
      m_structMessageDestinations.owner->PostMessage( 
        m_structMessageDestinations.msgMessageSummary, 
        reinterpret_cast<WPARAM>( pBuffer ),
        reinterpret_cast<LPARAM>( pMsg )
        );
    }
    else {
      SummaryDone( pBuffer, pMsg );
    }
  }

  void OnIQFeedNewsMessage( linebuffer_t* pBuffer, CIQFNewsMessage* pMsg ) {
    if ( 0 != m_structMessageDestinations.msgMessageNews ) {
      m_structMessageDestinations.owner->PostMessage( 
        m_structMessageDestinations.msgMessageNews, 
        reinterpret_cast<WPARAM>( pBuffer ),
        reinterpret_cast<LPARAM>( pMsg )
        );
    }
    else {
      NewsDone( pBuffer, pMsg );
    }
  }

  void OnIQFeedFundamentalMessage( linebuffer_t* pBuffer, CIQFFundamentalMessage* pMsg ) {
    if ( 0 != m_structMessageDestinations.msgMessageFundamental ) {
      m_structMessageDestinations.owner->PostMessage( 
        m_structMessageDestinations.msgMessageFundamental, 
        reinterpret_cast<WPARAM>( pBuffer ),
        reinterpret_cast<LPARAM>( pMsg )
        );
    }
    else {
      FundamentalDone( pBuffer, pMsg );
    }
  }

  void OnIQFeedTimeMessage( linebuffer_t* pBuffer, CIQFTimeMessage* pMsg ) {
    if ( 0 != m_structMessageDestinations.msgMessageTime ) {
      m_structMessageDestinations.owner->PostMessage( 
        m_structMessageDestinations.msgMessageTime, 
        reinterpret_cast<WPARAM>( pBuffer ),
        reinterpret_cast<LPARAM>( pMsg )
        );
    }
    else {
      TimeDone( pBuffer, pMsg );
    }
  }

  void OnIQFeedSystemMessage( linebuffer_t* pBuffer, CIQFSystemMessage* pMsg ) {
    if ( 0 != m_structMessageDestinations.msgMessageSystem ) {
      m_structMessageDestinations.owner->PostMessage( 
        m_structMessageDestinations.msgMessageSystem, 
        reinterpret_cast<WPARAM>( pBuffer ),
        reinterpret_cast<LPARAM>( pMsg )
        );
    }
    else {
      SystemDone( pBuffer, pMsg );
    }
  }

private:
    structMessageDestinations m_structMessageDestinations;
};

} // namespace tf
} // namespace ou

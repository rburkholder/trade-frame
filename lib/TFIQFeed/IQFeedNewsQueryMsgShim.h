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

// 2010/05/29 translates IQFeedNewsQuery events to Win API Messages for cross thread consumption

#include "IQFeedNewsQuery.h"

template <typename T>
class CIQFeedNewsQueryMsgShim: public CIQFeedNewsQuery<CIQFeedNewsQueryMsgShim<T> > {
  friend CIQFeedNewsQuery<CIQFeedNewsQueryMsgShim<T> >;
public:
  typedef CIQFeedNewsQuery<CIQFeedNewsQueryMsgShim<T> > inherited_t;

  // pre-initialized message ids for messages delivered to and accepted by external caller
  struct structMessageDestinations {
    T* owner;
    UINT msgConnected;
    UINT msgSendComplete;
    UINT msgDisconnected;
    UINT msgError;  // not currently forwarded
    UINT msgNewsConfigDone;
    UINT msgNewsStoryLine;
    UINT msgNewsStoryDone;
    structMessageDestinations( void )
      : owner( NULL ), msgConnected( 0 ), msgSendComplete( 0 ), msgDisconnected( 0 ), msgError( 0 ),
        msgNewsConfigDone( 0 ), 
        msgNewsStoryLine( 0 ), msgNewsStoryDone( 0 )
    {};
    structMessageDestinations( 
      T* owner_, 
      UINT msgConnected_, UINT msgSendComplete_, UINT msgDisconnected_, UINT msgError_,
      UINT msgNewsConfigDone_, 
      UINT msgNewsStoryLine_, UINT msgNewsStoryDone_
      ) 
    : owner( owner_ ), 
      msgConnected( msgConnected_ ), msgSendComplete( msgSendComplete_ ), msgDisconnected( msgDisconnected_ ), msgError( msgError_ ),
      msgNewsConfigDone( msgNewsConfigDone_ ),
      msgNewsStoryLine( msgNewsStoryLine_ ), msgNewsStoryDone( msgNewsStoryDone_ )
    {
      assert( NULL != owner_ );
    };
  };

  CIQFeedNewsQueryMsgShim( const structMessageDestinations& MessageDestinations) 
    : m_structMessageDestinations( MessageDestinations ) {
      assert( NULL != MessageDestinations.owner );
  };
  ~CIQFeedNewsQueryMsgShim( void ) {};

protected:
  // CRTP callbacks

  void OnNewsQueryConnected( void ) {
    if ( 0 != m_structMessageDestinations.msgConnected ) {
      m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgConnected );
    }
  };

  void OnNewsQueryDisconnected( void ) {
    if ( 0 != m_structMessageDestinations.msgDisconnected ) {
      m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgDisconnected );
    }
  };

  void OnNewsQueryError( size_t e ) {
    if ( 0 != m_structMessageDestinations.msgError ) {
      m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgError, e );
    }
  };

  void OnNewsQuerySendDone( void ) {
    if ( 0 != m_structMessageDestinations.msgSendComplete ) {
      m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgSendComplete );
    }
  };

  void OnNewsQueryNewsConfigDone( void ) {
    if ( 0 != m_structMessageDestinations.msgNewsConfigDone ) {
      m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgNewsConfigDone );
    }
  };

  void OnNewsQueryStoryLine( linebuffer_t* buf, LPARAM lParam ) {
    if ( 0 != m_structMessageDestinations.msgNewsStoryLine ) {
      m_structMessageDestinations.owner->PostMessage( 
        m_structMessageDestinations.msgNewsStoryLine, reinterpret_cast<WPARAM>( buf ), lParam );
    }
  };

  void OnNewsQueryStoryDone( LPARAM lParam ) {
    if ( 0 != m_structMessageDestinations.msgNewsStoryDone ) {
      m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgNewsStoryDone, 0, lParam );
    }
  };


private:
  structMessageDestinations m_structMessageDestinations;
};
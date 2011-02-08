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

// 2010/05/30 translates IQFeedHistoryQuery events to Win API Messages for cross thread consumption

#include "IQFeedHistoryQuery.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

template <typename T>
class CIQFeedHistoryQueryMsgShim: public CIQFeedHistoryQuery<CIQFeedHistoryQueryMsgShim<T> > {
  friend CIQFeedHistoryQuery<CIQFeedHistoryQueryMsgShim<T> >;
public:
  typedef typename CIQFeedHistoryQuery<CIQFeedHistoryQueryMsgShim<T> > inherited_t;

  struct structMessageDestinations {
    T* owner;
    UINT msgConnected;
    UINT msgSendComplete;
    UINT msgDisconnected;

    UINT msgError;  // not currently forwarded

    UINT msgHistoryTickDataPoint;
    UINT msgHistoryIntervalData;
    UINT msgHistorySummaryData;

    UINT msgHistoryRequestDone;

    structMessageDestinations( void )
      : owner( NULL ), msgConnected( 0 ), msgSendComplete( 0 ), msgDisconnected( 0 ), msgError( 0 ),
        msgHistoryTickDataPoint( 0 ), msgHistoryIntervalData( 0 ), msgHistorySummaryData( 0 ), 
        msgHistoryRequestDone( 0 )
    {};
    structMessageDestinations(
      T* owner_, 
      UINT msgConnected_, UINT msgSendComplete_, UINT msgDisconnected_, UINT msgError_,
      UINT msgHistoryTickDataPoint_, UINT msgHistoryIntervalData_, UINT msgHistorySummaryData_, 
      UINT msgHistoryRequestDone_
      ) 
    : owner( owner_ ), 
      msgConnected( msgConnected_ ), msgSendComplete( msgSendComplete_ ), msgDisconnected( msgDisconnected_ ), msgError( msgError_ ),
      msgHistoryTickDataPoint( msgHistoryTickDataPoint_ ),
      msgHistoryIntervalData( msgHistoryIntervalData_ ), msgHistorySummaryData( msgHistorySummaryData_ ), 
      msgHistoryRequestDone( msgHistoryRequestDone_ )
    {
      assert( NULL != owner_ );
    };
  };

  CIQFeedHistoryQueryMsgShim( const structMessageDestinations& MessageDestinations) 
    : m_structMessageDestinations( MessageDestinations ) {
      assert( NULL != MessageDestinations.owner );
  };
  ~CIQFeedHistoryQueryMsgShim( void ) {};

protected:

  // CRTP callbacks

  void OnHistoryConnected( void ) {
    if ( 0 != m_structMessageDestinations.msgConnected ) {
      m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgConnected );
    }
  }

  void OnHistoryDisconnected( void ) {
    if ( 0 != m_structMessageDestinations.msgDisconnected ) {
      m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgDisconnected );
    }
  }

  void OnHistoryError( size_t e ) {
    if ( 0 != m_structMessageDestinations.msgError ) {
      m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgError, e );
    }
  }

  void OnHistorySendDone( void ) {
    if ( 0 != m_structMessageDestinations.msgSendComplete ) {
      m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgSendComplete );
    }
  }

  void OnHistoryTickDataPoint( structTickDataPoint* pDP ) {
    if ( 0 != m_structMessageDestinations.msgHistoryTickDataPoint ) {
      m_structMessageDestinations.owner->PostMessage( 
        m_structMessageDestinations.msgHistoryTickDataPoint, reinterpret_cast<WPARAM>( pDP ) );
    }
  }

  void OnHistoryIntervalData( structInterval* pDP ) {
    if ( 0 != m_structMessageDestinations.msgHistoryIntervalData ) {
      m_structMessageDestinations.owner->PostMessage( 
        m_structMessageDestinations.msgHistoryIntervalData, reinterpret_cast<WPARAM>( pDP ) );
    }
  }

  void OnHistorySummaryData( structSummary* pDP ) {
    if ( 0 != m_structMessageDestinations.msgHistorySummaryData ) {
      m_structMessageDestinations.owner->PostMessage( 
        m_structMessageDestinations.msgHistorySummaryData, reinterpret_cast<WPARAM>( pDP ) );
    }
  }

  void OnHistoryRequestDone( void ) {
    if ( 0 != m_structMessageDestinations.msgHistoryRequestDone ) {
      m_structMessageDestinations.owner->PostMessage( 
        m_structMessageDestinations.msgHistoryRequestDone, 0 );
      }
  }

private:

  structMessageDestinations m_structMessageDestinations;

};

} // namespace tf
} // namespace ou

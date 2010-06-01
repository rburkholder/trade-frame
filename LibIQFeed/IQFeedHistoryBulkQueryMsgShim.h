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

// 2010/05/31 translates CIQFeedHistoryBulkQuery events to Win API Messages for cross thread consumption

#include "IQFeedHistoryBulkQuery.h"

template <typename T>
class CIQFeedHistoryBulkQueryMsgShim: public CIQFeedHistoryBulkQuery<CIQFeedHistoryBulkQueryMsgShim<T> > {
  friend CIQFeedHistoryBulkQuery<CIQFeedHistoryBulkQuery<CIQFeedHistoryBulkQueryMsgShim<T> >;
public:
  typedef typename CIQFeedHistoryBulkQuery<CIQFeedHistoryBulkQueryMsgShim<T> > inherited_t;

  enum enumMessages {
    WM_HBQ_Done,
    WM_HBQ_Bars,
    WM_HBQ_Ticks
  };

  struct structMessageDestinations {
    T* owner;
    UINT msgDone;
    UINT msgBars;
    UINT msgTicks;
    structMessageDestinations( void ) 
      : owner( NULL ), msgDone( 0 ), msgBars( 0 ), msgTicks( 0 ) {};
    structMessageDestinations( T* owner_, UINT msgDone_, UINT msgBars_, UINT msgTicks_ )
      : owner( owner_ ), msgDone( msgDone_ ), msgBars( msgBars_ ), msgTicks( msgTicks_ ) ) { assert( NULL != owner_ ); };
  };

  void DailyBars( size_t n, const structMessageDestinations& messages ) {
    m_messageDestinations = messages;
    inherited_t::DailyBars( n );
  };


protected:
  typename typedef inherited_t::query_t query_t;
private:
  structMessageDestinations m_messageDestinations;
};
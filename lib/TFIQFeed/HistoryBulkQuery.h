/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

// processes a series of historical data requests against the IQFeed API

#include <string>
#include <vector>
#include <cassert>

#include <algorithm>

#include <boost/atomic.hpp>

#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <OUCommon/ReusableBuffers.h>
#include <TFTimeSeries/TimeSeries.h>

#include "HistoryQuery.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

template <typename T, typename U>  // T=CRTP caller, U=internal use for marking queries
class HistoryQueryTag: public HistoryQuery<HistoryQueryTag<T,U> > {
  friend HistoryQuery<HistoryQueryTag<T,U> >;
public:
  // 2015/11/07 possibly a msvc version?
  //typedef typename HistoryQuery<HistoryQueryTag<T,U> > inherited_t;
  typedef HistoryQuery<HistoryQueryTag<T,U> > inherited_t;
  HistoryQueryTag( T* t = NULL ) : m_t( t ), m_bActivated( false ) {
  };

  HistoryQueryTag( T* t, U tagUser ) : m_t( t ), m_tagUser( tagUser ), m_bActivated( false ) {
  };

  virtual ~HistoryQueryTag() {
  };

  void SetUserTag( U tagUser ) { m_tagUser = tagUser; };
  U GetUserTag() { return m_tagUser; };

  void SetT( T* t ) { m_t = t; };
  T* GetT( void ) { return m_t; };

  void Activate() { m_bActivated = true; };
  bool Activated() { return m_bActivated; };

protected:

  // CRTP prototypes
//  void OnHistoryConnected( U ) {};
//  void OnHistoryDisconnected( U ) {};
//  void OnHistoryError( U, size_t ) {};
//  void OnHistorySendDone( U ) {};
//  void OnHistoryTickDataPoint( U, HistoryStructs::structTickDataPoint* ) {};
//  void OnHistoryIntervalData( U, HistoryStructs::structInterval* ) {};
//  void OnHistoryEndOfDayData( U, HistoryStructs::structEndOfDay ) {};
//  void OnHistoryRequestDone( U ) {};

  // CRTP based callbacks;
  void OnHistoryConnected() {
    assert( nullptr != m_t );
    static_cast<T*>( m_t )->OnHistoryConnected( m_tagUser );
  };

  void OnHistoryDisconnected() {
    assert( nullptr != m_t );
    static_cast<T*>( m_t )->OnHistoryDisconnected( m_tagUser );
  };

  void OnHistoryError( size_t e ) {
    assert( nullptr != m_t );
    static_cast<T*>( m_t )->OnHistoryError( m_tagUser, e );
  };

  void OnHistorySendDone() {
    assert( nullptr != m_t );
    static_cast<T*>( m_t )->OnHistorySendDone( m_tagUser );
  };

  void OnHistoryTickDataPoint( HistoryStructs::TickDataPoint* pDP ) {
    assert( nullptr != m_t );
    static_cast<T*>( m_t )->OnHistoryTickDataPoint( m_tagUser, pDP );
  };

  void OnHistoryIntervalData( HistoryStructs::Interval* pDP ) {
    assert( nullptr != m_t );
    static_cast<T*>( m_t )->OnHistoryIntervalData( m_tagUser, pDP );
  };

  void OnHistoryEndOfDayData( HistoryStructs::EndOfDay* pDP ) {
    assert( nullptr != m_t );
    static_cast<T*>( m_t )->OnHistoryEndOfDayData( m_tagUser, pDP );
  };

  void OnHistoryRequestDone( bool bStatus ) {
    assert( bStatus );
    assert( nullptr != m_t );
    static_cast<T*>( m_t )->OnHistoryRequestDone( m_tagUser );
  };

private:
  U m_tagUser;
  T* m_t;
  bool m_bActivated;
};

//
// =====================
//

template<typename T>  // T=CRTP based class
class HistoryBulkQuery {
public:

  struct structResultBar {
    std::string sSymbol;
    Bars bars;
    void Clear( void ) {
      sSymbol.clear();
      bars.Clear();
    };
  };

  struct structResultTicks {
    std::string sSymbol;
    Quotes quotes;  // quote added in sequence before trade
    Trades trades;
    void Clear( void ) {
      sSymbol.clear();
      quotes.Clear();
      trades.Clear();
    };
  };

  HistoryBulkQuery();
  virtual ~HistoryBulkQuery();

  template<typename Iter>
  void SetSymbols( Iter begin, Iter end );

  void SetMaxSimultaneousQueries( size_t n ) {
    assert( n > 0 );
    m_nMaxSimultaneousQueries = n;
  };
  size_t GetMaxSimultaneousQueries() const { return m_nMaxSimultaneousQueries; };

  // first of a series of requests to be built
  void DailyBars( size_t n );
  void Block() { boost::mutex::scoped_lock lock( m_mutexHistoryBulkQueryCompletion ); };

  void ReQueueBars( structResultBar* bars ) { bars->Clear(); m_reposBars.CheckInL( bars ); };
  void ReQueueTicks( structResultTicks* ticks ) { ticks->Clear(); m_reposTicks.CheckInL( ticks ); };

  struct structQueryState;  // forward reference
  using query_t = HistoryQueryTag<HistoryBulkQuery<T>, structQueryState*>;

  struct structQueryState {
    bool b;
    structResultBar* bars;  // one of bars or ticks will be used in any one session
    structResultTicks* ticks;
    query_t query;
    structQueryState( void )
      : bars( NULL ), ticks( NULL ), b( false )//, ix( 0 )
    {
      query.SetUserTag( this );
    };
  };

  void OnHistoryConnected( structQueryState* pqs );  // optional
  void OnHistoryDisconnected( structQueryState* pqs ); // optional
  void OnHistoryError( structQueryState* pqs, size_t e ); // optional
  void OnHistorySendDone( structQueryState* pqs ); // optional
  void OnHistoryTickDataPoint( structQueryState* pqs, ou::tf::iqfeed::HistoryStructs::TickDataPoint* pDP ); // for per tick processing
  void OnHistoryIntervalData( structQueryState* pqs, ou::tf::iqfeed::HistoryStructs::Interval* pDP ); // for per bar processing
  void OnHistoryEndOfDayData( structQueryState* pqs, ou::tf::iqfeed::HistoryStructs::EndOfDay* pDP ); // for per bar processing
  void OnHistoryRequestDone( structQueryState* pqs ); // for processing finished ticks, bars

  void OnCompletion();  // this needs to have an over ride to find out when all symbols are complete, needs to friend this class


protected:

  enum EProcessingState {
    Constructing, Quiescent, SymbolListBuilt, RetrievingWithMoreInQ, RetrievingWithQEmpty, InDestruction
    } m_stateBulkQuery;
  enum class EResultType {
    Unknown, Bars, Ticks
  } m_ResultType;

  // CRTP callbacks for inheriting class
  void OnBars( structResultBar* bars ) {
    //bars->Clear();
    ReQueueBars( bars );
  };
  void OnTicks( structResultTicks* ticks ) {
    //ticks->Clear();
    ReQueueTicks( ticks );
  };

  // CRTP based callbacks from HistoryQueryTag
private:

  using symbol_list_t = std::vector<std::string>;
  symbol_list_t m_listSymbols;
  size_t m_n;  // number of data points to retrieve

  ou::BufferRepository<structResultBar> m_reposBars;
  ou::BufferRepository<structResultTicks> m_reposTicks;

  int m_nMaxSimultaneousQueries;
  boost::atomic<int> m_nCurSimultaneousQueries;
  symbol_list_t::iterator m_iterSymbols;

  ou::BufferRepository<structQueryState> m_reposQueryStates;

  boost::mutex m_mutexHistoryBulkQueryCompletion;
  boost::mutex m_mutexProcessSymbolListScopeLock;

  void ProcessSymbolList();
  void GenerateQueries();

};

template <typename T>
HistoryBulkQuery<T>::HistoryBulkQuery()
:
  m_stateBulkQuery( EProcessingState::Constructing ),
  m_nMaxSimultaneousQueries( 10 ),
  m_nCurSimultaneousQueries( 0 ),
  m_ResultType( EResultType::Unknown )
{
  m_stateBulkQuery = EProcessingState::Quiescent;
}

template <typename T>
HistoryBulkQuery<T>::~HistoryBulkQuery() {
  assert( EProcessingState::Quiescent == m_stateBulkQuery );
}

template <typename T>
template<typename Iter>
void HistoryBulkQuery<T>::SetSymbols (Iter begin, Iter end ) {

  assert( EProcessingState::Quiescent == m_stateBulkQuery );

  m_listSymbols.clear();
  while ( begin != end ) {
    m_listSymbols.push_back( *begin );
    begin++;
  }
  std::sort( m_listSymbols.begin(), m_listSymbols.end() );
  m_stateBulkQuery = EProcessingState::SymbolListBuilt;

}

template <typename T>
void HistoryBulkQuery<T>::DailyBars( size_t n ) {
  m_n = n;
  m_ResultType = EResultType::Bars;
  GenerateQueries();
}

template <typename T>
void HistoryBulkQuery<T>::GenerateQueries() {
  assert( EProcessingState::SymbolListBuilt == m_stateBulkQuery );
  m_mutexHistoryBulkQueryCompletion.lock();
  m_nCurSimultaneousQueries = 0;
  m_iterSymbols = m_listSymbols.begin();
  ProcessSymbolList();  // startup first set of queries
}

/*
NOTE: 2020/12/23 IQFeed changed their request rates from 15 simultaneous requests:

The new method is a straightforward "requests per second" rate limiting method. It is a bucket limit system similar to the Linux kernel's iptables hashlimit. As a result, you
start with a full bucket of request credits. Each request consumes 1 credit and credits are re-allocated every few milliseconds unless you are already at max. If you send a request
when you are out of requests, you will get an error stating too many requests (the error message is the same as before). This new method allows all customers to have the same
limit. Also, customers can monitor the limit themselves, and know their real limit without trial and error

With all that said, the new limit for requests is 50/second (with one new credit added each 20ms).

IQFeedHistoryQuery currently has: m_nMillisecondsToSleep = 75

email: 12/23/20, 4:24 PM
forum: http://forums.iqfeed.net/index.cfm?page=topic&topicID=5832

*/

template <typename T>
void HistoryBulkQuery<T>::ProcessSymbolList() {
  boost::mutex::scoped_lock lock( m_mutexProcessSymbolListScopeLock );  // lock for the scope
  structQueryState* pqs;
  m_stateBulkQuery = EProcessingState::RetrievingWithMoreInQ;
  while ( ( m_nCurSimultaneousQueries.load( boost::memory_order_acquire ) < m_nMaxSimultaneousQueries ) && ( m_listSymbols.end() != m_iterSymbols ) ) {
    // generate another query
    m_nCurSimultaneousQueries.fetch_add( 1, boost::memory_order_acquire );
    // obtain a query state structure
    pqs = m_reposQueryStates.CheckOutL();
    if ( !pqs->query.Activated() ) {
      pqs->query.Activate();
      pqs->query.SetT( this );
      pqs->query.Connect();
    }

    switch ( m_ResultType ) {
      case EResultType::Ticks:
        pqs->ticks = m_reposTicks.CheckOutL();
        pqs->ticks->sSymbol = *m_iterSymbols;
        break;
      case EResultType::Bars:
        pqs->bars = m_reposBars.CheckOutL();
        pqs->bars->sSymbol = *m_iterSymbols;
        break;
    }

    // wait for query to reach connected state (do we need to do this anymore?)

    pqs->query.RetrieveNEndOfDays(*m_iterSymbols, m_n );
    ++m_iterSymbols;
  }

  if ( m_listSymbols.end() == m_iterSymbols ) {
    m_stateBulkQuery = EProcessingState::RetrievingWithQEmpty;
  }

  if ( 0 == m_nCurSimultaneousQueries.load( boost::memory_order_acquire ) ) { // no more queries outstanding so finish up
    m_stateBulkQuery = EProcessingState::Quiescent; // can now initiate another round of queries
    m_listSymbols.clear();
    static_cast<T*>( this )->OnCompletion();  // indicate total completion
    m_mutexHistoryBulkQueryCompletion.unlock();
  }
}

template <typename T>
void HistoryBulkQuery<T>::OnHistoryConnected( structQueryState* pqs ) {
}

template <typename T>
void HistoryBulkQuery<T>::OnHistoryDisconnected( structQueryState* pqs ) {
}

template <typename T>
void HistoryBulkQuery<T>::OnHistoryError( structQueryState* pqs, size_t e ) {
}

template <typename T>
void HistoryBulkQuery<T>::OnHistorySendDone( structQueryState* pqs ) {
}

template <typename T>
void HistoryBulkQuery<T>::OnHistoryTickDataPoint( structQueryState* pqs, ou::tf::iqfeed::HistoryStructs::TickDataPoint* pDP ) {

  Quote quote( pDP->DateTime, pDP->Bid, 0, pDP->Ask, 0 );
  pqs->ticks->quotes.Append( quote );
  Trade trade( pDP->DateTime, pDP->Last, pDP->LastSize );
  pqs->ticks->trades.Append( trade );

  if ( &HistoryBulkQuery<T>::OnHistoryTickDataPoint != &T::OnHistoryTickDataPoint ) {
    static_cast<T*>( this )->OnHistoryTickDataPoint( pqs, pDP );
  }

  pqs->query.ReQueueTickDataPoint( pDP );
}

template <typename T>
void HistoryBulkQuery<T>::OnHistoryIntervalData( structQueryState* pqs, ou::tf::iqfeed::HistoryStructs::Interval* pDP ) {

  Bar bar( pDP->DateTime, pDP->Open, pDP->High, pDP->Low, pDP->Close, pDP->PeriodVolume );
  pqs->bars->bars.Append( bar );

  if ( &HistoryBulkQuery<T>::OnHistoryIntervalData != &T::OnHistoryIntervalData ) {
    static_cast<T*>( this )->OnHistoryIntervalData( pqs, pDP );
  }

  pqs->query.ReQueueInterval( pDP );
}

template <typename T>
void HistoryBulkQuery<T>::OnHistoryEndOfDayData( structQueryState* pqs, ou::tf::iqfeed::HistoryStructs::EndOfDay* pDP ) {

  Bar bar( pDP->DateTime, pDP->Open, pDP->High, pDP->Low, pDP->Close, pDP->PeriodVolume );
  pqs->bars->bars.Append( bar );

  if ( &HistoryBulkQuery<T>::OnHistoryEndOfDayData != &T::OnHistoryEndOfDayData ) {
    static_cast<T*>( this )->OnHistoryEndOfDayData( pqs, pDP );
  }

  pqs->query.ReQueueEndOfDay( pDP );
}

template <typename T>
void HistoryBulkQuery<T>::OnHistoryRequestDone( structQueryState* pqs ) {

  if ( &HistoryBulkQuery<T>::OnHistoryRequestDone != &T::OnHistoryRequestDone ) {
    static_cast<T*>( this )->OnHistoryRequestDone( pqs );
  }

  // clean up.
  switch ( m_ResultType ) {
    case EResultType::Ticks:
      static_cast<T*>( this )->OnTicks( pqs->ticks );  // structure is reclaimed later
      pqs->ticks = NULL;
      break;
    case EResultType::Bars:
      static_cast<T*>( this )->OnBars( pqs->bars );  // structure is reclaimed later
      pqs->bars = NULL;
      break;
  }

  pqs->b = false;
  m_reposQueryStates.CheckInL( pqs );
  m_nCurSimultaneousQueries.fetch_sub( 1, boost::memory_order_release );
  ProcessSymbolList();
}

} // namespace iqfeed
} // namespace tf
} // namespace ou


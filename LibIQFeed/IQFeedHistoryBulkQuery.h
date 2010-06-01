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

// processes a series of historical data requests against the IQFeed API

#include <string>
#include <sstream>
#include <vector>
#include <cassert>

#include <algorithm>

#include <boost/foreach.hpp>

#include <LibCommon/ReusableBuffers.h>
#include <LibTimeSeries/TimeSeries.h>
#include <LibTrading/InstrumentFile.h>
#include <LibIQFeed/IQFeedHistoryQuery.h>

template <typename T, typename U>
class CIQFeedHistoryQueryTag: CIQFeedHistoryQuery<CIQFeedHistoryQueryTag<T,U> > {
  friend CIQFeedHistoryQuery<CIQFeedHistoryQueryTag<T,U> >;
public:
  typedef typename CIQFeedHistoryQuery<CIQFeedHistoryQueryTag<T,U> > inherited_t;
  CIQFeedHistoryQueryTag( T* t = NULL ) : m_t( t ) {
  };

  CIQFeedHistoryQueryTag( T* t, U tagUser ) : m_t( t ), m_tagUser( tagUser ) {
  };

  ~CIQFeedHistoryQueryTag( void ) {
  };

  void SetUserTag( U tagUser ) { m_tagUser = tagUser; };
  U GetUserTag( void ) { return m_tagUser; };

  void SetT( T* t ) { m_t = t; };
  T* GetT( void ) { return m_t; };

  void Activate( void ) { m_bActivated = true; };
  bool Activated( void ) { return m_bActivated; };

protected:

  // CRTP based callbacks;
  void OnHistoryConnected( void ) {
    assert( NULL != m_t );
    if ( &CIQFeedHistoryQueryTag<T,U>::OnHistoryConnected != &T::OnHistoryConnected ) {
      static_cast<T*>( m_t )->OnHistoryConnected( m_tagUser );
    }
  };

  void OnHistoryDisconnected( void ) {
    assert( NULL != m_t );
    if ( &CIQFeedHistoryQueryTag<T,U>::OnHistoryDisconnected != &T::OnHistoryDisconnected ) {
      static_cast<T*>( m_t )->OnHistoryDisconnected( m_tagUser );
    }
  };

  void OnHistoryError( size_t e ) {
    assert( NULL != m_t );
    if ( &CIQFeedHistoryQueryTag<T,U>::OnHistoryError != &T::OnHistoryError ) {
      static_cast<T*>( m_t )->OnHistoryError( m_tagUser, e );
    }
  };

  void OnHistorySendDone( void ) {
    assert( NULL != m_t );
    if ( &CIQFeedHistoryQueryTag<T,U>::OnHistorySendDone != &T::OnHistorySendDone ) {
      static_cast<T*>( m_t )->OnHistorySendDone( m_tagUser );
    }
  };

  void OnHistoryTickDataPoint( structTickDataPoint* pDP ) {
    assert( NULL != m_t );
    if ( &CIQFeedHistoryQueryTag<T,U>::OnHistoryTickDataPoint != &T::OnHistoryTickDataPoint ) {
      static_cast<T*>( m_t )->OnHistoryTickDataPoint( m_tagUser, pDP );
    }
  };

  void OnHistoryIntervalData( structInterval* pDP ) {
    assert( NULL != m_t );
    if ( &CIQFeedHistoryQueryTag<T,U>::OnHistoryIntervalData != &T::OnHistoryIntervalData ) {
      static_cast<T*>( m_t )->OnHistoryIntervalData( m_tagUser, pDP );
    }
  };

  void OnHistorySummaryData( structSummary* pDP ) {
    assert( NULL != m_t );
    if ( &CIQFeedHistoryQueryTag<T,U>::OnHistorySummaryData != &T::OnHistorySummaryData ) {
      static_cast<T*>( m_t )->OnHistorySummaryData( m_tagUser, pDP );
    }
  };

  void OnHistoryRequestDone( void ) {
    assert( NULL != m_t );
    if ( &CIQFeedHistoryQueryTag<T,U>::OnHistoryRequestDone != &T::OnHistoryRequestDone ) {
      static_cast<T*>( m_t )->OnHistoryRequestDone( m_tagUser );
    }
  };

private:
  U m_tagUser;
  T* m_t;
  bool m_bActivated;
};

template <typename T>
class CIQFeedHistoryBulkQuery {
public:

  struct structResultBar {
    std::string sSymbol;
    CBars bars;
    void Clear( void ) {
      sSymbol.Clear();
      bars.Clear();
    };
  };

  struct structResultTicks {
    std::string sSymbol;
    CQuotes quotes;  // quote added in sequence before trade
    CTrades trades;
    void Clear( void ) {
      sSymbol.Clear();
      quotes.Clear();
      trades.Clear();
    };
  };

  CIQFeedHistoryBulkQuery( void );
  virtual ~CIQFeedHistoryBulkQuery( void );

  // use one or the other of SetExchanges or SetSymbols
  typedef std::vector<std::string> exchange_list_t;
  void SetExchanges( const &exchange_list_t exchanges );

  typedef std::vector<std::string> symbol_list_t;
  void SetSymbols( const &symbol_list_t symbols );

  void SetMaxSimultaneousQueries( size_t n ) { 
    assert( n > 0 );
    m_nMaxSimultaneousQueries = n; 
  };
  size_t GetMaxSimultaneousQueries( void ) { return m_nMaxSimultaneousQueries; };

  // first of a series of requests to be built
  void DailyBars( size_t n );

protected:

  enum enumProcessingState {
    EConstructing, EQuiescent, ESymbolListBuilt, ERetrievingWithMoreInQ, ERetrievingWithQEmpty, EInDestruction
    } m_stateBulkQuery;

  void GenerateQueries( void );
  void ProcessSymbolList( void );

private:

  symbol_list_t m_listSymbols;
  size_t m_n;  // number of data points to retrieve

  size_t m_nMaxSimultaneousQueries;
  size_t m_nCurSimultaneousQueries;
  symbol_list_t::iterator m_iterSymbols;

  struct structQueryState;  // empty declaration for circular reference
  typedef typename CIQFeedHistoryQueryTag<CIQFeedHistoryBulkQuery<T>, structQueryState*> query_t;

  struct structQueryState {
    size_t ix;  // index into vector containing this structure
    structResultBar* bars;  // one of bars or ticks will be used in any one session
    structResultTicks* ticks;
    query_t query;
    void Clear( void ) {
      bars->Clear();
      ticks->Clear();
    }
    structQueryState( void ) {
      query.SetUserTag( this );
    };
  };

  CBufferRepository<structQueryState> m_reposQueryStates;

  // CRTP based callbacks from IQFeedHistoryQuery
  void OnHistoryConnected( structQueryState* pqs ) {};
  void OnHistoryDisconnected( structQueryState* pqs ) {};
  void OnHistoryError( structQueryState* pqs, size_t e ) {};
  void OnHistorySendDone( structQueryState* pqs ) {};
  void OnHistoryTickDataPoint( structQueryState* pqs, structTickDataPoint* pDP ) {};
  void OnHistoryIntervalData( structQueryState* pqs, structInterval* pDP ) {};
  void OnHistorySummaryData( structQueryState* pqs, structSummary* pDP ) {};
  void OnHistoryRequestDone( structQueryState* pqs ) {};
};

template <typename T>
CIQFeedHistoryBulkQuery<T>::CIQFeedHistoryBulkQuery( void ) 
: 
  m_stateBulkQuery( EConstructing ),
  m_nMaxSimultaneousQueries( 10 ),
  m_nCurSimultaneousQueries( 0 )
{
  m_stateBulkQuery( EQuiescent );
}

template <typename T>
CIQFeedHistoryBulkQuery<T>::~CIQFeedHistoryBulkQuery() {
  assert( EQuiescent == m_stateBulkQuery );
}

template <typename T>
void CIQFeedHistoryBulkQuery<T>::BulkQueryExchanges(const int &exchange_list_t list) {

  assert( EQuiescent == m_stateProcessing );

  CInstrumentFile file;
  CInstrumentFile::iterator iterSymbols;

  structSymbolRecord *pRec;

  file.OpenIQFSymbols();
  iterSymbols.SetInstrumentFile( &file );
  m_vSymbols.clear();

  BOOST_FOREACH( std::string s, list ) {
    pRec = iterSymbols.begin( s );
    while ( iterSymbols.end() != pRec ) {
      m_vSymbols.push_back( pRec->GetSymbol() );
      pRec = ++iterSymbols;
    }
  }

  file.CloseIQFSymbols();

  std::sort( m_vSymbols.begin(), m_vSymbols.end() );
  m_stateBulkQuery = ESymbolListBuilt;
}

template <typename T>
void CIQFeedHistoryBulkQuery<T>::BulkQuerySymbols(const int &symbol_list_t list) {

  assert( EQuiescent == m_stateBulkQuery );

  m_vSymbols.clear();
  BOOST_FOREACH( std::string s, list ) {
    m_vSymbols.push_back( list );
  }
  std::sort( m_vSymbols.begin(), m_vSymbols.end() );
  m_stateBulkQuery = ESymbolListBuilt;

}

templet <typename T>
void CIQFeedHistoryBulkQuery<T>::DailyBars( size_t n ) {
  m_n = n;
  GenerateQueries();
}

template <typename T>
void CIQFeedHistoryBulkQuery<T>::GenerateQueries( void ) {
  assert( ESymbolListBuilt == m_stateBulkQuery );
  m_nCurSimultaneousQueries = 0;
  m_iterSymbols = m_listSymbols.begin();
  ProcessSymbolList();  // startup first set of queries
}

template <typename T>
void CIQFeedHistoryBulkQuery<T>::ProcessSymbolList( void ) {
  size_t ix;
  structQueryState* pqs;
  m_stateBulkQuery = ERetrievingWithMoreInQ;  // set default state
  while ( ( m_nCurSimultaneousQueries < m_nMaxSimultaneousQueries ) && ( m_listSymbols.end() != m_iterSymbols ) ) {
    // generate another query
    ++m_nCurSimultaneousQueries;
    // obtain a query state structure
    pqs = m_reposQueryStates.CheckOutL;
    if ( !pqs->query.Activated() ) {
      pqs->query.SetT( this );
      pqs->query.Connect();
    }
    
    // wait for query to reach connected state (do we need to do this anymore?)

    pqs->query.RetrieveNEndOfDays(*m_iterSymbols, m_n );
    }
  }

  if ( m_listSymbols.end() == m_iterSymbols ) {
    m_stateBulkQuery = ERetrievingWithQEmpty; 
  }

  if ( 0 == m_nCurSimultaneousQueries ) { // no more queries outstanding so finish up
    m_stateBulkQuery = ESymbolListBuilt; // can now initiate another round of queries
  }
  else {
  }
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnHistoryConnected( structQueryState* pqs ) {
  ProcessSymbolList();
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnHistoryDisconnected( structQueryState* pqs ) {
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnHistoryError( structQueryState* pqs, size_t e ) {
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnHistorySendDone( structQueryState* pqs ) {
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnHistoryTickDataPoint( structQueryState* pqs, structTickDataPoint* pDP ) {
  pDp->ReQueueTickDataPoint( pDP );
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnHistoryIntervalData( structQueryState* pqs, structInterval* pDP ) {
  pDp->ReQueueSummary( pDP );
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnHistorySummaryData( structQueryState* pqs, structSummary* pDP ) {
  pDp->ReQueueInterval( pDP );
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnHistoryRequestDone( structQueryState* pqs ) {
  m_reposQueryStates.CheckInL( pqs );
  ProcessSymbolList();
}



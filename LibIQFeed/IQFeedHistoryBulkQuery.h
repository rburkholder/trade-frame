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

#include <algorithms>

#include <boost/foreach.hpp>
#include <boost/thread/barrier.hpp>

#include <codeproject/thread.h>  // class inbound messages

#include <LibTrading/InstrumentFile.h>
#include <LibIQFeed/IQFeedHistoryQuery.h>

template <typename T>
class CIQFeedHistoryBulkQuery: public CGuiThreadImpl<CIQFeedHistoryBulkQuery<T> > {
public:

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

  struct structResultBar {
    std::string sSymbol;
    // CBars bars;
  };

  struct structResultTicks {
    std::string sSymbol;
    // CQuotes Quotes;  // quote added in sequence before trade
    // CTrades Trades;
  };

  CIQFeedHistoryBulkQuery( CAppModule* pModule );
  ~CIQFeedHistoryBulkQuery( void );

  BOOL InitializeThread( void );
  void CleanupThread( DWORD );

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

  void DailyBars( size_t n, const structMessageDestinations& messages );

protected:
  enum enumLocalMessages {  // messages sent from CIQFeedHistoryQuery
    WM_BQ_CONNECTED = WM_USER + 1,
    WM_BQ_ERROR, 
    WM_BQ_TICK,
    WM_BQ_INTERVAL,
    WM_BQ_SUMMARY,
    WM_BQ_DONE,
    WM_BQ_SENDCOMPLETE,
    WM_BQ_DISCONNECTED,

    // get everything closed down prior to PostQuitMessage
    WM_BQ_PRE_QUIT,

    // unused here, derived can use it for start of its messages
    WM_BQ_ENDMARKER
  };

  BEGIN_MSG_MAP_EX(CIQFeedHistoryBulkQuery<T>)
    MESSAGE_HANDLER(WM_BQ_CONNECTED, OnQueryConnected);
    MESSAGE_HANDLER(WM_BQ_ERROR, OnQueryError);
    MESSAGE_HANDLER(WM_BQ_TICK, OnQueryDataPoint);
    MESSAGE_HANDLER(WM_BQ_INTERVAL, OnQueryInterval);
    MESSAGE_HANDLER(WM_BQ_SUMMARY, OnQuerySummary);
    MESSAGE_HANDLER(WM_BQ_DONE, OnQueryDoned);
    MESSAGE_HANDLER(WM_BQ_SENDCOMPLETE, OnQuerySendComplete);
    MESSAGE_HANDLER(WM_BQ_DISCONNECTED, OnQueryDisConnected);
    MESSAGE_HANDLER(WM_BQ_PRE_QUIT, OnPreQuit )
  END_MSG_MAP()

  enum enumProcessingState {
    EConstructing, EQuiescent, ESymbolListBuilt, ERetrievingWithMoreInQ, ERetrievingWithQEmpty, EInDestruction
    } m_stateBulkQuery;

  void GenerateQueries( void );
  void ProcessSymbolList( void );

private:
  CAppModule* m_pModule;
  boost::barrier m_barrier;  

  symbol_list_t m_listSymbols;
  structMessageDestinations m_messageDestinations;
  size_t m_n;  // number of data points to retrieve

  size_t m_nMaxSimultaneousQueries;
  size_t m_nCurSimultaneousQueries;
  symbol_list_t::iterator m_iterSymbols;

  typedef typename CIQFeedHistoryQuery<CIQFeedHistoryBulkQuery<T> > query_t;

  struct structQueryState {
    size_t ix;  // index into vector containing this structure
    structResultBar* bars;
    structResultTicks* ticks;
    query_t* pq;
    structQueryState(CAppModule* pModule, const query_t::structMessageDestinations& MessageDestinations) 
      : ix(0) 
    {
      pq = new query_t( pModule, MessageDestinations );
    };
    ~structQueryState(void) {
      delete pq;
    };
  };

  std::vector<structQueryState*> m_vQueryStates;  // all our usable queries
  std::vector<size_t> m_stkSpareQueries;  // stack of spare re-usable query indexes into m_vQueryStates;

  query_t::structMessageDestinations m_idsQueryMessage;

  LRESULT OnQueryConnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnQueryError( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnQueryDataPoint( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnQueryInterval( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnQuerySummary( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnQueryDone( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnQuerySendComplete( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnQueryDisConnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnPreQuit( UINT, WPARAM, LPARAM, BOOL &bHandled );

  void PostMessage( UINT id, WPARAM wparam = NULL, LPARAM lparam = NULL ) {
    PostThreadMessage( id, wparam, lparam );
  }
};

template <typename T>
CIQFeedHistoryBulkQuery<T>::CIQFeedHistoryBulkQuery(WTL::CAppModule *pModule) 
: CGuiThreadImpl<CIQFeedHistoryBulkQuery<T> >( pModule, CREATE_SUSPENDED ),
  m_barrier( 2 ),  // 2 threads need to meet
  m_pModule( pModule ),
  m_nMaxSimultaneousQueries( 10 ),
  m_nCurSimultaneousQueries( 0 ),
  m_QueryMessageIds( this, WM_BQ_CONNECTED, WM_BQ_SENDCOMPLETE, WM_BQ_DISCONNECTED, WM_BQ_ERROR, 
    WM_BQ_TICK, WM_BQ_INTERVAL, WM_BQ_SUMMARY, WM_BQ_DONE ),
  m_stateBulkQuery( EConstructing );
{
  this->Resume();
  m_barrier.wait();  // sync up with InitializeThread
  m_stateProcessing( EQuiescent );
}

template <typename T>
CIQFeedHistoryBulkQuery<T>::~CIQFeedHistoryBulkQuery() {

  PostThreadMessage( WM_BQ_PRE_QUIT );
  Join();  
}

template <typename T>
BOOL CIQFeedHistoryBulkQuery<T>::InitializeThread() {

  m_barrier.wait();  // sync up with constructor

  return TRUE;
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnPreQuit( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
  if ( EQuiescent == m_stateBulkQuery ) {
    PostQuitMessage();
  }
  else {
    Sleep(10); // dally for a bit
    PostThreadMessage( WM_BQ_PRE_QUIT );
  }
  bHandled = true;
  return 1;
}

template <typename T>
void CIQFeedHistoryBulkQuery<T>::CleanupThread(DWORD) {
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
void CIQFeedHistoryBulkQuery<T>::DailyBars( size_t n, const structMessageDestinations& messages ) {
  m_n = n;
  structMessageDestinations = messages;
  GenerateQueries();
}

template <typename T>
void CIQFeedHistoryBulkQuery<T>::GenerateQueries( void ) {
  assert( ESymbolListBuilt == m_stateBulkQuery );
  m_nCurSimultaneousQueries = 0;
  m_iterSymbols = m_listSymbols.begin();
  ProcessSymbolList();
}

template <typename T>
void CIQFeedHistoryBulkQuery<T>::ProcessSymbolList( void ) {
  size_t ix;
  structQueryState* pqs;
  m_stateBulkQuery = ERetrievingWithMoreInQ;  // set default state
  while ( ( m_nCurSimultaneousQueries < m_nMaxSimultaneousQueries ) && ( m_listSymbols.end() != m_iterSymbols ) ) {
    // generate another query
    // obtain a query state structure
    if ( m_stkSpareQueries.empty() ) {
      pqs = new structQueryState( m_pModule, m_idsQueryMessage );
      //pqs->ix = ix = m_vQueryStates.size();
      //m_vQueryStates.push_back( pq );
      // wait for query to reach connected state
    }
    else {
      ++m_nCurSimultaneousQueries;
      ix = m_stkSpareQueries.back();
      m_stkSpareQueries.pop_back();
      pqs = m_vQueryStates[ ix ];
      pqs->RetrieveNEndOfDays(*m_iterSymbols, m_n, reinterpret_cast<LPARAM>( pqs ) );
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
LRESULT CIQFeedHistoryBulkQuery<T>::OnQueryConnected( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
  ProcessSymbolList();
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnQueryError( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnQueryDataPoint( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnQueryInterval( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnQuerySummary( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnQueryDone( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnQuerySendComplete( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}

template <typename T>
LRESULT CIQFeedHistoryBulkQuery<T>::OnQueryDisConnected( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}



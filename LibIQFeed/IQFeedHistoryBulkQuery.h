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

#include <codeproject/thread.h>  // class inbound messages

#include <LibTrading/InstrumentFile.h>

template <typename T>
class CIQFeedHistoryBulkQuery: public CGuiThreadImpl<CIQFeedHistoryBulkQuery<T> > {
public:
  CIQFeedHistoryBulkQuery( CAppModule* pModule );
  ~CIQFeedHistoryBulkQuery( void );

  BOOL InitializeThread( void );
  void CleanupThread( DWORD );

  typedef std::vector<std::string> exchange_list_t;
  void BulkQueryExchanges( const &exchange_list_t exchanges );

  typedef std::vector<std::string> symbol_list_t;
  void BulkQuerySymbols( const &symbol_list_t symbols );

protected:
  enum enumMessages {  // messages sent from CIQFeedHistoryQuery
    WM_BQ_CONNECTED = WM_USER + 1,
    WM_BQ_ERROR, 
    WM_BQ_TICK,
    WM_BQ_INTERVAL,
    WM_BQ_SUMMARY,
    WM_BQ_DONE,
    WM_BQ_SENDCOMPLETE,
    WM_BQ_DISCONNECTED,

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
  END_MSG_MAP()

private:
  CAppModule* m_pModule;

  symbol_list_t m_listSymbols;

  LRESULT OnQueryConnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnQueryError( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnQueryDataPoint( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnQueryInterval( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnQuerySummary( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnQueryDone( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnQuerySendComplete( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnQueryDisConnected( UINT, WPARAM, LPARAM, BOOL &bHandled );

};

template <typename T>
CIQFeedHistoryBulkQuery<T>::CIQFeedHistoryBulkQuery(WTL::CAppModule *pModule) {
}

template <typename T>
CIQFeedHistoryBulkQuery<T>::~CIQFeedHistoryBulkQuery() {
}

template <typename T>
CIQFeedHistoryBulkQuery::InitializeThread() {
}

template <typename T>
CIQFeedHistoryBulkQuery::CleanupThread(DWORD) {
}

template <typename T>
CIQFeedHistoryBulkQuery::BulkQueryExchanges(const int &exchange_list_t list) {
  CInstrumentFile file;
  CInstrumentFile::iterator iterSymbols;

  structSymbolRecord *pRec;

  file.OpenIQFSymbols();
  iterSymbols.SetInstrumentFile( &file );

  BOOST_FOREACH( std::string s, list ) {
    pRec = iterSymbols.begin( s );
    while ( iterSymbols.end() != pRec ) {
      m_vSymbols.push_back( pRec->GetSymbol() );
      pRec = ++iterSymbols;
    }
  }

  file.CloseIQFSymbols();

  std::sort( m_vSymbols.begin(), m_vSymbols.end() );

}

template <typename T>
CIQFeedHistoryBulkQuery::BulkQuerySymbols(const int &symbol_list_t list) {
}

template <typename T>
CIQFeedHistoryBulkQuery::OnQueryConnected( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}

template <typename T>
CIQFeedHistoryBulkQuery::OnQueryError( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}

template <typename T>
CIQFeedHistoryBulkQuery::OnQueryDataPoint( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}

template <typename T>
CIQFeedHistoryBulkQuery::OnQueryInterval( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}

template <typename T>
CIQFeedHistoryBulkQuery::OnQuerySummary( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}

template <typename T>
CIQFeedHistoryBulkQuery::OnQueryDone( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}

template <typename T>
CIQFeedHistoryBulkQuery::OnQuerySendComplete( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}

template <typename T>
CIQFeedHistoryBulkQuery::OnQueryDisConnected( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
}



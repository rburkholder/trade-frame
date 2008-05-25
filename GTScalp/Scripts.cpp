#include "StdAfx.h"
#include "Scripts.h"

#include "IQFeedSymbolFile.h"

#include "HDF5TimeSeriesContainer.h"
#include "SymbolSelectionFilter.h"

#include "HDF5DataManager.h"
using namespace H5;

#include <stdexcept>
#include <algorithm>
#include <functional>
#include <string>
using namespace std;

//
// CScripts
//

CScripts::CScripts(void) 
: m_pProvider( NULL ) 
{
}

CScripts::~CScripts(void) {
  if ( NULL != m_pProvider ) {
    delete m_pProvider;  // needs to be disconnected before this though.
    m_pProvider = NULL;
  }
}

void CScripts::GetIQFeedHistoryForSymbol( char *szSymbol, EHistoryType typeHistory, unsigned long nDays ) {
  CHistoryCollector *phc;
  switch ( typeHistory ) {
        case Daily:
          phc = new CHistoryCollectorDaily( m_pProvider, szSymbol, nDays );
          break;
        case Tick:
          phc = new CHistoryCollectorTicks( m_pProvider, szSymbol, nDays );
          break;
        case Minute:
          break;
  }
  m_qHistoryCollectors.push( phc );
  StartHistoryCollection();  // start what collectors we can while still building up the queue
}

void CScripts::GetIQFeedHistoryForSymbolRange( EHistoryType typeHistory, unsigned long nDays ) {
  // process IQFSymbol Table for exchanges and retrieve associated symbols
  //_CrtMemCheckpoint( &memstate1 );
  //ofs.open( "archive.arc", std::ios::out | std::ios::binary );

  CIQFeedSymbolFile symbolfile;
  symbolfile.Open();
  cout << endl;

  // with list of symbols, initiate history retrieval of daily bars
  int cntSymbols = 0;
  //const char *szExchanges[] = { "ONECH", "NYSE", "AMEX", "NMS", "DTN", "" };
  const char *szExchanges[] = { "NYSE", "AMEX", "NMS", "" };
  //const char *szExchanges[] = { "PBOT", "" };
  int ixExchanges = 0;
  const char *szExchange = szExchanges[ixExchanges];
  while ( 0 != *szExchange ) {
    symbolfile.SetSearchExchange( szExchange );
    bool bSymbolFound = symbolfile.RetrieveSymbolRecord( DB_SET );
    while ( bSymbolFound ) {
      const char *szSymbol = symbolfile.GetSymbol();
      //cout << szSymbol << " ";
      if ( !symbolfile.GetBitMutual() && !symbolfile.GetBitMoneyMkt() ) {
        //cout << "useful";
        CHistoryCollector *phc;
        switch ( typeHistory ) {
        case Daily:
          phc = new CHistoryCollectorDaily( m_pProvider, szSymbol, nDays );
          break;
        case Tick:
          phc = new CHistoryCollectorTicks( m_pProvider, szSymbol, nDays );
          break;
        case Minute:
          break;
        }
        m_qHistoryCollectors.push( phc );
        StartHistoryCollection();  // start what collectors we can while still building up the queue
        ++cntSymbols;
      }
      else {
        //cout << "n/a";
      }
      //cout << endl;
      bSymbolFound = symbolfile.RetrieveSymbolRecord( DB_NEXT_DUP );
    }
    symbolfile.EndSearch();
    szExchange = szExchanges[ ++ixExchanges ];
  }
  cout << "#Symbols: " << cntSymbols << ", bar count: " << nDays << endl;
  //if ( 0 < i ) StartHistoryCollection();

  // close out files
  symbolfile.Close();
}

class ShowItem: public std::unary_function<CBar &, void> {
public:
  //ShowItem( void );
  //~ShowItem( void );
  void operator()( CBar &bar ) {
    cout << "bar is " << bar.m_dblOpen << endl;
  }
protected:
private:
};

void CScripts::StartHistoryCollection( void ) {
  CHistoryCollector *phc;
  while ( ( m_mapActiveHistoryCollectors.size() < nMaxActiveCollectors ) && !m_qHistoryCollectors.empty() ) {
    phc = m_qHistoryCollectors.front();
    m_qHistoryCollectors.pop();
    m_mapActiveHistoryCollectors.insert( m_pair_mapActiveHistoryCollectors( phc->Symbol(), phc ) );
    phc->SetOnRetrievalComplete( MakeDelegate( this, &CScripts::HistoryCollectorIsComplete ) );
    phc->Start();
  }
}

void CScripts::HistoryCollectorIsComplete( CHistoryCollector *phc ) {
  
  map<string, CHistoryCollector *>::iterator m_iter_mapActiveHistoryCollectors;
  m_iter_mapActiveHistoryCollectors = m_mapActiveHistoryCollectors.find( phc->Symbol());
  if ( m_mapActiveHistoryCollectors.end() == m_iter_mapActiveHistoryCollectors ) {
    throw runtime_error( "History Collector iterator should not be empty" );
  }
  else {
    m_mapActiveHistoryCollectors.erase( m_iter_mapActiveHistoryCollectors );
    StartHistoryCollection();  // start a new one while we process this one
  }

  // send data to file storage
  try {
    phc->WriteData();
  }
  catch (...) {
    cout << "exception on writing" << endl;
  }
  // clean up
  delete phc;

  // final reporting
  if ( m_qHistoryCollectors.empty() && !m_mapActiveHistoryCollectors.empty() ) {
    cout << "active = " << m_mapActiveHistoryCollectors.size() << endl;
    map<string, CHistoryCollector *>::iterator iter = m_mapActiveHistoryCollectors.begin();
    while ( m_mapActiveHistoryCollectors.end() != iter ) {
      cout << "  symbol remaining: " << iter->first << endl;
      ++iter;
    }
    //_CrtDumpMemoryLeaks();
  }
  if ( m_qHistoryCollectors.empty() && m_mapActiveHistoryCollectors.empty() ) {
    cout << "History processing queue has been finished." << endl;
    //_CrtMemCheckpoint( &memstate2 );
    //int val = _CrtMemDifference( &memstate3, &memstate1, &memstate2 );
    //_CrtMemDumpStatistics( &memstate3 );
    //_CrtMemDumpAllObjectsSince( &memstate1 );

    delete this;
  }
}

// need to check queue as didn't get the queue has finished message


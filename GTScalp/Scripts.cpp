#include "StdAfx.h"

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <string>
using namespace std;

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>
using namespace H5;

#include <TFIQFeed/IQFeedSymbolFile.h>

#include "SymbolSelectionFilter.h"

#include "Scripts.h"

using namespace ou::tf;

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

//
// CScripts
//

CScripts::CScripts(void) {
}

CScripts::~CScripts(void) {
}

void CScripts::GetIQFeedHistoryForSymbol( char *szSymbol, EHistoryType typeHistory, unsigned long nDays ) {
  CHistoryCollector *phc;
  switch ( typeHistory ) {
        case Daily:
          phc = new CHistoryCollectorDaily( m_Provider.GetIQFeedProvider(), szSymbol, nDays );
          break;
        case Tick:
          phc = new CHistoryCollectorTicks( m_Provider.GetIQFeedProvider(), szSymbol, nDays );
          break;
        case Minute:
          break;
  }
  m_qHistoryCollectors.push( phc );
  StartHistoryCollection();  // start what collectors we can while still building up the queue
}

// need a method for delving into the left over data structures when a symbol doesn't complete
//  any records recieved?  what were they?  where are we in the command hand shaking?

void CScripts::GetIQFeedHistoryForSymbolRange( EHistoryType typeHistory, unsigned long nDays ) {
  // process IQFSymbol Table for exchanges and retrieve associated symbols
  //_CrtMemCheckpoint( &memstate1 );
  //ofs.open( "archive.arc", std::ios::out | std::ios::binary );

  CIQFeedSymbolFile symbolfile;
  symbolfile.OpenIQFSymbols();
  cout << endl;

  // with list of symbols, initiate history retrieval of daily bars
  int cntSymbols = 0;
  //const char *szExchanges[] = { "ONECH", "NYSE", "AMEX", "NMS", "DTN", "" };
  const char *szExchanges[] = { "NYSE", "AMEX", "NMS", "" };
  //const char *szExchanges[] = { "PBOT", "" };
  int ixExchanges = 0;
  const char *szExchange = szExchanges[ixExchanges];
  //std::vector<string> v;
  //std::vector<string>::iterator i;
  while ( 0 != *szExchange ) {
    symbolfile.SetSearchExchange( szExchange );
    bool bSymbolFound = symbolfile.RetrieveSymbolRecordByExchange( DB_SET );
    while ( bSymbolFound ) {
      const char *szSymbol = symbolfile.GetSymbol();
//      if ( ( 'C' == *szSymbol ) 
//        || ( 0 == strcmp( szSymbol, "PDT" ) )
//        || ( 0 == strcmp( szSymbol, "NBXH" ) )
//        || ( 0 == strcmp( szSymbol, "LBC" ) )
//        || ( 0 == strcmp( szSymbol, "HGR" ) )
//        || ( 0 == strcmp( szSymbol, "CIT" ) )
//        || ( 0 == strcmp( szSymbol, "BOH" ) )
//        || ( 0 == strcmp( szSymbol, "ARTC" ) ) ) {
      CInstrumentFile::bitsSymbolClassifier_t sc;
      sc.reset();
      sc |= symbolfile.GetSymbolClassifier();
      if ( !sc.test( CInstrumentFile::Mutual )
        && !sc.test( CInstrumentFile::MoneyMarket )
        && sc.test( CInstrumentFile::HasOptions )
        && !sc.test( CInstrumentFile::NotAStock ) ) { // could probably use this flag by itself
          CHistoryCollector *phc;
          switch ( typeHistory ) {
             case Daily:
               phc = new CHistoryCollectorDaily( m_Provider.GetIQFeedProvider(), szSymbol, nDays );
               break;
             case Tick:
               phc = new CHistoryCollectorTicks( m_Provider.GetIQFeedProvider(), szSymbol, nDays );
               break;
             case Minute:
               break;
          }
          m_qHistoryCollectors.push( phc );
          StartHistoryCollection();  // start what collectors we can while still building up the queue

          //std::cout << "  \"" << szSymbol << "\"," << std::endl;
          //i = v.begin();
          //string s( szSymbol );
          //v.insert(i, s );

          ++cntSymbols;
        }
        else {
        }
//      }
      bSymbolFound = symbolfile.RetrieveSymbolRecordByExchange( DB_NEXT_DUP );
    }
    symbolfile.EndSearch();
    szExchange = szExchanges[ ++ixExchanges ];
  }
  //for ( std::vector<string>::iterator j=v.begin(); j < v.end(); j++ ) {
  //  std::cout << "  \"" << *j << "\"," << std::endl;
  //}
  cout << "#Symbols: " << cntSymbols << ", bar count: " << nDays << endl;

  // close out files
  symbolfile.CloseIQFSymbols();
}

class ShowItem: public std::unary_function<Bar&,void> {
public:
  //ShowItem( void );
  //~ShowItem( void );
  void operator()( Bar& bar ) {
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
    //std::cout << "Start " << phc->Symbol() << std::endl;
    phc->Start();
  }
}

void CScripts::HistoryCollectorIsComplete( CHistoryCollector *phc ) {
  
  //std::cout << "End   " << phc->Symbol() << std::endl;
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
    if ( 1 == m_mapActiveHistoryCollectors.size() ) {
      std::cout << "one element left" << std::endl;
      // is there still anything in buffers?  what is the last symbol?
      // examine m_mapActiveHistoryCollectors to see can see what we have recieved.
    }
    //_CrtDumpMemoryLeaks();
  }
  if ( m_qHistoryCollectors.empty() && m_mapActiveHistoryCollectors.empty() ) {
    cout << "History processing queue has been finished." << endl;
    // check that all buffers are empty *****
    //_CrtMemCheckpoint( &memstate2 );
    //int val = _CrtMemDifference( &memstate3, &memstate1, &memstate2 );
    //_CrtMemDumpStatistics( &memstate3 );
    //_CrtMemDumpAllObjectsSince( &memstate1 );

    delete this;
  }
}

// need to check queue as didn't get the queue has finished message


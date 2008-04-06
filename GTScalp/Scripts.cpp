#include "StdAfx.h"
#include "Scripts.h"

#include "IQFeedSymbolFile.h"

#include "HDF5TimeSeriesContainer.h"

#include <stdexcept>
#include <algorithm>
#include <functional>
#include <string>
using namespace std;

//
// CScripts
//

CScripts::CScripts(void) {
}

CScripts::~CScripts(void) {
}

void CScripts::GetIQFeedHistoryForSymbolRange( EHistoryType typeHistory, unsigned long nDays ) {
  // process IQFSymbol Table for exchanges and retrieve associated symbols

  //_CrtMemCheckpoint( &memstate1 );


  CIQFeedSymbolFile symbolfile;
  symbolfile.Open();

  //ofs.open( "archive.arc", std::ios::out | std::ios::binary );
  
  

  // with list of symbols, initiate history retrieval of daily bars
  int i = 0;
  const char szExchange[] = "PBOT";
  cout << endl;
  symbolfile.SetSearchExchange( szExchange );
    const char *szSymbol = symbolfile.GetSymbol( DB_SET );
    while ( NULL != szSymbol ) {
      //cout << szSymbol << endl;
      CHistoryCollector *phc;
      switch ( typeHistory ) {
        case Daily:
          phc = new CHistoryCollectorDaily( szSymbol, nDays );
          break;
        case Tick:
          phc = new CHistoryCollectorTicks( szSymbol, nDays );
          break;
        case Minute:
          break;
      }
      m_qHistoryCollectors.push( phc );
      StartHistoryCollection();  // start what collectors we can while still building up the queue
      ++i;
      szSymbol = symbolfile.GetSymbol( DB_NEXT_DUP );
    }
  
  symbolfile.EndSearch();
  cout << "#Symbols: " << i << endl;
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

void CScripts::TestDataSet( void ) {
  //DataSpace *pds = new DataSpace( dm.GetH5File()->

  string sFilename( "/bar/86400/SOX.X" );
  try {

    CHDF5TimeSeriesContainer<CBar> barRepository( sFilename );
    //CHDF5TimeSeriesContainer<CBar>::iterator iter;
    //iter = barRepository.begin();

    for_each( barRepository.begin(), barRepository.end(), ShowItem() );

  }
  catch ( ... ) {
    cout << "problems" << endl;
  }

}

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


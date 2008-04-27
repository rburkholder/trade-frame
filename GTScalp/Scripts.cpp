#include "StdAfx.h"
#include "Scripts.h"

#include "IQFeedSymbolFile.h"

#include "HDF5TimeSeriesContainer.h"
#include "SymbolSelectionFilter.h"

#include "DataManager.h"
using namespace H5;

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

void CScripts::GetIQFeedHistoryForSymbol( char *szSymbol, EHistoryType typeHistory, unsigned long nDays ) {
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
  cout << "#Symbols: " << cntSymbols << endl;
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

  string sFilename( "/bar/86400/I/C/ICE" );
  try {

    CHDF5TimeSeriesContainer<CBar> barRepository( sFilename );
    //CHDF5TimeSeriesContainer<CBar>::iterator iter;
    //iter = barRepository.begin();

    //for_each( barRepository.begin(), barRepository.end(), ShowItem() );
    //ptime dt(boost::date_time::special_values::not_a_date_time );
    //ptime dt( boost::gregorian::date( 2008, 04, 04 ), boost::posix_time::time_duration( 1, 1, 1 ) );
    ptime dt( boost::gregorian::date( 2008, 04, 18 ) );
    //CHDF5TimeSeriesContainer<CBar>::iterator iter;
    pair<CHDF5TimeSeriesContainer<CBar>::iterator, CHDF5TimeSeriesContainer<CBar>::iterator> p;
    //iter = find( barRepository.begin(), barRepository.end(), dt );
    //iter = lower_bound( barRepository.begin(), barRepository.end(), dt );

    p = equal_range( barRepository.begin(), barRepository.end(), dt );
    //iter = upper_bound( barRepository.begin(), barRepository.end(), dt );
    //if ( (const CHDF5TimeSeriesIterator<CBar> ) barRepository.end() == iter ) {
    if ( p.first == p.second ) {
      cout << "nothing found: insertion point is " << (*p.first).m_dt << endl;
    }
    else {
      cout << "found " << (*p.first).m_dt << endl;
    }

  }
  catch ( ... ) {
    cout << "problems" << endl;
  }
}

herr_t IterateCallback( hid_t group, const char *name, void *op_data );

class CFilterSelectionIteratorControl {
public:
  CFilterSelectionIteratorControl( 
    CScripts::enumDayStart dstype, int count, ptime dtStart, ptime dtEnd,
    const string &sBaseGroup,
    CSymbolSelectionFilter *pFilter ) :
      m_dstype( dstype ), m_count( count ), m_dtStart( dtStart ), m_dtEnd( dtEnd ), 
        m_sBaseGroup( sBaseGroup ), m_pFilter( pFilter ) { };
  void Process( const string &sObjectName ) {
    CDataManager dm;
    H5G_stat_t stats;
    string sObjectPath;
    try {
      sObjectPath = m_sBaseGroup + sObjectName;
      dm.GetH5File()->getObjinfo( sObjectPath, stats );
      switch ( stats.type ) {
        case H5G_DATASET: {
          CHDF5TimeSeriesContainer<CBar> barRepository( sObjectPath );
          CHDF5TimeSeriesContainer<CBar>::iterator begin, end;
          end = lower_bound( barRepository.begin(), barRepository.end(), m_dtEnd );
          switch ( m_dstype ) {
            case CScripts::DaySelect:
              begin = lower_bound( barRepository.begin(), end, m_dtStart );
              break;
            case CScripts::BarCount:
              break;
              begin = end;
              begin -= m_count;
            case CScripts::DayCount:
              date_duration tmp( m_count );
              ptime dt = (*end).m_dt - tmp;
              begin = lower_bound( barRepository.begin(), end, dt );
          }
          hsize_t cnt = end - begin;
          //CBars bars( cnt );
          void *p = m_pFilter->Bars()->First();
          int p2 = sizeof( CBar );
          m_pFilter->Bars()->Resize( cnt );
          barRepository.Read( begin, end, m_pFilter->Bars() );
          m_pFilter->Process( sObjectName );
          break;
        }
        break;
        case H5G_GROUP:
          int idx = 0;  // starting location for interrupted queries
          sObjectPath.append( "/" );
          CFilterSelectionIteratorControl control( m_dstype, m_count, m_dtStart, m_dtEnd, sObjectPath, m_pFilter );
          int result = dm.GetH5File()->iterateElems( sObjectPath, &idx, &IterateCallback, &control );  
          break;
      }
    }
    catch ( H5::Exception e ) {
      cout << "CHDF5TimeSeriesAccessor<T>::Retrieve H5::Exception " << e.getDetailMsg() << endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CDataManager::PrintH5ErrorStackItem, 0 );
    }
  }
protected:
  CScripts::enumDayStart m_dstype;
  int m_count;
  ptime m_dtStart;
  ptime m_dtEnd;
  string m_sBaseGroup;
  CSymbolSelectionFilter *m_pFilter;
private:
};

herr_t IterateCallback( hid_t group, const char *name, void *op_data ) {
  CFilterSelectionIteratorControl *pControl = 
    ( CFilterSelectionIteratorControl * ) op_data;
  pControl->Process( name );
  return 0;
}

void CScripts::Scan( enumScanType scantype, enumDayStart dstype, int count, ptime dtStart, ptime dtEnd ) {

  CSymbolSelectionFilter *pFilter;
  switch ( scantype ) {
    case Darvas:
      pFilter = new CSelectSymbolWithDarvas();
      break;
    case Bollinger:
      pFilter = new CSelectSymbolWithBollinger();
      break;
    case Breakout:
      pFilter = new CSelectSymbolWithBreakout();
      break;
  }

  CDataManager dm;
  int idx = 0;  // starting location for interrupted queries
  string sBaseGroup = "/bar/86400/";
  CFilterSelectionIteratorControl control( dstype, count, dtStart, dtEnd, sBaseGroup, pFilter );
  int result = dm.GetH5File()->iterateElems( sBaseGroup, &idx, &IterateCallback, &control );
  cout << "iteration returned " << result << endl;

  delete pFilter;
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


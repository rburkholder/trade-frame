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
      cout << szSymbol << " ";
      if ( !symbolfile.GetBitMutual() && !symbolfile.GetBitMoneyMkt() ) {
        cout << "useful";
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
        cout << "n/a";
      }
      cout << endl;
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
    ptime dt( boost::gregorian::date( 2008, 04, 04 ), boost::posix_time::time_duration( 1, 1, 1 ) );
    //ptime dt( boost::gregorian::date( 2008, 04, 04 ) );
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

// need to check queue as didn't get the queue has finished message

/*
active = 1
H5::FileIException H5Gopen2 failed
HD XTC.X done 3342
active = 2
H5::FileIException H5Gopen2 failed
HD YIH.X done 2037
active = 3
H5 Error Level 12: k:\data\projects\hdf5-1.8.0-beta5\src\h5gnode.c::H5G_node_found::not found
H5 Error Level 11: k:\data\projects\hdf5-1.8.0-beta5\src\h5gstab.c::H5G_stab_lookup::not found
H5 Error Level 10: k:\data\projects\hdf5-1.8.0-beta5\src\h5gobj.c::H5G_obj_lookup::can't locate object
H5 Error Level 9: k:\data\projects\hdf5-1.8.0-beta5\src\h5dint.c::H5D_create::dataspace extent has not been set.
H5 Error Level 8: k:\data\projects\hdf5-1.8.0-beta5\src\h5doh.c::H5O_dset_create::unable to create dataset
H5 Error Level 7: k:\data\projects\hdf5-1.8.0-beta5\src\h5o.c::H5O_obj_create::unable to open object
H5 Error Level 6: k:\data\projects\hdf5-1.8.0-beta5\src\h5l.c::H5L_link_cb::unable to create object
H5 Error Level 5: k:\data\projects\hdf5-1.8.0-beta5\src\h5gtraverse.c::H5G_traverse_real::traversal operator failed
H5 Error Level 4: k:\data\projects\hdf5-1.8.0-beta5\src\h5gtraverse.c::H5G_traverse::internal path traversal failed
H5 Error Level 3: k:\data\projects\hdf5-1.8.0-beta5\src\h5l.c::H5L_create_real::can't insert link
H5 Error Level 2: k:\data\projects\hdf5-1.8.0-beta5\src\h5l.c::H5L_link_object::unable to create new link to object
H5 Error Level 1: k:\data\projects\hdf5-1.8.0-beta5\src\h5dint.c::H5D_create_named::unable to create and link to dataset
H5 Error Level 0: k:\data\projects\hdf5-1.8.0-beta5\src\h5d.c::H5Dcreate2::unable to create dataset
H5::FileIException H5Dcreate2 failed
series is empty
HD ZWI.X done 0
HD ZWI.X !ERROR! Invalid symbol.
active = 4

*/
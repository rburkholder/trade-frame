#include "StdAfx.h"
#include "Scripts.h"

#include "DataManager.h"
using namespace H5;
#include "IQFeedSymbolFile.h"

#include "HDF5TimeSeriesContainer.h"

#include <stdexcept>
#include<algorithm>
#include<functional>
#include<string>
using namespace std;

//
// HistoryCollector
// 

CHistoryCollector::CHistoryCollector( const char *szSymbol, unsigned long nCount ) {
  m_sSymbol.assign( szSymbol );
  m_nCount = nCount;
}

CHistoryCollector::~CHistoryCollector() {
}

void CHistoryCollector::Start( void ) {
  // m_phistory will be assigned by sub classes
  ASSERT( NULL != m_phistory );
  m_phistory->LiveRequest( m_sSymbol.c_str(), m_nCount );
}

void CHistoryCollector::OnCompletion( IQFeedHistory *pHistory ) {
  if ( NULL != OnRetrievalComplete ) OnRetrievalComplete( this );
}

//
// CHistoryCollectorDaily
//

CHistoryCollectorDaily::CHistoryCollectorDaily( const char *szSymbol, unsigned long nCount ) :
  CHistoryCollector( szSymbol, nCount ) {
  m_phistory = new IQFeedHistoryHD( &m_bars );
  FinalizeCreation();
}

CHistoryCollectorDaily::~CHistoryCollectorDaily( void ) {
  delete m_phistory;
}

void CHistoryCollectorDaily::Start( void ) {
  m_bars.Clear();
  CHistoryCollector::Start();
}

void CHistoryCollectorDaily::WriteData( void ) {
  try {
    string sFileName1 = "/bar/86400/" + m_sSymbol;

    CDataManager dm;
    CompType *pdt = CBar::DefineDataType();
    DataSpace *pds = m_bars.DefineDataSpace(); 
    DataSet *dataset;
    DSetCreatPropList pl;
    hsize_t sizeChunk = 64;
    pl.setChunk( 1, &sizeChunk );
    bool bNeedToCreateDataSet = false;
    try { // check if dataset exists (for overwrite)
      dataset = new DataSet( dm.GetH5File()->openDataSet( sFileName1 ) );
    }
    catch ( H5::FileIException e ) {
      cout << "H5::FileIException " << e.getDetailMsg() << endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CDataManager::PrintH5ErrorStackItem, this );
      bNeedToCreateDataSet = true;
    }
    catch (...) {
      cout << "unknown error" << endl;
    }
    if ( bNeedToCreateDataSet ) {
      dataset = new DataSet( dm.GetH5File()->createDataSet( sFileName1, *pdt, *pds, pl ) );
    }
    //Dataset ds2 = dm.GetH5File()->openDataSet( m_sSymbol.c_str() );
    dataset->write( m_bars.First(), *pdt );
    dataset->close();
    pds->close();
    pdt->close();
    dm.AddGroupForSymbol( m_sSymbol );
    dm.GetH5File()->link( H5L_type_t::H5L_TYPE_HARD, sFileName1, "/symbol/" + m_sSymbol + "/bar.86400" );
    delete dataset;
    delete pds;
    delete pdt;
  }
  catch ( H5::DataSetIException e ) {
    cout << "H5::DataSetIException " << e.getDetailMsg() << endl;
  }
  catch ( H5::DataSpaceIException e ) {
    cout << "H5::DataSpaceIException " << e.getDetailMsg() << endl;
  }
  catch ( H5::DataTypeIException e ) {
    cout << "H5::DataTypeIException " << e.getDetailMsg() << endl;
  }
  catch ( H5::FileIException e ) {
    cout << "H5::FileIException " << e.getDetailMsg() << endl;
    e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CDataManager::PrintH5ErrorStackItem, this );
  }
  catch ( H5::GroupIException e ) {
    cout << "H5::GroupIException " << e.getDetailMsg() << endl;
    e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CDataManager::PrintH5ErrorStackItem, this );
  }
  catch (...) {
    cout << "unknown error" << endl;
  }
}

//
// CHistoryCollectorTicks
//

CHistoryCollectorTicks::CHistoryCollectorTicks( const char *szSymbol, unsigned long nCount ) :
  CHistoryCollector( szSymbol, nCount ) {
  m_phistory = new IQFeedHistoryHT( &m_quotes, &m_trades );
  FinalizeCreation();
}

CHistoryCollectorTicks::~CHistoryCollectorTicks( void ) {
  delete m_phistory;
}

void CHistoryCollectorTicks::Start( void ) {
  m_trades.Clear();
  m_quotes.Clear();
  CHistoryCollector::Start();
}

void CHistoryCollectorTicks::WriteData( void ) {
}

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


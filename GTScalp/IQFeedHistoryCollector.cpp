#include "StdAfx.h"
#include "IQFeedHistoryCollector.h"

#include "DataManager.h"
using namespace H5;


//
// CHistoryCollector
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


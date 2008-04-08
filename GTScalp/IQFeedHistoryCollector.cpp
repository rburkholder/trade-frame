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
  if ( 0 != m_bars.Count() ) {
    try {
      string sFileName1 = "/bar/86400/" + m_sSymbol;

      CDataManager dm;
      CompType *pdt = CBar::DefineDataType();
      DataSpace *pds = m_bars.DefineDataSpace(); 
      DataSet *dataset;
      DSetCreatPropList pl;
      hsize_t sizeChunk = CDataManager::H5ChunkSize();
      pl.setChunk( 1, &sizeChunk );
      bool bNeedToCreateDataSet = false;
      try { // check if dataset exists (for overwrite)
        dataset = new DataSet( dm.GetH5File()->openDataSet( sFileName1 ) );
      }
      catch ( H5::FileIException e ) {
        bNeedToCreateDataSet = true;
      }
      catch (...) {
        cout << "CHistoryCollectorDaily::WriteData  unknown error 1" << endl;
      }
      if ( bNeedToCreateDataSet ) {
        try {
          dataset = new DataSet( dm.GetH5File()->createDataSet( sFileName1, *pdt, *pds, pl ) );
          dataset->write( m_bars.First(), *pdt );
          dataset->close();
          delete dataset;
          dm.AddGroupForSymbol( m_sSymbol );
          //dm.GetH5File()->link( H5L_type_t::H5L_TYPE_HARD, sFileName1, "/symbol/" + m_sSymbol + "/bar.86400" );
        }
        catch (  H5::FileIException e ) {
          cout << "H5::FileIException " << e.getDetailMsg() << endl;
          e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CDataManager::PrintH5ErrorStackItem, this );
        }
        catch ( ... ) {
          cout << "CHistoryCollectorDaily::WriteData:  unknown error 2" << endl;
        }
      }
      else {
        cout << "Code is needed to write over existing dataset for " << m_sSymbol << endl;
      }
      pds->close();
      pdt->close();
      delete pds;
      delete pdt;
    }
    catch ( H5::Exception e ) {
      cout << "H5::Exception " << e.getDetailMsg() << endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CDataManager::PrintH5ErrorStackItem, this );
    }
    catch (...) {
      cout << "CHistoryCollectorDaily::WriteData:  unknown error 3" << endl;
    }
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


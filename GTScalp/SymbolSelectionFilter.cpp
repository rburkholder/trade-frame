#include "StdAfx.h"
#include "SymbolSelectionFilter.h"

#include "DataManager.h"
using namespace H5;

#include "TimeSeries.h"
#include "HDF5TimeSeriesContainer.h"

#include "Darvas.h"

// destroy self when done

//
// CSymbolSelectionFilter
//
CSymbolSelectionFilter::CSymbolSelectionFilter( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd ) :
  m_DayStartType( dstype) , m_nCount( count ), m_bUseStart( bUseStart ), m_dtStart( dtStart ), m_bUseLast( bUseEnd ), m_dtLast( dtEnd ) {
}

CSymbolSelectionFilter::~CSymbolSelectionFilter(void) {
}

// forward reference only
herr_t IterateCallback( hid_t group, const char *name, void *op_data );

// called from IterateCallback (which is called as HDF5 iterates the directory
class CFilterSelectionIteratorControl {
public:
  CFilterSelectionIteratorControl( const string &sBaseGroup, CSymbolSelectionFilter *pFilter ) :
      m_sBaseGroup( sBaseGroup ), m_pFilter( pFilter ) { };
  void Process( const string &sObjectName ) {
    CDataManager dm;
    H5G_stat_t stats;
    string sObjectPath;
    sObjectPath = m_sBaseGroup + sObjectName;
    try {
      dm.GetH5File()->getObjinfo( sObjectPath, stats );
      switch ( stats.type ) {
        case H5G_DATASET: 
          m_pFilter->Process( sObjectName, sObjectPath );
          break;
        case H5G_GROUP:
          int idx = 0;  // starting location for interrupted queries
          sObjectPath.append( "/" );
          CFilterSelectionIteratorControl control( sObjectPath, m_pFilter );
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
  string m_sBaseGroup;
  CSymbolSelectionFilter *m_pFilter;
private:
};

void CSymbolSelectionFilter::Start( void ) {
  CDataManager dm;
  int idx = 0;  // starting location for interrupted queries
  string sBaseGroup = "/bar/86400/";
  CFilterSelectionIteratorControl control( sBaseGroup, this );
  int result = dm.GetH5File()->iterateElems( sBaseGroup, &idx, &IterateCallback, &control );
  cout << "iteration returned " << result << endl;
}

herr_t IterateCallback( hid_t group, const char *name, void *op_data ) {
  CFilterSelectionIteratorControl *pControl = 
    ( CFilterSelectionIteratorControl * ) op_data;
  pControl->Process( name );
  return 0;
}

//
// CSelectSymbolWithDarvas
//
CSelectSymbolWithDarvas::CSelectSymbolWithDarvas( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd) :
  CSymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) {
}

CSelectSymbolWithDarvas::~CSelectSymbolWithDarvas(void) {
}

bool CSelectSymbolWithDarvas::Validate( void ) {
  return ( m_bUseStart && m_bUseLast );
}

class CalcMaxDate: public std::unary_function<CBar &, void> {
public:
  CalcMaxDate( void ) : dtMax( boost::date_time::special_values::min_date_time ), dblMax( 0 ) { };
  void operator() ( CBar &bar ) {
    if ( bar.m_dblClose > dblMax ) {
      dblMax = bar.m_dblClose;
      dtMax = bar.m_dt;
    }
  }
  operator ptime() { return dtMax; };
protected:
  ptime dtMax;
  double dblMax;
private:
};

void CSelectSymbolWithDarvas::Process( const string &sSymbol, const string &sPath ) {
  //cout << "Darvas for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
  CHDF5TimeSeriesContainer<CBar> barRepository( sPath );
  CHDF5TimeSeriesContainer<CBar>::iterator begin, end;
  ptime dtStart = m_dtStart - date_duration(52 * 7); // get 52 weeks of values from day of max value
  begin = lower_bound( barRepository.begin(), barRepository.end(), dtStart );
  end = lower_bound( begin, barRepository.end(), m_dtLast );  // retrieve to end for the trigger
  hsize_t cnt = end - begin;
  //void *p = m_bars.First();  // used for memory test
  //int p2 = sizeof( CBar );   // used for memory test
  m_bars.Resize( cnt );
  if ( 200 < cnt ) {   // at least 200 bars needed
    barRepository.Read( begin, end, &m_bars );
    ptime dtDayOfMax = std::for_each( m_bars.begin(), m_bars.end(), CalcMaxDate() );
    ptime dtDayRequested = m_dtStart - m_dtStart.time_of_day();
    if ( dtDayOfMax == dtDayRequested ) {
      cout << "Darvas has max for " << sSymbol << endl;
      vector<CBar>::iterator iter = m_bars.iterAtOrAfter( dtDayRequested - date_duration( 20 ) ); // take 20 days to run trigger
      bool bTrigger = for_each( iter, m_bars.end(), CDarvas() );
      if ( bTrigger ) {
        cout << "Darvas triggered for " << sSymbol << endl;
      }
    }
  }
}

//
// CSelectSymbolWithBollinger
//
CSelectSymbolWithBollinger::CSelectSymbolWithBollinger( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd) :
  CSymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) {
}

CSelectSymbolWithBollinger::~CSelectSymbolWithBollinger(void) {
}

void CSelectSymbolWithBollinger::Process( const string &sSymbol, const string &sPath ) {
  cout << "Bollinger for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
  CHDF5TimeSeriesContainer<CBar> barRepository( sPath );
  CHDF5TimeSeriesContainer<CBar>::iterator begin, end;
  end = lower_bound( barRepository.begin(), barRepository.end(), m_dtStart );
  switch ( m_DayStartType ) {
    case DaySelect:
      begin = lower_bound( barRepository.begin(), end, m_dtStart );
      break;
    case BarCount:
      break;
      begin = end;
      begin -= m_nCount;
    case DayCount:
      date_duration tmp( m_nCount );
      ptime dt = (*end).m_dt - tmp;
      begin = lower_bound( barRepository.begin(), end, dt );
  }
  hsize_t cnt = end - begin;
  //CBars bars( cnt );
  void *p = m_bars.First();
  int p2 = sizeof( CBar );
  m_bars.Resize( cnt );
  barRepository.Read( begin, end, &m_bars );
}

//
// CSelectSymbolWithBreakout
//
CSelectSymbolWithBreakout::CSelectSymbolWithBreakout( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd) :
  CSymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) {
}

CSelectSymbolWithBreakout::~CSelectSymbolWithBreakout(void) {
}

void CSelectSymbolWithBreakout::Process( const string &sSymbol, const string &sPath ) {
  cout << "Breakout for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
}



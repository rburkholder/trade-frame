#include "StdAfx.h"
#include "SymbolSelectionFilter.h"

#include "HDF5DataManager.h"
using namespace H5;

#include "TimeSeries.h"
#include "HDF5TimeSeriesContainer.h"
#include "HDF5IterateGroups.h"

#include "RunningStats.h"

#include "Darvas.h"

#include <stdexcept>
#include <map>

// need to destroy self when done
// http://www.codeproject.com/KB/stl/xingstlarticle.aspx  functors

// implement boost 1_35_0 soon for the boost::bind

//
// CSymbolSelectionFilter ( base class for filters )
//
CSymbolSelectionFilter::CSymbolSelectionFilter( 
  enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd ) :
  m_DayStartType( dstype) , m_nCount( count ), 
    m_bUseStart( bUseStart ), m_dtStart( dtStart ), m_bUseLast( bUseEnd ), m_dtLast( dtEnd ) {
}

CSymbolSelectionFilter::~CSymbolSelectionFilter(void) {
  m_bars.Clear();
}

void CSymbolSelectionFilter::Start( void ) {
  string sBaseGroup = "/bar/86400/";
  HDF5IterateGroups<CSymbolSelectionFilter> control;
  int result = control.Start( sBaseGroup, this );
  cout << "iteration returned " << result << endl;
}

//
// CSelectSymbolWithDarvas
//
class CalcMaxDate: public std::unary_function<CBar &, void> {
public:
  CalcMaxDate( void ) : dtMax( boost::date_time::special_values::min_date_time ), dblMax( 0 ) { };
  void operator() ( CBar &bar ) {
    if ( bar.m_dblClose >= dblMax ) {
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

class CalcSixMonMeans {
private:
  CRunningStats rsUpper, rsLower;
  int cnt;
protected:
public:
  CalcSixMonMeans( void ): cnt( 0 ) {};
  ~CalcSixMonMeans( void ) {
    rsUpper.CalcStats();
    rsLower.CalcStats();
    cout << endl << "  upper mean=" << rsUpper.meanY << ", SD=" << rsUpper.SD;
    cout << endl << "  lower mean=" << rsLower.meanY << ", SD=" << rsLower.SD;
  }
  void operator() ( const CBar &bar ) {
    ++cnt;
    double t = cnt;
    rsUpper.Add( t, bar.m_dblHigh - bar.m_dblOpen );
    rsLower.Add( t, bar.m_dblOpen - bar.m_dblLow );
  }
};

class CalcAverageVolume {
private:
  unsigned long m_nTotalVolume;
  unsigned long m_nNumberOfValues;
protected:
public:
  CalcAverageVolume() : m_nTotalVolume( 0 ), m_nNumberOfValues( 0 ) {};
  void operator() ( const CBar &bar ) {
    m_nTotalVolume += bar.m_nVolume;
    ++m_nNumberOfValues;
  }
  operator unsigned long() { return m_nTotalVolume / m_nNumberOfValues; };
};

class CalcAveragePrice {
private:
  double m_dblSumOfPrices;
  unsigned long m_nNumberOfValues;
protected:
public:
  CalcAveragePrice() : m_dblSumOfPrices( 0 ), m_nNumberOfValues( 0 ) {};
  void operator() ( const CBar &bar ) {
    m_dblSumOfPrices += bar.m_dblClose;
    ++m_nNumberOfValues;
  }
  operator double() { return m_dblSumOfPrices / m_nNumberOfValues; };
};

CSelectSymbolWithDarvas::CSelectSymbolWithDarvas( 
  enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd ) :
  CSymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) {
}

CSelectSymbolWithDarvas::~CSelectSymbolWithDarvas(void) {
}

bool CSelectSymbolWithDarvas::Validate( void ) {
  return ( CSymbolSelectionFilter::Validate() && m_bUseStart && m_bUseLast );
}

void CSelectSymbolWithDarvas::Process( const string &sSymbol, const string &sPath ) {
  //cout << "Darvas for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
  CHDF5TimeSeriesContainer<CBar> barRepository( sPath );
  CHDF5TimeSeriesContainer<CBar>::iterator begin, end;
  ptime dtPt1 = m_dtStart - m_dtStart.time_of_day();  // normalize day of max
  ptime dtPt2 = m_dtLast - m_dtLast.time_of_day();  // normalize end day ( 1 past day of last bar )
  ptime dtOneYrAgo = dtPt2 - date_duration(52 * 7); // get 52 weeks of values from day of max value
  ptime dt26WksAgo = dtPt2 - date_duration(26 * 7);
  ptime dtTrigger = dtPt2 - date_duration( 1 );
  begin = lower_bound( barRepository.begin(), barRepository.end(), dtOneYrAgo );
  end = lower_bound( begin, barRepository.end(), dtPt2 );  // retrieve to one day past trigger
  hsize_t cnt = end - begin;
  if ( 20 <= cnt ) {
    ptime dttmp = (*(end-1)).m_dt;
    if ( 
      ( 240 < cnt ) 
      && ( dttmp == dtTrigger ) ){   // at least 240 bars, with required last bar
        m_bars.Resize( cnt );
        barRepository.Read( begin, end, &m_bars );

        vector<CBar>::iterator volIter = m_bars.end() - 20;
        unsigned long nAverageVolume = std::for_each( volIter, m_bars.end(), CalcAverageVolume() );

        if ( 
          ( 1000000 < nAverageVolume )
          && ( m_bars.Last()->m_dblClose >= 20.0 ) 
          && ( m_bars.Last()->m_dblClose  < 80.0 ) 
          ) {  // need certain amount of liquidity before entering trade (20 bars worth)

            ptime dtDayOfMax = std::for_each( m_bars.begin(), m_bars.end(), CalcMaxDate() );
            if ( dtDayOfMax >= dtPt1 ) {
              cout << 
                "Darvas max for " << sSymbol << 
                " on " << dtDayOfMax << 
                ", close=" << m_bars.Last()->m_dblClose <<
                ", volume=" << m_bars.Last()->m_nVolume;
              vector<CBar>::iterator iter = m_bars.iterAtOrAfter( dtPt2 - date_duration( 20 ) ); // take 20 days to run trigger
              CDarvasResults results = for_each( iter, m_bars.end(), CDarvas() );
              if ( results.GetTrigger() ) {
                cout << " triggered, stop=" << results.GetStopLevel();
                iter = m_bars.iterAtOrAfter( dt26WksAgo );
                for_each( iter, m_bars.end(), CalcSixMonMeans() );
                if ( NULL != OnAddSymbol ) OnAddSymbol( sSymbol, sPath, "Darvas" );
              }
              cout << endl;
            }
        }
    }
  }
}

//
// CSelectSymbolWith10Percent
//
CSelectSymbolWith10Percent::CSelectSymbolWith10Percent( 
  enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd ) :
  CSymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) {
}

CSelectSymbolWith10Percent::~CSelectSymbolWith10Percent(void) {
}

bool CSelectSymbolWith10Percent::Validate( void ) {
  return ( CSymbolSelectionFilter::Validate() && m_bUseLast );
}

void CSelectSymbolWith10Percent::Process( const string &sSymbol, const string &sPath ) {
  //cout << "10 Percent for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
  m_sPath = sPath;
  CHDF5TimeSeriesContainer<CBar> barRepository( sPath );
  CHDF5TimeSeriesContainer<CBar>::iterator begin, end;
  ptime dtPt2 = m_dtLast - m_dtLast.time_of_day();  // normalize end day ( 1 past day of last bar )
  end = lower_bound( barRepository.begin(), barRepository.end(), dtPt2 );
  //if (  ( end != barRepository.end() ) ) {
    if ( 20 < ( end - barRepository.begin() ) ) {  // make sure there are at least 20 bars available
      begin = end - 20;
      m_bars.Resize( 20 );
      barRepository.Read( begin, end, &m_bars );
      if ( ( m_bars.Last()->m_dblClose < 80.0 ) 
        && ( m_bars.Last()->m_dblClose > 20.0 ) 
        && ( m_bars.Last()->m_dt == ( dtPt2 - date_duration( 1 ) ) ) ) {
        unsigned long nAverageVolume = std::for_each( m_bars.begin(), m_bars.end(), CalcAverageVolume() );
        double dblAveragePrice = std::for_each( m_bars.begin(), m_bars.end(), CalcAveragePrice() );
        if ( ( 1000000 < nAverageVolume ) && ( 25 < dblAveragePrice ) ) {  // need certain amount of liquidity before entering trade (20 bars worth)
          multimap<double, string>::iterator iterPos;
          std::multimap<double, string, MaxNegativesCompare>::iterator iterNeg;
          double dblReturn = ( m_bars.Last()->m_dblClose - m_bars.Last()->m_dblOpen ) / m_bars.Last()->m_dblClose;
          if ( nMaxInList > mapMaxPositives.size() ) {
            mapMaxPositives.insert( pair<double, string>( dblReturn, sPath ) );
          }
          else {
            iterPos = mapMaxPositives.begin();
            if ( dblReturn > iterPos->first ) {
              mapMaxPositives.erase( iterPos );
              mapMaxPositives.insert( pair<double, string>( dblReturn, sPath ) );
            }
          }
          if ( nMaxInList > mapMaxNegatives.size() ) {
            mapMaxNegatives.insert( pair<double, string>( dblReturn, sPath ) );
          }
          else {
            iterNeg = mapMaxNegatives.begin();
            if ( dblReturn < iterNeg->first ) {
              mapMaxNegatives.erase( iterNeg );
              mapMaxNegatives.insert( pair<double, string>( dblReturn, sPath ) );
            }
          }
        }
      }
    }
  //}
}

void CSelectSymbolWith10Percent::WrapUp( void ) {
  cout << "Positives: " << endl;
  multimap<double, string>::iterator iterPos;
  for ( iterPos = mapMaxPositives.begin(); iterPos != mapMaxPositives.end(); ++iterPos ) {
    size_t pos = (iterPos->second).find_last_of( "/" );
    std::string sSymbolName = (iterPos->second).substr( pos + 1 );;
    cout << " " << sSymbolName << "=" << iterPos->first << endl;
    if ( NULL != OnAddSymbol ) OnAddSymbol( sSymbolName, iterPos->second, "10%+" );
  }
  mapMaxPositives.clear();
  cout << "Negatives: " << endl;
  std::multimap<double, string, MaxNegativesCompare>::iterator iterNeg;
  for ( iterNeg = mapMaxNegatives.begin(); iterNeg != mapMaxNegatives.end(); ++iterNeg ) {
    size_t pos = (iterNeg->second).find_last_of( "/" );
    std::string sSymbolName = (iterNeg->second).substr( pos + 1 );;
    cout << " " << sSymbolName << "=" << iterNeg->first << endl;
    if ( NULL != OnAddSymbol ) OnAddSymbol( sSymbolName, iterNeg->second, "10%-" );
  }
  mapMaxNegatives.clear();
}

//
// CSelectSymbolWithVolatility
//

CSelectSymbolWithVolatility::CSelectSymbolWithVolatility( 
  enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd ) :
  CSymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) {
}

CSelectSymbolWithVolatility::~CSelectSymbolWithVolatility(void) {
}

bool CSelectSymbolWithVolatility::Validate( void ) {
  return ( CSymbolSelectionFilter::Validate() && m_bUseLast );
}

class CalcAverageVolatility {
private:
  double m_dblVolatility;
  unsigned long m_nNumberOfValues;
protected:
public:
  CalcAverageVolatility() : m_dblVolatility( 0 ), m_nNumberOfValues( 0 ) {};
  void operator() ( const CBar &bar ) {
    //m_dblVolatility += ( bar.m_dblHigh - bar.m_dblLow ) / bar.m_dblClose;
    m_dblVolatility += ( bar.m_dblHigh - bar.m_dblLow ) ;
    ++m_nNumberOfValues;
  }
  operator double() { return m_dblVolatility / m_nNumberOfValues; };
};

void CSelectSymbolWithVolatility::Process( const string &sSymbol, const string &sPath ) {
  try {
  m_sPath = sPath;
  //cout << "Volatility for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
  CHDF5TimeSeriesContainer<CBar> barRepository( sPath );
  CHDF5TimeSeriesContainer<CBar>::iterator begin, end;
  ptime dtPt2 = m_dtLast - m_dtLast.time_of_day();  // normalize end day ( 1 past day of last bar )
  end = lower_bound( barRepository.begin(), barRepository.end(), dtPt2 );
  if ( 20 < ( end - barRepository.begin() ) ) {  // make sure there are at least 20 bars available
    begin = end - 20;
    m_bars.Resize( 20 );
    barRepository.Read( begin, end, &m_bars );
    if ( ( m_bars.Last()->m_dblClose < 50.0 ) 
      && ( m_bars.Last()->m_dblClose > 20.0 ) 
      && ( m_bars.Last()->m_dt == ( dtPt2 - date_duration( 1 ) ) ) ) {
      unsigned long nAverageVolume = std::for_each( m_bars.begin(), m_bars.end(), CalcAverageVolume() );
      double dblAveragePrice = std::for_each( m_bars.begin(), m_bars.end(), CalcAveragePrice() );
      if ( ( 1000000 < nAverageVolume ) && ( 25.0 < dblAveragePrice ) ) {  // need certain amount of liquidity before entering trade (20 bars worth)
        double dblAverageVolatility = std::for_each( m_bars.begin(), m_bars.end(), CalcAverageVolatility() );
        //double dblVolatility = ( m_bars.Last()->m_dblHigh - m_bars.Last()->m_dblLow ) / m_bars.Last()->m_dblClose;
        multimap<double, string>::iterator iterPos;
        if ( nMaxInList > mapMaxVolatility.size() ) {
          mapMaxVolatility.insert( pair<double, string>( dblAverageVolatility, sPath ) );
        }
        else {
          iterPos = mapMaxVolatility.begin();
          if ( dblAverageVolatility > iterPos->first ) {
            mapMaxVolatility.erase( iterPos );
            mapMaxVolatility.insert( pair<double, string>( dblAverageVolatility, sPath ) );
          }
        }
      }
    }
  }
  }
  catch (...) {
    std::cout << "problems somewhere" << std::endl;
  }
}

void CSelectSymbolWithVolatility::WrapUp( void ) {
  cout << "Volatiles: " << endl;
  multimap<double, string>::iterator iterPos;
  for ( iterPos = mapMaxVolatility.begin(); iterPos != mapMaxVolatility.end(); ++iterPos ) {
    size_t pos = (iterPos->second).find_last_of( "/" );
    std::string sSymbolName = (iterPos->second).substr( pos + 1 );;
    cout << " " << sSymbolName << "=" << iterPos->first << endl;
    if ( NULL != OnAddSymbol ) OnAddSymbol( sSymbolName, iterPos->second, "Volatile" );
  }
  mapMaxVolatility.clear();
}

//
// CSelectSymbolWithBreakout
//

CSelectSymbolWithBreakout::CSelectSymbolWithBreakout( 
  enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd) :
  CSymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) {
}

CSelectSymbolWithBreakout::~CSelectSymbolWithBreakout(void) {
}

void CSelectSymbolWithBreakout::Process( const string &sSymbol, const string &sPath ) {
  cout << "Breakout for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
}


//
// CSelectSymbolWithXWeekHigh
//
CSelectSymbolWithXWeekHigh::CSelectSymbolWithXWeekHigh( 
  enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd) :
  CSymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) {
}

CSelectSymbolWithXWeekHigh::~CSelectSymbolWithXWeekHigh(void) {
}

void CSelectSymbolWithXWeekHigh::Process( const string &sSymbol, const string &sPath ) {
  //cout << "CSelectSymbolWithXWeekHigh for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
  CHDF5TimeSeriesContainer<CBar> barRepository( sPath );
  CHDF5TimeSeriesContainer<CBar>::iterator begin, end;
  ptime dtStart = m_dtStart - date_duration(52 * 7); // get 52 weeks of values from day of max value
  begin = lower_bound( barRepository.begin(), barRepository.end(), dtStart );
  //end = lower_bound( begin, barRepository.end(), m_dtLast );  // retrieve to end for the trigger
  end = barRepository.end();
  hsize_t cnt = end - begin;
  if ( 240 < cnt ) {   // at least 200 bars needed
    m_bars.Resize( cnt );
    barRepository.Read( begin, end, &m_bars );
    ptime dtDayOfMax = std::for_each( m_bars.begin(), m_bars.end(), CalcMaxDate() );
    ptime dtDayRequested = m_dtStart - m_dtStart.time_of_day();  // normalize to midnight
    if ( dtDayOfMax == dtDayRequested ) {
      cout << "XDayMax has max for " << sSymbol << " on " << dtDayOfMax << endl;
      vector<CBar>::iterator iter = m_bars.iterAtOrAfter( dtDayRequested - date_duration( 20 ) ); // take 20 days to run trigger
    }
  }
}


//
// CSelectSymbolWithBollinger
//

CSelectSymbolWithBollinger::CSelectSymbolWithBollinger( 
  enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd) :
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


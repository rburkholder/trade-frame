#include "StdAfx.h"

#include <stdexcept>
#include <map>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
using namespace H5;

#include <TFIndicators/RunningStats.h>
#include <TFIndicators/Darvas.h>

#include "SymbolSelectionFilter.h"

using namespace ou::tf;

// need to destroy self when done
// http://www.codeproject.com/KB/stl/xingstlarticle.aspx  functors

// implement boost 1_35_0 soon for the boost::bind

//
// SymbolSelectionFilter ( base class for filters )
//
SymbolSelectionFilter::SymbolSelectionFilter( 
  enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd ) :
  m_DayStartType( dstype) , m_nCount( count ), 
    m_bUseStart( bUseStart ), m_dtStart( dtStart ), m_bUseLast( bUseEnd ), m_dtLast( dtEnd ) {
}

SymbolSelectionFilter::~SymbolSelectionFilter(void) {
  m_bars.Clear();
}

void SymbolSelectionFilter::Start( void ) {
  string sBaseGroup = "/bar/86400/";
  HDF5IterateGroups<SymbolSelectionFilter> control;
  int result = control.Start( sBaseGroup, this );
  cout << "iteration returned " << result << endl;
}

//
// SelectSymbolWithDarvas
//
class CalcMaxDate: public std::unary_function<Bar &, void> {
public:
  CalcMaxDate( void ) : dtMax( boost::date_time::special_values::min_date_time ), dblMax( 0 ) { };
  void operator() ( Bar &bar ) {
    if ( bar.Close() >= dblMax ) {
      dblMax = bar.Close();
      dtMax = bar.DateTime();
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
  RunningStats rsUpper, rsLower;
  int cnt;
protected:
public:
  CalcSixMonMeans( void ): cnt( 0 ) {};
  ~CalcSixMonMeans( void ) {
    rsUpper.CalcStats();
    rsLower.CalcStats();
    cout << endl << "  upper mean=" << rsUpper.MeanY() << ", SD=" << rsUpper.SD();
    cout << endl << "  lower mean=" << rsLower.MeanY() << ", SD=" << rsLower.SD();
  }
  void operator() ( const Bar &bar ) {
    ++cnt;
    double t = cnt;
    rsUpper.Add( t, bar.High() - bar.Open() );
    rsLower.Add( t, bar.Open() - bar.Low() );
  }
};

class CalcAverageVolume {
private:
  unsigned long m_nTotalVolume;
  unsigned long m_nNumberOfValues;
protected:
public:
  CalcAverageVolume() : m_nTotalVolume( 0 ), m_nNumberOfValues( 0 ) {};
  void operator() ( const Bar &bar ) {
    m_nTotalVolume += bar.Volume();
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
  void operator() ( const Bar &bar ) {
    m_dblSumOfPrices += bar.Close();
    ++m_nNumberOfValues;
  }
  operator double() { return m_dblSumOfPrices / m_nNumberOfValues; };
};

SelectSymbolWithDarvas::SelectSymbolWithDarvas( 
  enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd ) :
  SymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) {
}

SelectSymbolWithDarvas::~SelectSymbolWithDarvas(void) {
}

bool SelectSymbolWithDarvas::Validate( void ) {
  return ( SymbolSelectionFilter::Validate() && m_bUseStart && m_bUseLast );
}

void SelectSymbolWithDarvas::Process( const string &sSymbol, const string &sPath ) {
  //cout << "Darvas for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
  HDF5TimeSeriesContainer<Bar> barRepository( sPath );
  HDF5TimeSeriesContainer<Bar>::iterator begin, end;
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

        vector<Bar>::iterator volIter = m_bars.end() - 20;
        unsigned long nAverageVolume = std::for_each( volIter, m_bars.end(), CalcAverageVolume() );

        if ( 
          ( 1000000 < nAverageVolume )
          && ( m_bars.Last()->Close() >= 20.0 ) 
          && ( m_bars.Last()->Close()  < 80.0 ) 
          ) {  // need certain amount of liquidity before entering trade (20 bars worth)

            ptime dtDayOfMax = std::for_each( m_bars.begin(), m_bars.end(), CalcMaxDate() );
            if ( dtDayOfMax >= dtPt1 ) {
              cout << 
                "Darvas max for " << sSymbol << 
                " on " << dtDayOfMax << 
                ", close=" << m_bars.Last()->Close() <<
                ", volume=" << m_bars.Last()->m_nVolume;
              vector<Bar>::iterator iter = m_bars.iterAtOrAfter( dtPt2 - date_duration( 20 ) ); // take 20 days to run trigger
              DarvasResults results = for_each( iter, m_bars.end(), CDarvas() );
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
// SelectSymbolWith10Percent
//
SelectSymbolWith10Percent::SelectSymbolWith10Percent( 
  enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd ) :
  SymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) {
}

SelectSymbolWith10Percent::~SelectSymbolWith10Percent(void) {
}

bool SelectSymbolWith10Percent::Validate( void ) {
  return ( SymbolSelectionFilter::Validate() && m_bUseLast );
}

void SelectSymbolWith10Percent::Process( const string &sSymbol, const string &sPath ) {
  //cout << "10 Percent for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
  m_sPath = sPath;
  HDF5TimeSeriesContainer<Bar> barRepository( sPath );
  HDF5TimeSeriesContainer<Bar>::iterator begin, end;
  ptime dtPt2 = m_dtLast - m_dtLast.time_of_day();  // normalize end day ( 1 past day of last bar )
  end = lower_bound( barRepository.begin(), barRepository.end(), dtPt2 );
  //if (  ( end != barRepository.end() ) ) {
    if ( 20 < ( end - barRepository.begin() ) ) {  // make sure there are at least 20 bars available
      begin = end - 20;
      m_bars.Resize( 20 );
      barRepository.Read( begin, end, &m_bars );
      if ( ( m_bars.Last()->Close() < 80.0 ) 
        && ( m_bars.Last()->Close() > 20.0 ) 
        && ( m_bars.Last()->DateTime() == ( dtPt2 - date_duration( 1 ) ) ) ) {
        unsigned long nAverageVolume = std::for_each( m_bars.begin(), m_bars.end(), CalcAverageVolume() );
        double dblAveragePrice = std::for_each( m_bars.begin(), m_bars.end(), CalcAveragePrice() );
        if ( ( 1000000 < nAverageVolume ) && ( 25 < dblAveragePrice ) ) {  // need certain amount of liquidity before entering trade (20 bars worth)
          multimap<double, string>::iterator iterPos;
          std::multimap<double, string, MaxNegativesCompare>::iterator iterNeg;
          double dblReturn = ( m_bars.Last()->Close() - m_bars.Last()->Open() ) / m_bars.Last()->Close();
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

void SelectSymbolWith10Percent::WrapUp( void ) {
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
// SelectSymbolWithVolatility
//

SelectSymbolWithVolatility::SelectSymbolWithVolatility( 
  enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd ) :
  SymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) {
}

SelectSymbolWithVolatility::~SelectSymbolWithVolatility(void) {
}

bool SelectSymbolWithVolatility::Validate( void ) {
  return ( SymbolSelectionFilter::Validate() && m_bUseLast );
}

class CalcAverageVolatility {
private:
  double m_dblVolatility;
  unsigned long m_nNumberOfValues;
protected:
public:
  CalcAverageVolatility() : m_dblVolatility( 0 ), m_nNumberOfValues( 0 ) {};
  void operator() ( const Bar &bar ) {
    //m_dblVolatility += ( bar.m_dblHigh - bar.m_dblLow ) / bar.m_dblClose;
    m_dblVolatility += ( bar.High() - bar.Low() ) ;
    ++m_nNumberOfValues;
  }
  operator double() { return m_dblVolatility / m_nNumberOfValues; };
};

void SelectSymbolWithVolatility::Process( const string &sSymbol, const string &sPath ) {
  try {
  m_sPath = sPath;
  //cout << "Volatility for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
  HDF5TimeSeriesContainer<Bar> barRepository( sPath );
  HDF5TimeSeriesContainer<Bar>::iterator begin, end;
  ptime dtPt2 = m_dtLast - m_dtLast.time_of_day();  // normalize end day ( 1 past day of last bar )
  end = lower_bound( barRepository.begin(), barRepository.end(), dtPt2 );
  if ( 20 < ( end - barRepository.begin() ) ) {  // make sure there are at least 20 bars available
    begin = end - 20;
    m_bars.Resize( 20 );
    barRepository.Read( begin, end, &m_bars );
    if ( ( m_bars.Last()->Close() < 50.0 ) 
      && ( m_bars.Last()->Close() > 20.0 ) 
      && ( m_bars.Last()->DateTime() == ( dtPt2 - date_duration( 1 ) ) ) ) {
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

void SelectSymbolWithVolatility::WrapUp( void ) {
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
// SelectSymbolWithBreakout
//

SelectSymbolWithBreakout::SelectSymbolWithBreakout( 
  enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd) :
  SymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) {
}

SelectSymbolWithBreakout::~SelectSymbolWithBreakout(void) {
}

void SelectSymbolWithBreakout::Process( const string &sSymbol, const string &sPath ) {
  cout << "Breakout for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
}


//
// CSelectSymbolWithXWeekHigh
//
CSelectSymbolWithXWeekHigh::CSelectSymbolWithXWeekHigh( 
  enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd) :
  SymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) {
}

CSelectSymbolWithXWeekHigh::~CSelectSymbolWithXWeekHigh(void) {
}

void CSelectSymbolWithXWeekHigh::Process( const string &sSymbol, const string &sPath ) {
  //cout << "CSelectSymbolWithXWeekHigh for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
  HDF5TimeSeriesContainer<Bar> barRepository( sPath );
  HDF5TimeSeriesContainer<Bar>::iterator begin, end;
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
      vector<Bar>::iterator iter = m_bars.iterAtOrAfter( dtDayRequested - date_duration( 20 ) ); // take 20 days to run trigger
    }
  }
}


//
// SelectSymbolWithBollinger
//

SelectSymbolWithBollinger::SelectSymbolWithBollinger( 
  enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd) :
  SymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) {
}

SelectSymbolWithBollinger::~SelectSymbolWithBollinger(void) {
}

void SelectSymbolWithBollinger::Process( const string &sSymbol, const string &sPath ) {
  cout << "Bollinger for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
  HDF5TimeSeriesContainer<Bar> barRepository( sPath );
  HDF5TimeSeriesContainer<Bar>::iterator begin, end;
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
  int p2 = sizeof( Bar );
  m_bars.Resize( cnt );
  barRepository.Read( begin, end, &m_bars );
}


//
// SelectSymbolWithStrikeCoverage
//

SelectSymbolWithStrikeCoverage::SelectSymbolWithStrikeCoverage(
  SymbolSelectionFilter::enumDayCalc dstype, int count, 
  bool bUseStart, boost::posix_time::ptime dtStart, bool bUseEnd, boost::posix_time::ptime dtEnd) 
  : SymbolSelectionFilter( dstype, count, bUseStart, dtStart, bUseEnd, dtEnd ) 
{
}

SelectSymbolWithStrikeCoverage::~SelectSymbolWithStrikeCoverage() {
}

void SelectSymbolWithStrikeCoverage::Process(const std::string &sSymbol, const std::string &sPath) {
}
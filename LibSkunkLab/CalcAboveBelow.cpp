#include "StdAfx.h"
#include "CalcAboveBelow.h"

#include "TimeSeries.h"

#include "HDF5DataManager.h"
#include "HDF5TimeSeriesContainer.h"

class CalcRangeStats {
public:
  CalcRangeStats( void ) : 
    m_dblTotalOfOpen( 0 ), m_dblTotalOfClose( 0 ),
    m_nCount( 0 ),
    m_nCountAbove( 0 ), m_nCountBelow( 0 ),
    m_dblAbove( 0 ), m_dblBelow( 0 ),
    m_nCountUp( 0 ), m_nCountDown( 0 )
  {
    std::cout << "=================" << std::endl;
  };
  ~CalcRangeStats( void ) {
    std::cout << "*****************" << std::endl;
  };
  void operator() ( const CBar &bar ) {
    std::cout << bar.m_dt << " " << bar.m_dblClose << std::endl;
    ++m_nCount;
    m_dblTotalOfOpen += bar.m_dblOpen;
    m_dblTotalOfClose += bar.m_dblClose;
    m_dblAbove += bar.m_dblHigh - bar.m_dblOpen;
    m_dblBelow += bar.m_dblOpen - bar.m_dblLow;
    if ( bar.m_dblOpen > bar.m_dblClose ) ++m_nCountDown;
    if ( bar.m_dblOpen < bar.m_dblClose ) ++m_nCountUp;
  };
  operator double() { 
    std::cout << "Stats:  "
      <<  " ave above=" << m_dblAbove / m_nCount
      << ", ave below=" << m_dblBelow / m_nCount
      << ", count=" << m_nCount
      << ", up=" << m_nCountUp
      << ", down=" << m_nCountDown
      << ", ave open=" << m_dblTotalOfOpen / m_nCount
      << ", ave close=" << m_dblTotalOfClose / m_nCount
      << std::endl;
    return m_dblTotalOfOpen / m_nCount; 
  };
protected:
private:
  double m_dblTotalOfOpen;
  double m_dblTotalOfClose;
  unsigned long m_nCount;
  unsigned long m_nCountAbove;
  unsigned long m_nCountBelow;
  unsigned long m_nCountUp;
  unsigned long m_nCountDown;
  double m_dblAbove;
  double m_dblBelow;
};

CCalcAboveBelow::CCalcAboveBelow(void) {
}

CCalcAboveBelow::~CCalcAboveBelow(void) {
}

void CCalcAboveBelow::Start( void ) {
  string sPath( "/bar/86400/I/C/ICE" );
  CHDF5TimeSeriesContainer<CBar> barRepository( sPath );
  CHDF5TimeSeriesContainer<CBar>::iterator begin, end;
  end = barRepository.end();
  if ( 20 > ( end - barRepository.begin() ) ) {
    std::cout << "Ice does not have 20 or more daily bars" << std::endl;
  }
  else {
    CTimeSeries<CBar> m_bars;
    begin = end - 20;
    m_bars.Resize( 20 );
    barRepository.Read( begin, end, &m_bars );
    double avg = std::for_each( m_bars.begin(), m_bars.end(), CalcRangeStats() );
    std::cout << "ICE average open: " << avg << std::endl;
  }
}

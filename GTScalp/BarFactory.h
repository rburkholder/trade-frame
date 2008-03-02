#pragma once

#include "DatedDatum.h"
#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "FastDelegate.h"
using namespace fastdelegate;

class CBarFactory {
public:
  CBarFactory(long nSeconds = 60);
  virtual ~CBarFactory(void);
  void Add( const ptime &, double, unsigned int);
  const CBar &getCurrentBar();
  void SetBarWidth( long seconds ) { m_nBarWidthSeconds = seconds; };
  long GetBarWidth( void ) { return m_nBarWidthSeconds; };

  typedef FastDelegate1<const CBar &> OnNewBarHandler;
  void SetOnNewBar( OnNewBarHandler function ) {
    OnNewBar = function;
  }
  typedef FastDelegate1<const CBar &> OnBarUpdatedHandler;
  void SetOnBarUpdated( OnNewBarHandler function ) {
    OnBarUpdated = function;
  }

protected:
  //bool m_bEmpty;  // no values yet
  //double m_dblOpen;
  //double m_dblHigh;
  //double m_dblLow;
  //double m_dblClose;
  //unsigned int m_nVolume;
  CBar m_bar;
  OnNewBarHandler OnNewBar;
  OnNewBarHandler OnBarUpdated;
  long m_nBarWidthSeconds;
  long m_curInterval; // current bar interval
  ptime m_dtBarStart;
  ptime m_dtLastIntermediateEmission; // changes emitted no less than 1 second apart
  boost::posix_time::time_duration m_1Sec;
private:
};

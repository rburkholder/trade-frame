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
  void Add( const CTrade &trade ) { Add( trade.m_dt, trade.m_dblTrade, trade.m_nTradeSize ); };
  const CBar &getCurrentBar() { return m_bar; };
  void SetBarWidth( long seconds ) { m_nBarWidthSeconds = seconds; };
  long GetBarWidth( void ) { return m_nBarWidthSeconds; };

  typedef FastDelegate1<const CBar &> OnNewBarStartedHandler;
  void SetOnNewBarStarted( OnNewBarStartedHandler function ) {
    OnNewBarStarted = function;
  }
  typedef FastDelegate1<const CBar &> OnBarUpdatedHandler;
  void SetOnBarUpdated( OnBarUpdatedHandler function ) {  // called at most once a second
    OnBarUpdated = function;
  }
  typedef FastDelegate1<const CBar &> OnBarCompleteHandler;
  void SetOnBarComplete( OnBarCompleteHandler function ) {
    OnBarComplete = function;
  }

protected:
  CBar m_bar;
  OnNewBarStartedHandler OnNewBarStarted;
  OnBarUpdatedHandler OnBarUpdated;
  OnBarCompleteHandler OnBarComplete;
  long m_nBarWidthSeconds;
  long m_curInterval; // current bar interval
  ptime m_dtBarStart;
  ptime m_dtLastIntermediateEmission; // changes emitted no less than 1 second apart
  boost::posix_time::time_duration m_1Sec;
private:
};

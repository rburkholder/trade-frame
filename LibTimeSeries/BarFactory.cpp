#include "StdAfx.h"
#include "BarFactory.h"

CBarFactory::CBarFactory(long nSeconds) : 
    m_nBarWidthSeconds( max( 1, nSeconds ) ), m_1Sec( time_duration( 0, 0, 1 ) )
{
}

CBarFactory::~CBarFactory(void) {
  OnNewBarStarted = NULL;
  OnBarUpdated = NULL;
  OnBarComplete = NULL;
}

void CBarFactory::Add(const ptime &dt, double val, unsigned int volume) {
  long seconds = dt.time_of_day().total_seconds();
  long interval = seconds / m_nBarWidthSeconds;
  if ( m_bar.IsNull() ) {
    m_bar.m_dblClose = m_bar.m_dblHigh = m_bar.m_dblLow = m_bar.m_dblOpen = val;
    m_bar.m_nVolume = volume;
    m_curInterval = interval;
    m_bar.m_dt = ptime( dt.date(), time_duration( 0, 0, interval * m_nBarWidthSeconds, 0 ) );
    m_dtLastIntermediateEmission = dt - m_1Sec; // prime the value
    if ( NULL != OnNewBarStarted ) OnNewBarStarted( m_bar );
  }
  else {
    if ( interval > m_curInterval ) { // emit bar and start again
      if ( NULL != OnBarComplete ) OnBarComplete( m_bar );
      m_bar.m_dblClose = m_bar.m_dblHigh = m_bar.m_dblLow = m_bar.m_dblOpen = val;
      m_bar.m_nVolume = volume;
      m_curInterval = interval;
      m_bar.m_dt = ptime( dt.date(), time_duration( 0, 0, interval * m_nBarWidthSeconds, 0 ) );
      if ( NULL != OnNewBarStarted ) OnNewBarStarted( m_bar );
    }
    else { // update current interval
      m_bar.m_dblClose = val;
      m_bar.m_dblHigh = max( m_bar.m_dblHigh, val );
      m_bar.m_dblLow = min( m_bar.m_dblLow, val );
      m_bar.m_nVolume += volume;

    }
  }
  if ( m_1Sec <= ( dt - m_dtLastIntermediateEmission ) ) {
    if ( NULL != OnBarUpdated ) OnBarUpdated( m_bar );
    m_dtLastIntermediateEmission = dt;
  }
  
}


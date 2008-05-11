#pragma once
#include "slidingwindow.h"
#include "RunningStats.h"
#include "DatedDatum.h"

template<class T> class CStatsInSlidingWindow :  public CSlidingWindow<T> {
public:
  CStatsInSlidingWindow<T>(string sName, unsigned int WindowSizeSeconds, unsigned int WindowSizeCount = 0);
  virtual ~CStatsInSlidingWindow<T>(void);
  void CalcStats( void );
  CRunningStats m_stats;
protected:
  virtual void Add( ptime dt, double val );
  virtual void Remove( ptime dt, double val );
  ptime m_dtFirstTime;
//  double m_dblTicksPerSecond;
  string m_sName;
private:
};

template<class T> CStatsInSlidingWindow<T>::CStatsInSlidingWindow(
  string sName, unsigned int WindowSizeSeconds, unsigned int WindowSizeCount = 0) :
    CSlidingWindow( WindowSizeSeconds, WindowSizeCount ) {
  m_sName = sName;
//  m_dblTicksPerSecond = (double) time_duration::ticks_per_second();
}

template<class T> CStatsInSlidingWindow<T>::~CStatsInSlidingWindow(void) {
  CSlidingWindow::~CSlidingWindow();
}

template<class T> void CStatsInSlidingWindow<T>::Add(boost::posix_time::ptime dt, double val) {
  if ( CSlidingWindow::m_qT.empty() ) {
    m_dtFirstTime = dt;
  }
  time_duration dur = dt - m_dtFirstTime;
  double dif = (double) dur.total_seconds();
  m_stats.Add( dif, val );
}

template<class T> void CStatsInSlidingWindow<T>::Remove( ptime dt, double val ) {
  time_duration dur = dt - m_dtFirstTime;
  double dif = (double) dur.total_seconds();
  m_stats.Remove( dif, val );
}

template<class T> void CStatsInSlidingWindow<T>::CalcStats() {
  CSlidingWindow::UpdateWindow();
  m_stats.CalcStats();
}

// CTradeStats

class CTradeStats: CStatsInSlidingWindow<CTrade> {
public:
  CTradeStats(string sName, unsigned int WindowSizeSeconds, unsigned int WindowSizeCount = 0);
  virtual ~CTradeStats(void);
  CTrade *Add( ptime dt, CTrade *trade );
  CTrade *Remove();
protected:
private:
};

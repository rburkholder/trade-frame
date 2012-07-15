/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

#include <string>

#include <TFTimeSeries/DatedDatum.h>

#include "SlidingWindow.h"
#include "RunningStats.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

template<class T> class CStatsInSlidingWindow :  public CSlidingWindow<T> {
public:
  CStatsInSlidingWindow<T>(std::string sName, unsigned int WindowSizeSeconds, unsigned int WindowSizeCount = 0);
  virtual ~CStatsInSlidingWindow<T>(void);
  void CalcStats( void );
  CRunningStats m_stats;
protected:
  virtual void Add( ptime dt, double val );
  virtual void Remove( ptime dt, double val );
  ptime m_dtFirstTime;
  std::string m_sName;
private:
};

template<class T> CStatsInSlidingWindow<T>::CStatsInSlidingWindow(
  std::string sName, unsigned int WindowSizeSeconds, unsigned int WindowSizeCount = 0) :
    CSlidingWindow( WindowSizeSeconds, WindowSizeCount ) {
  m_sName = sName;
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

class CTradeStats: CStatsInSlidingWindow<Trade> {
public:
  CTradeStats(std::string sName, unsigned int WindowSizeSeconds, unsigned int WindowSizeCount = 0);
  virtual ~CTradeStats(void);
  Trade* Add( ptime dt, Trade *trade );
  Trade* Remove();
protected:
private:
};

} // namespace tf
} // namespace ou

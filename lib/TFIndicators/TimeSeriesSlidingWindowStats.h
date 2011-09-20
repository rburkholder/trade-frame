/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include "TimeSeriesSlidingWindow.h"
#include "RunningStats.h"

// batch mode time series processing
// used for processing a time series and obtaining the results at the end
// Construct then run Update to process the time series

// at some point, redo Add, Expire, PostUpdate as CRTP mechanism

namespace ou { // One Unified
namespace tf { // TradeFrame

template<class T> class CTimeSeriesSlidingWindowStats
: public CTimeSeriesSlidingWindow<T> {
public:
  CTimeSeriesSlidingWindowStats<T>( CTimeSeries<T> *pSeries, long WindowSizeSeconds = 0, size_t WindowSizeCount = 0 );
  virtual ~CTimeSeriesSlidingWindowStats<T>( void );
  double Accel( void ) { return m_stats.b2; };
  double Slope( void ) { return m_stats.b1; };
  double Offset( void ) { return m_stats.b0; };
  double MeanY( void ) { return m_stats.meanY; };
  double RR( void ) { return m_stats.RR; };
  double R( void ) { return m_stats.R; };
  double SD( void ) { return m_stats.SD; };
  double BBUpper( void ) { return m_stats.BBUpper; };
  double BBLower( void ) { return m_stats.BBLower; };
protected:
  virtual void Add( const T &datum ) {}; // override to process elements passing into window scope
  virtual void Expire( const T &datum ) {};  // override to process elements passing out of window scope 
  virtual void PostUpdate( void ) { m_stats.CalcStats(); };
  CRunningStats m_stats;
private:
};

// constructor
template<class T> CTimeSeriesSlidingWindowStats<T>::CTimeSeriesSlidingWindowStats( 
  CTimeSeries<T> *pSeries, long WindowSizeSeconds, size_t WindowSizeCount ) 
: CTimeSeriesSlidingWindow<T>( pSeries, WindowSizeSeconds, WindowSizeCount )
{
  m_stats.SetBBMultiplier( 2.0 );
}

template<class T> CTimeSeriesSlidingWindowStats<T>::~CTimeSeriesSlidingWindowStats(void) {
}

//
// with CTrade
//

class CTimeSeriesSlidingWindowStatsTrade: public CTimeSeriesSlidingWindowStats<CTrade> {
public:
  CTimeSeriesSlidingWindowStatsTrade( CTimeSeries<CTrade> *pSeries, long WindowSizeSeconds = 0, size_t WindowSizeCount = 0 );
  ~CTimeSeriesSlidingWindowStatsTrade( void );
protected:
  virtual void Add( const CTrade &trade ); // override to process elements passing into window scope
  virtual void Expire( const CTrade &trade );  // override to process elements passing out of window scope 
private:
};

//
// with CQuote
//

class CTimeSeriesSlidingWindowStatsQuote: public CTimeSeriesSlidingWindowStats<CQuote> {
public:
  CTimeSeriesSlidingWindowStatsQuote( CTimeSeries<CQuote> *pSeries, long WindowSizeSeconds = 0, size_t WindowSizeCount = 0 );
  ~CTimeSeriesSlidingWindowStatsQuote( void );
protected:
  virtual void Add( const CQuote &quote ); // override to process elements passing into window scope
  virtual void Expire( const CQuote &quote );  // override to process elements passing out of window scope 
private:
};

} // namespace tf
} // namespace ou

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

template<class T, class D> class TimeSeriesSlidingWindowStats
: public TimeSeriesSlidingWindow<T,D> {
public:
  TimeSeriesSlidingWindowStats<T,D>( CTimeSeries<D> *pSeries, long WindowSizeSeconds = 0, size_t WindowSizeCount = 0 );
  virtual ~TimeSeriesSlidingWindowStats<T,D>( void );
  double Accel( void ) const { return m_stats.B2(); };
  double Slope( void ) const { return m_stats.B1(); };
  double Offset( void ) const { return m_stats.B0(); };
  double MeanY( void ) const { return m_stats.MeanY(); };
  double RR( void ) const { return m_stats.RR(); };
  double R( void ) const { return m_stats.R(); };
  double SD( void ) const { return m_stats.SD(); };
  double BBUpper( void ) const { return m_stats.BBUpper(); };
  double BBLower( void ) const { return m_stats.BBLower(); };
  void SetBBMultiplier( double mult ) { m_stats.SetBBMultiplier( mult ); };
  double GetBBMultiplier( void ) const { return m_stats.SetBBMultiplier(); };
  void Reset( void ) { TimeSeriesSlidingWindow<T,D>::Reset(); m_stats.Reset(); };
protected:
//  void Add( const T &datum ) {}; // override to process elements passing into window scope
//  void Expire( const T &datum ) {};  // override to process elements passing out of window scope 
  void PostUpdate( void ) { m_stats.CalcStats(); };  // CRTP based call
  CRunningStats m_stats;
private:
};

// constructor
template<class T, class D> TimeSeriesSlidingWindowStats<T,D>::TimeSeriesSlidingWindowStats( 
  CTimeSeries<D> *pSeries, long WindowSizeSeconds, size_t WindowSizeCount ) 
: TimeSeriesSlidingWindow<T,D>( pSeries, WindowSizeSeconds, WindowSizeCount )
{
  m_stats.SetBBMultiplier( 2.0 );
}

template<class T, class D> TimeSeriesSlidingWindowStats<T,D>::~TimeSeriesSlidingWindowStats(void) {
}

// Convert the following flavours into template based actors so can be used among different indicators

//
// with CTrade
//

class TSSWStatsTrade: public TimeSeriesSlidingWindowStats<TSSWStatsTrade, CTrade> {
//  friend TimeSeriesSlidingWindowStats<TSSWStatsTrade, CTrade>;
  friend TimeSeriesSlidingWindow<TSSWStatsTrade, CTrade>;
public:
  TSSWStatsTrade( CTimeSeries<CTrade> *pSeries, long WindowSizeSeconds = 0, size_t WindowSizeCount = 0 );
  ~TSSWStatsTrade( void );
protected:
  void Add( const CTrade &trade ); // override to process elements passing into window scope
  void Expire( const CTrade &trade );  // override to process elements passing out of window scope 
private:
};

//
// with CQuote, bid, ask
//

class TSSWStatsQuote: public TimeSeriesSlidingWindowStats<TSSWStatsQuote, CQuote> {
//  friend TimeSeriesSlidingWindowStats<TSSWStatsQuote, CQuote>;
  friend TimeSeriesSlidingWindow<TSSWStatsQuote, CQuote>;
public:
  TSSWStatsQuote( CTimeSeries<CQuote> *pSeries, long WindowSizeSeconds = 0, size_t WindowSizeCount = 0 );
  ~TSSWStatsQuote( void );
protected:
  void Add( const CQuote &quote ); // override to process elements passing into window scope
  void Expire( const CQuote &quote );  // override to process elements passing out of window scope 
private:
};

//
// with CQuote, midquote
//

class TSSWStatsMidQuote: public TimeSeriesSlidingWindowStats<TSSWStatsMidQuote, CQuote> {
//  friend TimeSeriesSlidingWindowStats<TSSWStatsMidQuote, CQuote>;
  friend TimeSeriesSlidingWindow<TSSWStatsMidQuote, CQuote>;
public:
  TSSWStatsMidQuote( CTimeSeries<CQuote> *pSeries, long WindowSizeSeconds = 0, size_t WindowSizeCount = 0 );
  ~TSSWStatsMidQuote( void );
protected:
  void Add( const CQuote &quote ); // override to process elements passing into window scope
  void Expire( const CQuote &quote );  // override to process elements passing out of window scope 
private:
};

//
// with CPrice
//

class TSSWStatsPrice: public TimeSeriesSlidingWindowStats<TSSWStatsPrice, CPrice> {
//  friend TimeSeriesSlidingWindowStats<TSSWStatsTrade, CTrade>;
  friend TimeSeriesSlidingWindow<TSSWStatsPrice, CPrice>;
public:
  TSSWStatsPrice( CTimeSeries<CPrice>* pSeries, long WindowSizeSeconds = 0, size_t WindowSizeCount = 0 );
  ~TSSWStatsPrice( void );
protected:
  void Add( const CPrice &price ); // override to process elements passing into window scope
  void Expire( const CPrice &price );  // override to process elements passing out of window scope 
private:
};

} // namespace tf
} // namespace ou

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

#include "TimeSeriesSlidingWindow.h"
#include "RunningStats.h"

// continuously updated series based upon attachment to an underlying time series.

namespace ou { // One Unified
namespace tf { // TradeFrame

template<class T, class D> 
class TimeSeriesSlidingWindowStats
: public TimeSeriesSlidingWindow<T,D> {
public:
  TimeSeriesSlidingWindowStats<T,D>( TimeSeries<D>& Series, time_duration tdWindowWidth, size_t WindowSizeCount = 0 );
  virtual ~TimeSeriesSlidingWindowStats<T,D>( void );
//  double Accel( void ) const { return m_stats.B2(); };
  double Slope( void ) const { return m_stats.B1(); };
  double Offset( void ) const { return m_stats.B0(); };
  double MeanY( void ) const { return m_stats.MeanY(); };
  double RR( void ) const { return m_stats.RR(); };
  double R( void ) const { return m_stats.R(); };
  double SD( void ) const { return m_stats.SD(); };
  double BBOffset( void ) const { return m_stats.BBOffset(); };
  double BBUpper( void ) const { return m_stats.BBUpper(); };
  double BBLower( void ) const { return m_stats.BBLower(); };
  void SetBBMultiplier( double mult ) { m_stats.SetBBMultiplier( mult ); };
  double GetBBMultiplier( void ) const { return m_stats.SetBBMultiplier(); };
  void Reset( void ) { TimeSeriesSlidingWindow<T,D>::Reset(); m_stats.Reset(); };
protected:
//  void Add( const T &datum ) {}; // override to process elements passing into window scope
//  void Expire( const T &datum ) {};  // override to process elements passing out of window scope 
  void PostUpdate( void ) { m_stats.CalcStats(); };  // CRTP based call
  RunningStats m_stats;
private:
};

// constructor
template<class T, class D> TimeSeriesSlidingWindowStats<T,D>::TimeSeriesSlidingWindowStats( 
  TimeSeries<D>& Series, time_duration tdWindowWidth, size_t WindowSizeCount ) 
: TimeSeriesSlidingWindow<T,D>( Series, tdWindowWidth, WindowSizeCount )
{
  m_stats.SetBBMultiplier( 2.0 );
}

template<class T, class D> TimeSeriesSlidingWindowStats<T,D>::~TimeSeriesSlidingWindowStats(void) {
}

// Convert the following flavours into template based actors so can be used among different indicators

//
// with Trade
//

class TSSWStatsTrade: public TimeSeriesSlidingWindowStats<TSSWStatsTrade, Trade> {
//  friend TimeSeriesSlidingWindowStats<TSSWStatsTrade, Trade>;
  friend TimeSeriesSlidingWindow<TSSWStatsTrade, Trade>;
public:
  TSSWStatsTrade( TimeSeries<Trade>& series, time_duration tdWindowWidth, size_t WindowSizeCount = 0 );
  ~TSSWStatsTrade( void );
protected:
  void Add( const Trade &trade ); // override to process elements passing into window scope
  void Expire( const Trade &trade );  // override to process elements passing out of window scope 
private:
};

//
// with Quote, bid, ask
//

class TSSWStatsQuote: public TimeSeriesSlidingWindowStats<TSSWStatsQuote, Quote> {
//  friend TimeSeriesSlidingWindowStats<TSSWStatsQuote, Quote>;
  friend TimeSeriesSlidingWindow<TSSWStatsQuote, Quote>;
public:
  TSSWStatsQuote( TimeSeries<Quote>& series, time_duration tdWindowWidth, size_t WindowSizeCount = 0 );
  ~TSSWStatsQuote( void );
protected:
  void Add( const Quote &quote ); // override to process elements passing into window scope
  void Expire( const Quote &quote );  // override to process elements passing out of window scope 
private:
};

//
// with Quote, midquote
//

class TSSWStatsMidQuote: public TimeSeriesSlidingWindowStats<TSSWStatsMidQuote, Quote> {
//  friend TimeSeriesSlidingWindowStats<TSSWStatsMidQuote, Quote>;
  friend TimeSeriesSlidingWindow<TSSWStatsMidQuote, Quote>;
public:
  TSSWStatsMidQuote( Quotes& series, time_duration tdWindowWidth, size_t WindowSizeCount = 0 );
  ~TSSWStatsMidQuote( void );
protected:
  void Add( const Quote &quote ); // override to process elements passing into window scope
  void Expire( const Quote &quote );  // override to process elements passing out of window scope 
private:
};

//
// with Price
//

class TSSWStatsPrice: public TimeSeriesSlidingWindowStats<TSSWStatsPrice, Price> {
//  friend TimeSeriesSlidingWindowStats<TSSWStatsTrade, Trade>;
  friend TimeSeriesSlidingWindow<TSSWStatsPrice, Price>;
public:
  TSSWStatsPrice( TimeSeries<Price>& series, time_duration tdWindowWidth, size_t WindowSizeCount = 0 );
  ~TSSWStatsPrice( void );
protected:
  void Add( const Price &price ); // override to process elements passing into window scope
  void Expire( const Price &price );  // override to process elements passing out of window scope 
private:
};

} // namespace tf
} // namespace ou

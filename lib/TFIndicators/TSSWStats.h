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

#include "RunningStats.h"
#include "TimeSeriesSlidingWindow.h"

// continuously updated series based upon attachment to an underlying time series.

namespace ou { // One Unified
namespace tf { // TradeFrame

template<class T, class D>
class TimeSeriesSlidingWindowStats
: public TimeSeriesSlidingWindow<T,D> {
  friend TimeSeriesSlidingWindow<T,D>;
public:

  TimeSeriesSlidingWindowStats<T,D>( TimeSeries<D>& Series, time_duration tdWindowWidth, size_t WindowSizeCount = 0 );
  TimeSeriesSlidingWindowStats<T,D>( TimeSeries<D>& Series, size_t nPeriods, time_duration tdPeriodWidth, size_t WindowSizeCount = 0 );
  TimeSeriesSlidingWindowStats<T,D>( const TimeSeriesSlidingWindowStats<T,D>& rhs );
  TimeSeriesSlidingWindowStats<T,D>( TimeSeriesSlidingWindowStats<T,D>&& rhs );
  virtual ~TimeSeriesSlidingWindowStats<T,D>();

//  double Accel( void ) const { return m_stats.B2(); };
  double Slope() const { return m_stats.Slope(); }
  double Offset() const { return m_stats.Offset(); }
  double MeanY() const { return m_stats.MeanY(); }
  double RR() const { return m_stats.RR(); }
  double R() const { return m_stats.R(); }
  double SD() const { return m_stats.SD(); }
  double BBOffset() const { return m_stats.BBOffset(); }
  double BBUpper() const { return m_stats.BBUpper(); }
  double BBLower() const { return m_stats.BBLower(); }
  double SumY() const { return m_stats.SumY(); }

  void SetBBMultiplier( double mult ) { m_stats.SetBBMultiplier( mult ); }
  double GetBBMultiplier() const { return m_stats.GetBBMultiplier(); }

  void Reset() {
    TimeSeriesSlidingWindow<T,D>::Reset();
    m_stats.Reset();
    };

  struct Results {
    boost::posix_time::ptime dt;
    const linear::Stats& stats;
    Results( boost::posix_time::ptime dt_, const linear::Stats& stats_ )
    : dt( dt_ ), stats( stats_ ) {}
  };

  Delegate<const Results&> OnUpdate;

protected:
  // void Add( const T &datum ) {}; // override to process elements passing into window scope
  // void Expire( const T &datum ) {};  // override to process elements passing out of window scope

  boost::posix_time::ptime m_dtLast;
  RunningStats m_stats;

private:
  void PostUpdate() {   // CRTP based call
    m_stats.CalcStats();
    OnUpdate( Results( m_dtLast, m_stats.Get() ) );
  };
};

// constructor
template<class T, class D>
TimeSeriesSlidingWindowStats<T,D>::TimeSeriesSlidingWindowStats(
  TimeSeries<D>& Series, time_duration tdWindowWidth, size_t WindowSizeCount )
: TimeSeriesSlidingWindow<T,D>( Series, tdWindowWidth, WindowSizeCount )
{
  m_stats.SetBBMultiplier( 2.0 );
}

template<class T, class D>
TimeSeriesSlidingWindowStats<T,D>::TimeSeriesSlidingWindowStats(
  TimeSeries<D>& Series, size_t nPeriods, time_duration tdPeriodWidth, size_t WindowSizeCount )
: TimeSeriesSlidingWindow<T,D>( Series, nPeriods, tdPeriodWidth, WindowSizeCount )
{
  m_stats.SetBBMultiplier( 2.0 );
}

template<class T, class D>
TimeSeriesSlidingWindowStats<T,D>::TimeSeriesSlidingWindowStats(
  const TimeSeriesSlidingWindowStats<T,D>& rhs )
: TimeSeriesSlidingWindow<T,D>( rhs ), m_stats( rhs.m_stats )
{}

template<class T, class D>
TimeSeriesSlidingWindowStats<T,D>::TimeSeriesSlidingWindowStats( TimeSeriesSlidingWindowStats<T,D>&& rhs )
: TimeSeriesSlidingWindow<T,D>( rhs ), m_stats( std::move( rhs.m_stats ) )
{}

template<class T, class D> TimeSeriesSlidingWindowStats<T,D>::~TimeSeriesSlidingWindowStats() {
}

// Convert the following flavours into template based actors so can be used among different indicators

//
// with Trade
//

class TSSWStatsTrade: public TimeSeriesSlidingWindowStats<TSSWStatsTrade, Trade> {
  friend TimeSeriesSlidingWindow<TSSWStatsTrade, Trade>;
public:
  TSSWStatsTrade( TimeSeries<Trade>& series, time_duration tdWindowWidth, size_t WindowSizeCount = 0 );
  TSSWStatsTrade( const TSSWStatsTrade& rhs );
  virtual ~TSSWStatsTrade();
protected:
  void Add( const Trade &trade ); // override to process elements passing into window scope
  void Expire( const Trade &trade );  // override to process elements passing out of window scope
private:
};

//
// with Quote, bid, ask
//

class TSSWStatsQuote: public TimeSeriesSlidingWindowStats<TSSWStatsQuote, Quote> {
  friend TimeSeriesSlidingWindow<TSSWStatsQuote, Quote>;
public:
  TSSWStatsQuote( TimeSeries<Quote>& series, time_duration tdWindowWidth, size_t WindowSizeCount = 0 );
  TSSWStatsQuote( const TSSWStatsQuote& rhs );
  virtual ~TSSWStatsQuote();
protected:
  void Add( const Quote &quote ); // override to process elements passing into window scope
  void Expire( const Quote &quote );  // override to process elements passing out of window scope
private:
};

//
// with Quote, midquote
//

class TSSWStatsMidQuote: public TimeSeriesSlidingWindowStats<TSSWStatsMidQuote, Quote> {
  friend TimeSeriesSlidingWindow<TSSWStatsMidQuote, Quote>;
public:
  TSSWStatsMidQuote( Quotes& series, time_duration tdWindowWidth, size_t WindowSizeCount = 0 );
  TSSWStatsMidQuote( Quotes& series, size_t nPeriods, time_duration tdPeriodWidth, size_type WindowSizeCount = 0 );
  TSSWStatsMidQuote( const TSSWStatsMidQuote& );
  TSSWStatsMidQuote( TSSWStatsMidQuote&& );
  virtual ~TSSWStatsMidQuote();
protected:
  void Add( const Quote &quote ); // override to process elements passing into window scope
  void Expire( const Quote &quote );  // override to process elements passing out of window scope
private:
};

//
// with Price
//

class TSSWStatsPrice: public TimeSeriesSlidingWindowStats<TSSWStatsPrice, Price> {
  friend TimeSeriesSlidingWindow<TSSWStatsPrice, Price>;
public:
  TSSWStatsPrice( TimeSeries<Price>& series, time_duration tdWindowWidth, size_t WindowSizeCount = 0 );
  TSSWStatsPrice( const TSSWStatsPrice& rhs );
  virtual ~TSSWStatsPrice();
protected:
  void Add( const Price &price ); // override to process elements passing into window scope
  void Expire( const Price &price );  // override to process elements passing out of window scope
private:
};

} // namespace tf
} // namespace ou

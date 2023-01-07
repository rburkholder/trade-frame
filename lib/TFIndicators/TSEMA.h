/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// could use traits template mechanism to deal with the multiple GetPrice methods

#include <math.h>

#include <TFTimeSeries/TimeSeries.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

template<class D> // D => type derived from DatedDatum
class TSEMA: public ou::tf::Prices {  // new time series built up from linked time series
public:

  TSEMA( ou::tf::TimeSeries<D>& series, time_duration );
  TSEMA<D>( ou::tf::TimeSeries<D>& series, size_t nPeriods, time_duration tdPeriodWidth );
  TSEMA( const TSEMA& );
  TSEMA( TSEMA&& );
  virtual ~TSEMA();

  double GetEMA() const { return m_dblRecentEMA; };

  ou::Delegate<const ou::tf::Price&> OnUpdate;

protected:
private:

  time_duration m_tdTimeRange;
  double m_dblTimeRange;
  TimeSeries<D>& m_seriesSource;
  double m_XatTminus1;
  double m_dblRecentEMA;

  void EMA( ptime t, double XatT );

  void HandleAppend( const D& datum ) {
    EMA( datum.DateTime(), GetPrice( datum ) );
  }

  double GetPrice( const Price& price ) const {
    return price.Value();
  }

  double GetPrice( const Quote& quote ) const {
    return quote.Midpoint();
  }

  double GetPrice( const Trade& trade ) const {
    return trade.Price();
  }

/*
  // http://stackoverflow.com/questions/6013066/explicit-specialization-template-class-member-function-with-different-return-typ
  void HandleAppend( const D& datum ) { EMA_impl<TSEMA<D>,D>::calc( *this, datum ); };

  template<class T, class=void> struct EMA_impl {
    static void calc( T& t, const D& ) { throw std::logic_error( "no specialization" ); };
  };
  template<class T> struct EMA_impl<Price,T> {
    static void calc( T& t, const Price& price ) {
      t.EMA( price.DateTime(), price.Price() );
    };
  };
  template<class T> struct EMA_impl<Quote,T> {
    static void calc( T& t, const Quote& quote ) {
      t.EMA( quote.DateTime(), quote.LogarithmicMidPointA() );
    };
  };
  template<class T> struct EMA_impl<Trade,T> {
    static void calc( T& t, const Trade& trade ) {
      t.EMA( trade.DateTime(), trade.Trade() );
    };
  };
*/
};

template<class D>
TSEMA<D>::TSEMA( ou::tf::TimeSeries<D>& series, time_duration td )
: Prices()
, m_seriesSource( series )
, m_tdTimeRange( td ), m_XatTminus1( 0.0 ), m_dblRecentEMA( 0.0 )
{
  assert( 0 < td.total_seconds() );
  m_dblTimeRange = (double) td.total_microseconds();
  m_seriesSource.OnAppend.Add( MakeDelegate( this, &TSEMA<D>::HandleAppend ) );
}

template<class D>
TSEMA<D>::TSEMA( ou::tf::TimeSeries<D>& series, size_t nPeriods, time_duration tdPeriodWidth )
: Prices()
, m_seriesSource( series )
, m_XatTminus1( 0.0 ), m_dblRecentEMA( 0.0 )
{
  assert( 0 < nPeriods );
  assert( 0 < tdPeriodWidth.total_seconds() );
  time_duration tdSum {};
  while ( 0 != nPeriods ) {
    tdSum += tdPeriodWidth;
    nPeriods--;
  }
  m_tdTimeRange = tdSum;
  m_dblTimeRange = (double) m_tdTimeRange.total_microseconds();
  m_seriesSource.OnAppend.Add( MakeDelegate( this, &TSEMA<D>::HandleAppend ) );
}

template<class D>
TSEMA<D>::TSEMA( const TSEMA<D>& rhs )
: Prices()
, m_seriesSource( rhs.m_seriesSource )
, m_tdTimeRange( rhs.m_tdTimeRange )
, m_dblTimeRange( rhs.m_dblTimeRange )
, m_dblRecentEMA( rhs.m_dblRecentEMA )
, m_XatTminus1( rhs.m_XatTminus1 )
{
  //rhs.m_seriesSource.OnAppend.Remove( MakeDelegate( &rhs, &TSEMA<D>::HandleAppend ) ); // proper but causes problems?
  m_seriesSource.OnAppend.Add( MakeDelegate( this, &TSEMA<D>::HandleAppend ) );
}

template<class D>
TSEMA<D>::TSEMA( TSEMA<D>&& rhs )
: ou::tf::Prices()
, m_seriesSource( rhs.m_seriesSource )
, m_tdTimeRange( rhs.m_tdTimeRange )
, m_dblTimeRange( rhs.m_dblTimeRange )
, m_XatTminus1( rhs.m_XatTminus1 )
, m_dblRecentEMA( rhs.m_dblRecentEMA )
{
  //rhs.m_seriesSource.OnAppend.Remove( MakeDelegate( &rhs, &TSEMA<D>::HandleAppend ) ); // proper but causes problems?
  m_seriesSource.OnAppend.Add( MakeDelegate( this, &TSEMA<D>::HandleAppend ) );
}

template<class D>
TSEMA<D>::~TSEMA() {
  m_seriesSource.OnAppend.Remove( MakeDelegate( this, &TSEMA<D>::HandleAppend ) );
}

// refer to paper : "Specially Weighted Movng Averages With Repeated Application of the EMA Operator"
// Intro to HF Finance, pg 59, has further variation
template<class D>
void TSEMA<D>::EMA( ptime dt, double XatT ) {
  static const time_duration tdOne( microseconds( 1 ) );
  if ( 0 == Prices::Size() ) {
    m_dblRecentEMA = XatT;
    m_XatTminus1 = XatT;
    ou::tf::Prices::Append( ou::tf::Price( dt, XatT ) );  // initialize first element of series
  }
  else {
    const Price& prvEMA( ou::tf::Prices::last() );
    ptime dtPrv = prvEMA.DateTime();
    time_duration tdDif = dt == dtPrv ? tdOne : dt - dtPrv;
    double alpha = ( (double) tdDif.total_microseconds() ) / m_dblTimeRange;
    double mu = std::exp( -alpha );  // used with any form of interpolation
    double v = ( 1.0 - mu ) / alpha;  // linear interpolation
    //double v = 1.0;  // previous point
    //double v = std::exp( -alpha / 2.0 ); // or std::sqrt( mu );  // nearest value
    //double v = mu;  // next point
    m_dblRecentEMA = mu * prvEMA.Value() + ( v - mu ) * m_XatTminus1 + ( 1.0 - v ) * XatT; // ema calc
    m_XatTminus1 = XatT;

    const ou::tf::Price price( dt, m_dblRecentEMA );
    ou::tf::Prices::Append( price ); // inherited class
    OnUpdate( price );
  }

}

} // namespace hf
} // namespace tf
} // namespace ou


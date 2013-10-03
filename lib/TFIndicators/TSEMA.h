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
class TSEMA: public Prices {  // new time series built up from linked time series
public:
  TSEMA( TimeSeries<D>& series, time_duration td );
  virtual ~TSEMA(void);
  double GetEMA( void ) { return m_dblRecentEMA; };
protected:
private:
  time_duration m_tdTimeRange;
  double m_dblTimeRange;
  TimeSeries<D>& m_seriesSource;
  double m_XatTminus1;
  double EMA( ptime t, double XatT );
  double m_dblRecentEMA;

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
TSEMA<D>::TSEMA( TimeSeries<D>& series, time_duration td )
  : m_seriesSource( series ), m_tdTimeRange( td ), m_XatTminus1( 0.0 ), m_dblRecentEMA( 0.0 )
{
  assert( 0 < td.total_seconds() );
  m_dblTimeRange = (double) td.total_microseconds();
  series.OnAppend.Add( MakeDelegate( this, &TSEMA<D>::HandleAppend ) );
}

template<class D>
TSEMA<D>::~TSEMA( void ) {
  m_seriesSource.OnAppend.Remove( MakeDelegate( this, &TSEMA<D>::HandleAppend ) );
}

// refer to paper : "Specially Weighted Movng Averages With Repeated Application of the EMA Operator"
// Intro to HF Finance, pg 59, has further variation
template<class D>
double TSEMA<D>::EMA( ptime t, double XatT ) {
  static const time_duration tdOne( microseconds( 1 ) );
  if ( 0 == Prices::Size() ) {
    m_dblRecentEMA = XatT;
    m_XatTminus1 = XatT;
    Prices::Append( Price( t, XatT ) );  // initialize first element of series
  }
  else { 
    const Price& prvEMA( Prices::Ago( 0 ) );
    ptime dtPrv = prvEMA.DateTime();
    time_duration tdDif = t == dtPrv ? tdOne : t - dtPrv;
    double alpha = ( (double) tdDif.total_microseconds() ) / m_dblTimeRange;
    double mu = std::exp( -alpha );  // used with any form of interpolation
    double v = ( 1.0 - mu ) / alpha;  // linear interpolation
    //double v = 1.0;  // previous point 
    //double v = std::exp( -alpha / 2.0 ); // or std::sqrt( mu );  // nearest value
    //double v = mu;  // next point
    m_dblRecentEMA = mu * prvEMA.Value() + ( v - mu ) * m_XatTminus1 + ( 1.0 - v ) * XatT; // ema calc
    m_XatTminus1 = XatT;
    Prices::Append( Price( t, m_dblRecentEMA ) );
  }
  return m_dblRecentEMA;

}

} // namespace hf
} // namespace tf
} // namespace ou


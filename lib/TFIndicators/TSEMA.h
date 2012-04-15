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

#include <math.h>

#include <TFTimeSeries/TimeSeries.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

template<class D> // D => type derived from DatedDatum
class TSEMA:
  public CPrices
{
public:
  TSEMA( CTimeSeries<D>& series, time_duration dt );
  virtual ~TSEMA(void);
protected:
private:
  time_duration m_dtTimeRange;
  double m_dblTimeRange;
  CTimeSeries<D>& m_seriesSource;
  double m_XatTminus1;
  double EMA( ptime t, double XatT );
  void HandleAppend( const CPrice& );
  void HandleAppend( const CQuote& );
  void HandleAppend( const CTrade& );
};

template<class D>
TSEMA<D>::TSEMA( CTimeSeries<D>& series, time_duration dt )
  : m_seriesSource( series ), m_dtTimeRange( dt ), m_prvX( 0.0 )
{
  assert( 0 < dt.total_milliseconds() );
  m_dblTimeRange = (double) dt.total_milliseconds();
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
  double ema( XatT );
  if ( 0 == CPrices::Size() ) {
    CPrices::Append( CPrice( t, XatT ) );  // initialize first element of series
  }
  else { 
    const CPrice& prvEMA( CPrices::Ago( 0 ) );
    double alpha = (double) ( ( t - prvEMA.DateTime() ).total_milliseconds() ) / m_dblTimeRange;
    double mu = std::exp( -alpha );  // used with any form of interpolation
    double v = ( 1.0 - mu ) / alpha;  // linear interpolation
    //double v = 1.0;  // previous point 
    //double v = std::exp( -alpha / 2.0 ); // or std::sqrt( mu );  // nearest value
    //double v = mu;  // next point
    ema = mu * prvEMA.Price() + ( v - mu ) * m_XatTminus1 + ( 1.0 - v ) * XatT; // ema calc
    CPrices::Append( CPrice( t, ema ) );
  }
  m_XatTminus1 = XatT;
  return ema;
}

template<class D>
void TSEMA<D>::HandleAppend( const CPrice& price ) {
  EMA( price.DateTime(), price.Price() );
}

template<class D>
void TSEMA<D>::HandleAppend( const CQuote& quote ) {
  EMA( quote.DateTime(), quote.LogarithmicMidPointA() );
}

template<class D>
void TSEMA<D>::HandleAppend( const CTrade& trade ) {
  EMA( trade.DateTime(), trade.Trade() );
}

} // namespace tf
} // namespace ou


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
  double m_zPrev;
  double EMA( ptime t, double zt );
  void HandleAppend( const CPrice& );
  void HandleAppend( const CQuote& );
  void HandleAppend( const CTrade& );
};

template<class D>
TSEMA<D>::TSEMA( CTimeSeries<D>& series, time_duration dt )
  : m_seriesSource( series ), m_dtTimeRange( dt ), m_zPrev( 0.0 )
{
  assert( 0 < dt.total_milliseconds() );
  m_dblTimeRange = dt.total_milliseconds();
  series.OnAppend.Add( MakeDelegate( this, &TSEMA<D>::HandleAppend ) );
}

template<class D>
TSEMA<D>::~TSEMA( void ) {
  m_seriesSource.OnAppend.Remove( MakeDelegate( this, &TSEMA<D>::HandleAppend ) );
}

template<class D>
double TSEMA<D>::EMA( ptime t, double zt ) {
  if ( 0 == CPrices::Size() ) {
    m_zPrev = zt;
    CPrices::Append( CPrice( t, zt ) );
  }
  else { // pg 59, Intro HF Finance
    const CPrice& prev( CPrices::Ago( 0 ) );
    double alpha = (double) ( ( t - prev.DateTime() ).total_milliseconds() ) / m_dblTimeRange;
    double mu = std::exp( -alpha );
    //double v = 1.0;  // previous point 
    double v = ( 1.0 - mu ) / alpha;  // interpolation
    //double v = mu;  // next point
    double ema = mu * prev.Price() + ( v - mu ) * m_zPrev + ( 1.0 - v ) * zt;
    m_zPrev = zt;
    CPrices::Append( CPrice( t, ema ) );
  }
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


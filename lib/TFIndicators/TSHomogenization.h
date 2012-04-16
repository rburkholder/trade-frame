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

#include <OUCommon/Delegate.h>

#include <TFTimeSeries/TimeSeries.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

template<typename T>
class TSHomogenization {
private:
  static time_duration m_zeroDuration;
public:

  enum interpolation_t { eNone, ePreviousTick, eLinear };

  TSHomogenization<T>( CTimeSeries<T>&, time_duration interval = m_zeroDuration, interpolation_t interpolation = eNone );
  virtual ~TSHomogenization<T>(void);

  ou::Delegate<const T&> OnAppend;

protected:
private:
  interpolation_t m_interpolation;
  CTimeSeries<T>& m_ts;
  time_duration m_tdHomogenizingInterval;  // 0 if no homoginization
  ptime m_dtMarker;
  T m_datum;  // this is Tau at j
  void HandleFirstDatum( const T& );
  void HandleDatum( const T& ); // this is Tau at j+1, need to handle CPrice, CTrade, CBar
  void FlowThrough( const T& ) { OnAppend( datum ); };

  void CalcDatum( const CPrice& price, double ratio );
  void CalcDatum( const CTrade& trade, double ratio );
};

template<typename T>
time_duration TSHomogenization<T>::m_zeroDuration = time_duration( 0, 0, 0 );

template<typename T>
TSHomogenization<T>::TSHomogenization( CTimeSeries<T>& ts, time_duration interval, interpolation_t interpolation ) 
  : m_zeroDuration( time_duration( 0, 0, 0 ) ), m_nHomogenizingInterval( interval ), 
    m_ts( ts ), m_interpolation( interpolation ), m_dtMarker( not_a_date_time )
{
  if ( m_zeroDuration == interval ) assert( eNone == m_interpolation );
  if ( m_zeroDuration != interval ) assert( eNone != m_interpolation );
  switch ( m_interpolation ) {
  case eNone:
    ts.OnAppend.Add( MakeDelegate( this, &TSHomogenization<T>::FlowThrough ) );
    break
  case ePreviousTick:
  case eLinear:
    ts.OnAppend.Add( MakeDelegate( this, &TSHomogenization<T>::HandleFirstDatum ) );
    break;
  }
}

template<typename T>
TSHomogenization<T>::~TSHomogenization( void ) {
  switch ( m_interpolation ) {
  case eNone:
    m_ts.OnAppend.Remove( MakeDelegate( this, &TSHomogenization<T>::FlowThrough ) );
    break
  case ePreviousTick:
  case eLinear:
    if ( not_a_date_time == m_dtInterval ) {
      m_ts.OnAppend.Remove( MakeDelegate( this, &TSHomogenization<T>::HandleFirstDatum ) );
    }
    else {
      m_ts.OnAppend.Remove( MakeDelegate( this, &TSHomogenization<T>::HandleDatum ) );
    }
    break;
  }
}

template<typename T>
void TSHomogenization<T>::HandleFirstDatum( const T& datum ) {
  m_ts.OnAppend.Remove( MakeDelegate( this, &TSHomogenization<T>::HandleFirstDatum ) );
  m_datum = datum;
  m_dtMarker = datum.DateTime() % m_tdHomogenizingInterval;
  if ( m_dtMarker == datum.DateTime() ) {
    HandleDatum( datum );
  }
  else {
    m_dtMarker += m_tdHomogenizingInterval;
    assert( m_dtMarker > datum.DateTime() );
  }
  m_ts.OnAppend.Add( MakeDelegate( this, &TSHomogenization<T>::HandleDatum ) );
}

template<typename T>
void TSHomogenization<T>::HandleDatum( const T& datum ) {
  if ( m_dtMarker == datum.DateTime() ) {
    OnAppend( datum );
  }
  else {
    if ( datum.DateTime() > m_dtMarker ) {
      switch ( m_interpolation ) {
      case ePreviousTick:
        OnAppend( m_datum );
        break;
      case eLinear:
        TimeDuration numerator( m_dtMarker - m_datum.DateTime() );
        TimeDuration denomenator( datum.DateTime() - m_datam.DateTime() );
        double ratio = ( (double) numerator.total_microseconds() ) / ( (double) denomenator.total_microseconds );
        CalcDatum( datum, ratio );
        break;
      }
      while ( m_dtMarker <= datum.DateTime() ) m_dtMarker += m_tdHomogenizingInterval;
    }
  }
  m_datum = datum;
}

template<typename T>
void TSHomogenization<T>::CalcDatum( const CPrice& datum, double ratio ) {
  CPrice price( m_dtMarker, m_datum.Price() + ratio * ( datum.Price() - m_datum.Price ) );
  OnAppend( price );
}

template<typename T>
void TSHomogenization<T>::CalcDatum( const CTrade& datum, double ratio ) {
  CTrade trade( m_dtMarker, m_datum.Trade() + ratio * ( datum.Trade() - m_datum.Trade ), m_datum.Volume() );
  OnAppend( trade );
}

} // namespace hf
} // namespace tf
} // namespace ou


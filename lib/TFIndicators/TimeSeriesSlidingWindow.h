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

#include <LibTimeSeries/TimeSeries.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

template<class T> class CTimeSeriesSlidingWindow {
public:
  CTimeSeriesSlidingWindow<T>( CTimeSeries<T> *pSeries, long WindowSizeSeconds = 0, size_t WindowSizeCount = 0 );
  virtual ~CTimeSeriesSlidingWindow<T>(void);
  void Update( void );
protected:
  virtual void Add( const T &datum ) {}; // override to process elements passing into window scope
  virtual void Expire( const T &datum ) {};  // override to process elements passing out of window scope 
  virtual void PostUpdate( void ) {};
  ptime m_dtZero;  // datetime of first element, used as offset
private:
  CTimeSeries<T> *m_pSeries;
  long m_nWindowSizeSeconds;
  size_t m_nWindowSizeCount;
  time_duration m_tdWindowWidth;
  size_t m_ixTrailing;  // index to datums to be processed out (expired)
  size_t m_ixLeading;  // index to vector end of datums to be processed in
  ptime m_dtLeading;
  bool m_bFirstDatumFound;
};

template<class T> CTimeSeriesSlidingWindow<T>::CTimeSeriesSlidingWindow( 
  CTimeSeries<T> *pSeries, long WindowSizeSeconds, size_t WindowSizeCount ) 
: m_pSeries( pSeries ), //m_iterTrailing( pSeries->begin() ), 
  m_ixTrailing( 0 ), m_ixLeading( 0 ), m_dtLeading( not_a_date_time ),
  m_nWindowSizeSeconds( WindowSizeSeconds ), m_nWindowSizeCount( WindowSizeCount ),
  m_tdWindowWidth( seconds(WindowSizeSeconds) ),
  m_bFirstDatumFound( false )
{
  assert( 0 <= WindowSizeSeconds );
  assert( 0 <= WindowSizeCount );
}

template<class T> CTimeSeriesSlidingWindow<T>::~CTimeSeriesSlidingWindow(void) {
}

template<class T> void CTimeSeriesSlidingWindow<T>::Update( void ) {
  if ( !m_bFirstDatumFound ) {
    if ( 0 < m_pSeries->Size() ) {
      T *datum = (*m_pSeries)[ 0 ];
      m_dtZero = datum->m_dt;
      m_bFirstDatumFound = true;
    }
  }
  while ( m_ixLeading < m_pSeries->Size() ) {
    T *datum = (*m_pSeries)[ m_ixLeading ];
    m_dtLeading = datum->m_dt;
    Add( *datum );
    ++m_ixLeading;
  }
  if ( 0 < m_pSeries->Size() ) {
    if ( 0 < m_nWindowSizeCount ) {
      while ( ( m_ixLeading - m_ixTrailing ) > m_nWindowSizeCount ) {
        T *datum = (*m_pSeries)[ m_ixTrailing ];
        Expire( *datum );
        ++m_ixTrailing;
      }
    }
    if ( 0 < m_nWindowSizeSeconds ) {
      T *datum = (*m_pSeries)[ m_ixTrailing ];
      time_duration dif = m_dtLeading - datum->m_dt;
      while ( dif > m_tdWindowWidth ) {
        Expire( *datum );
        ++m_ixTrailing;
        datum = (*m_pSeries)[ m_ixTrailing ];
        dif = m_dtLeading - datum->m_dt;
      }
    }
  }
  PostUpdate();
}

} // namespace tf
} // namespace ou

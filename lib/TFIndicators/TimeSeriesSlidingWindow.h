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

// used with TimeSeriesSlidingWindowStats in batch mode on a TimeSeries
// Construct then run Update to process the time series
// Each time timeseries updated, run Update to continue
// useful when timeseries serves multiple windows

#include <TFTimeSeries/TimeSeries.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

template<class T, class D> class TimeSeriesSlidingWindow { // T=CRTP class for Add, Expire, PostUpdate; D=DatedDatum
public:
  TimeSeriesSlidingWindow<T,D>( CTimeSeries<D>& Series, long WindowSizeSeconds = 0, size_t WindowSizeCount = 0 );
  TimeSeriesSlidingWindow<T,D>( const TimeSeriesSlidingWindow<T,D>& );
  virtual ~TimeSeriesSlidingWindow<T,D>(void);
  void Update( void );
  virtual void Reset( void );
protected:
  void Add( const D &datum ) {}; // CRTP override to process elements passing into window scope
  void Expire( const D &datum ) {};  // CRTP override to process elements passing out of window scope 
  void PostUpdate( void ) {};  // CRTP override to do final calcs
  ptime m_dtZero;  // datetime of first element, used as offset
private:
  CTimeSeries<D>& m_Series;
  long m_nWindowSizeSeconds;
  size_t m_nWindowSizeCount;
  time_duration m_tdWindowWidth;
  size_t m_ixTrailing;  // index to datums to be processed out (expired)
  size_t m_ixLeading;  // index to vector end of datums to be processed in
  ptime m_dtLeading;
  bool m_bFirstDatumFound;
};

template<class T, class D> TimeSeriesSlidingWindow<T,D>::TimeSeriesSlidingWindow( 
  CTimeSeries<D>& Series, long WindowSizeSeconds, size_t WindowSizeCount ) 
: m_Series( Series ), //m_iterTrailing( Series.begin() ), 
  m_ixTrailing( 0 ), m_ixLeading( 0 ), m_dtLeading( not_a_date_time ),
  m_nWindowSizeSeconds( WindowSizeSeconds ), m_nWindowSizeCount( WindowSizeCount ),
  m_tdWindowWidth( seconds(WindowSizeSeconds) ),
  m_bFirstDatumFound( false )
{
  assert( 0 <= WindowSizeSeconds );
  assert( 0 <= WindowSizeCount );
}

template<class T, class D> TimeSeriesSlidingWindow<T,D>::TimeSeriesSlidingWindow( const TimeSeriesSlidingWindow<T,D>& sw ) 
  : m_Series( sw.m_Series ), m_nWindowSizeSeconds( sw.m_nWindowSizeSeconds ),
  m_nWindowSizeCount( sw.m_nWindowSizeCount ), m_tdWindowWidth( sw.m_tdWindowWidth ),
  m_ixTrailing( sw.m_ixTrailing ), m_ixLeading( sw.m_ixLeading ), m_dtLeading( sw.m_dtLeading ),
  m_bFirstDatumFound( sw.m_bFirstDatumFound ), m_dtZero( sw.m_dtZero )
{
}

template<class T, class D> TimeSeriesSlidingWindow<T,D>::~TimeSeriesSlidingWindow(void) {
}

template<class T, class D> void TimeSeriesSlidingWindow<T,D>::Reset( void ) {
  m_ixTrailing = m_ixLeading = 0;
  m_dtLeading = not_a_date_time;
}

template<class T, class D> void TimeSeriesSlidingWindow<T,D>::Update( void ) {
  if ( !m_bFirstDatumFound ) {
    if ( 0 < m_Series.Size() ) {
      //T *datum = (*m_pSeries)[ 0 ]->m_dt;
      m_dtZero = m_Series[ 0 ]->DateTime();  // used for zeroing the statistics
      m_bFirstDatumFound = true;
    }
  }
  bool bMovedIndex = false;
  while ( m_ixLeading < m_Series.Size() ) {
    D *datum = m_Series[ m_ixLeading ];
    m_dtLeading = datum->DateTime();
    if ( &TimeSeriesSlidingWindow<T,D>::Add != &T::Add ) {
      static_cast<T*>( this )->Add( *datum ); // add datum to stats
    }
    
    ++m_ixLeading;
    bMovedIndex = true;
  }
  if ( bMovedIndex ) {
    if ( 0 < m_nWindowSizeCount ) {
      while ( ( m_ixLeading - m_ixTrailing ) > m_nWindowSizeCount ) {
        D *datum = m_Series[ m_ixTrailing ];
        if ( &TimeSeriesSlidingWindow<T,D>::Add != &T::Add ) {
          static_cast<T*>( this )->Expire( *datum );  // expire datum from stats
        }
        ++m_ixTrailing;
      }
    }
    if ( 0 < m_nWindowSizeSeconds ) {
      D *datum = m_Series[ m_ixTrailing ];
      time_duration dif = m_dtLeading - datum->DateTime();
      while ( dif > m_tdWindowWidth ) {
        if ( &TimeSeriesSlidingWindow<T,D>::Add != &T::Add ) {
          static_cast<T*>( this )->Expire( *datum );  // expire datum from stats
        }
        ++m_ixTrailing;
        if ( m_ixTrailing >= m_ixLeading ) {
          break;
        }
        datum = m_Series[ m_ixTrailing ];
        dif = m_dtLeading - datum->DateTime();
      }
    }
  }
  if ( &TimeSeriesSlidingWindow<T,D>::PostUpdate != &T::PostUpdate ) {
    static_cast<T*>( this )->PostUpdate();
  }
}

// ======== QuoteBidAsk

// ======== QuoteMidPoint
/*
// not sure how to use this yet.may not even use it
template<class T> class TimeSeriesSlidingWindowQuoteMidPoint: public TimeSeriesSlidingWindow<T, CQuote>
{
public:
  TimeSeriesSlidingWindowQuoteMidPoint<T>( CTimeSeries<CQuote> *pSeries, long WindowSizeSeconds = 0, size_t WindowSizeCount = 0 )
    : TimeSeriesSlidingWindow<T, Quote>( pSeries, WindowSizeSeconds, WindowSizeCount ) {
  };
  ~TimeSeriesSlidingWindowQuoteMidPoint<T>( void );
protected:
  void Add( const CQuote &datum ) { // CRTP override to process elements passing into window scope
  };
  void Expire( const CQuote &datum ) { // CRTP override to process elements passing out of window scope 
  };  
  void PostUpdate( void ) {};  // CRTPover ride to do final calcs
private:
};
*/
// ======== Trade

} // namespace tf
} // namespace ou

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

// used with TimeSeriesSlidingWindowStats in batch mode on a TimeSeries
// Construct then run Update to process the time series
// Each time timeseries updated, run Update to continue
// useful when timeseries serves multiple windows

#include <TFTimeSeries/TimeSeries.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

template<class T, class D>   //
//class TimeSeriesSlidingWindow: public TimeSeries<D> { // T=CRTP class for Add, Expire, PostUpdate; D=DatedDatum
  // the TimeSeries<D> isn't actually used, could use it, I suppose, but is there in order to recurse additional indicators
class TimeSeriesSlidingWindow { // T=CRTP class for Add, Expire, PostUpdate; D=DatedDatum
public:
  using size_type = typename TimeSeries<D>::size_type;
  TimeSeriesSlidingWindow<T,D>( TimeSeries<D>& Series, time_duration tdWindowWidth, size_type WindowSizeCount = 0 );
  TimeSeriesSlidingWindow<T,D>( TimeSeries<D>& Series, size_t nPeriods, time_duration tdPeriodWidth, size_type WindowSizeCount = 0 );
  TimeSeriesSlidingWindow<T,D>( const TimeSeriesSlidingWindow<T,D>& );  // Delegate is not copied, other values may need some tuning
  virtual ~TimeSeriesSlidingWindow<T,D>();
  virtual void Reset();
  ou::Delegate<const D&> OnAppend;
protected:
  ptime m_dtZero;  // datetime of first element, used as offset
  time_duration WindowWidth() const { return m_tdWindowWidth; };

  void Update( void );

  void Add( const D& datum ) {}; // CRTP override to process elements passing into window scope
  void Expire( const D& datum ) {};  // CRTP override to process elements passing out of window scope
  void PostUpdate() {};  // CRTP override to do final calcs
private:
  TimeSeries<D>& m_Series;
  time_duration m_tdWindowWidth;
  size_type m_nWindowSizeCount;
  size_type m_ixTrailing;  // index to datums to be processed out (expired)
  size_type m_ixLeading;  // index to vector end of datums to be processed in
  ptime m_dtLeading;
  bool m_bFirstDatumFound;
  bool m_bAutoUpdate; // use the OnAppend event to update stuff, else use the Update method to process

  void Init();  // called in constructors
  void HandleDatum( const D& );
};

template<class T, class D>
TimeSeriesSlidingWindow<T,D>::TimeSeriesSlidingWindow(
  TimeSeries<D>& Series, time_duration tdWindowWidth, size_type WindowSizeCount )
: m_Series( Series ), //m_iterTrailing( Series.begin() ),
  m_ixTrailing( 0 ), m_ixLeading( 0 ), m_dtLeading( not_a_date_time ),
  m_tdWindowWidth( tdWindowWidth ), m_nWindowSizeCount( WindowSizeCount ),
  m_bFirstDatumFound( false ), m_bAutoUpdate( true )
{
  assert( seconds( 0 ) <= tdWindowWidth );
  assert( 0 <= WindowSizeCount );
  Init();
}

template<class T, class D>
TimeSeriesSlidingWindow<T,D>::TimeSeriesSlidingWindow(
  TimeSeries<D>& Series, size_t nPeriods, time_duration tdPeriodWidth, size_type WindowSizeCount )
: m_Series( Series ), //m_iterTrailing( Series.begin() ),
  m_ixTrailing( 0 ), m_ixLeading( 0 ), m_dtLeading( not_a_date_time ),
  m_tdWindowWidth( tdPeriodWidth ), m_nWindowSizeCount( WindowSizeCount ),
  m_bFirstDatumFound( false ), m_bAutoUpdate( true )
{
  assert( seconds( 0 ) <= tdPeriodWidth );
  assert( 0 <= WindowSizeCount );
  assert( 0 < nPeriods );
  time_duration tdSum {};
  while ( 0 != nPeriods ) {
    tdSum += tdPeriodWidth;
    nPeriods--;
  }
  m_tdWindowWidth = tdSum;
  Init();
}

template<class T, class D>
TimeSeriesSlidingWindow<T,D>::TimeSeriesSlidingWindow( const TimeSeriesSlidingWindow<T,D>& rhs )
  : m_Series( rhs.m_Series ),
  m_tdWindowWidth( rhs.m_tdWindowWidth ), m_nWindowSizeCount( rhs.m_nWindowSizeCount ),
  m_ixTrailing( rhs.m_ixTrailing ), m_ixLeading( rhs.m_ixLeading ), m_dtLeading( rhs.m_dtLeading ),
  m_bFirstDatumFound( rhs.m_bFirstDatumFound ), m_dtZero( rhs.m_dtZero ), m_bAutoUpdate( true )
{
  // best used when originating timeseries is empty
  Init();
}

template<class T, class D>
TimeSeriesSlidingWindow<T,D>::~TimeSeriesSlidingWindow(void) {
  m_Series.OnAppend.Remove( MakeDelegate( this, &TimeSeriesSlidingWindow<T,D>::HandleDatum ) );
}

template<class T, class D>
void TimeSeriesSlidingWindow<T,D>::Init(void) {
  m_Series.OnAppend.Add( MakeDelegate( this, &TimeSeriesSlidingWindow<T,D>::HandleDatum ) );
}

template<class T, class D>
void TimeSeriesSlidingWindow<T,D>::Reset( void ) {
  m_ixTrailing = m_ixLeading = 0;
  m_dtLeading = not_a_date_time;
}

template<class T, class D>
void TimeSeriesSlidingWindow<T,D>::Update( void ) {
  if ( !m_bFirstDatumFound ) {
    if ( 0 < m_Series.Size() ) {
      m_dtZero = m_Series[ 0 ].DateTime();  // used for zeroing the statistics
      m_bFirstDatumFound = true;
    }
  }
  bool bMovedIndex = false;
  while ( m_ixLeading < m_Series.Size() ) {
    const D& datum( m_Series[ m_ixLeading ] );
    m_dtLeading = datum.DateTime();
    if ( &TimeSeriesSlidingWindow<T,D>::Add != &T::Add ) {
      static_cast<T*>( this )->Add( datum ); // add datum to stats
    }

    ++m_ixLeading;
    bMovedIndex = true;
  }
  if ( bMovedIndex ) {
    if ( 0 < m_nWindowSizeCount ) {
      while ( ( m_ixLeading - m_ixTrailing ) > m_nWindowSizeCount ) {
        const D& datum( m_Series[ m_ixTrailing ] );
        if ( &TimeSeriesSlidingWindow<T,D>::Add != &T::Add ) {
          static_cast<T*>( this )->Expire( datum );  // expire datum from stats
        }
        ++m_ixTrailing;
      }
    }
    if ( 0 < m_tdWindowWidth.total_milliseconds() ) {
      while ( ( m_dtLeading - m_Series[ m_ixTrailing ].DateTime() ) > m_tdWindowWidth ) {
        if ( &TimeSeriesSlidingWindow<T,D>::Add != &T::Add ) {
          static_cast<T*>( this )->Expire( m_Series[ m_ixTrailing ] );  // expire datum from stats
        }
        ++m_ixTrailing;
        if ( m_ixTrailing >= m_ixLeading ) {
          break;
        }
      }
    }
  }
  if ( &TimeSeriesSlidingWindow<T,D>::PostUpdate != &T::PostUpdate ) {
    static_cast<T*>( this )->PostUpdate();
  }
}

template<class T, class D>
void TimeSeriesSlidingWindow<T,D>::HandleDatum( const D& datum ) {
  if ( m_bAutoUpdate ) Update();
  OnAppend( datum );
}

// ======== QuoteBidAsk

// ======== QuoteMidPoint
/*
// not sure how to use this yet.may not even use it
template<class T> class TimeSeriesSlidingWindowQuoteMidPoint: public TimeSeriesSlidingWindow<T, Quote>
{
public:
  TimeSeriesSlidingWindowQuoteMidPoint<T>( TimeSeries<Quote> *pSeries, long WindowSizeSeconds = 0, size_t WindowSizeCount = 0 )
    : TimeSeriesSlidingWindow<T, Quote>( pSeries, WindowSizeSeconds, WindowSizeCount ) {
  };
  ~TimeSeriesSlidingWindowQuoteMidPoint<T>( void );
protected:
  void Add( const Quote &datum ) { // CRTP override to process elements passing into window scope
  };
  void Expire( const Quote &datum ) { // CRTP override to process elements passing out of window scope
  };
  void PostUpdate( void ) {};  // CRTPover ride to do final calcs
private:
};
*/
// ======== Trade

} // namespace tf
} // namespace ou

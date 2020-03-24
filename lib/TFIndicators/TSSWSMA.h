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

#include "TFTimeSeries/TimeSeries.h"
#include "TimeSeriesSlidingWindow.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

template<typename TS> // TS => TimeSeries
class TSSWSMA:
  public TimeSeriesSlidingWindow<TSSWSMA<TS>, typename TS::datum_t>,
  public ou::tf::Prices  // allows daisy chain of indicators
{
  friend TimeSeriesSlidingWindow<TSSWSMA<TS>, typename TS::datum_t>;
public:
  typedef typename TimeSeries<typename TS::datum_t>::size_type size_type;
  TSSWSMA( TS& series, time_duration tdWindowWidth, size_type stWindowSize = 0 );
  TSSWSMA( TS& series, size_t nPeriods, time_duration tdPeriodWidth, size_type stWindowSize = 0 );
  virtual ~TSSWSMA(void);
protected:
  typedef typename TS::datum_t datum_t;
  void Add( const datum_t& datum );
  void Expire( const datum_t& datum );
  void PostUpdate( void );
private:
  size_t m_n;
  ptime m_dt;
  double m_sum;
};

template<typename TS>
TSSWSMA<TS>::TSSWSMA( TS& series, time_duration tdWindowWidth, size_type stWindowSize ):
  TimeSeriesSlidingWindow<TSSWSMA<TS>, datum_t>( series, tdWindowWidth, stWindowSize ),
    m_n {}, m_sum {}
{
}

template<typename TS>
TSSWSMA<TS>::TSSWSMA( TS& series, size_t nPeriods, time_duration tdPeriodWidth, size_type stWindowSize ):
  TimeSeriesSlidingWindow<TSSWSMA<TS>, datum_t>( series, nPeriods, tdPeriodWidth, stWindowSize ),
    m_n {}, m_sum {}
{
}

template<typename TS>
TSSWSMA<TS>::~TSSWSMA(void) {
}

template<typename TS>
void TSSWSMA<TS>::Add( const datum_t& datum ) {
  m_n++;
  m_dt = datum.DateTime();
  m_sum += datum.Value(); // will need to be fixed to be more generic
}

template<typename TS>
void TSSWSMA<TS>::Expire( const datum_t& datum ) {
  m_n--;
  m_sum -= datum.Value(); // will need to be fixed to be more generic
}

template<typename TS>
void TSSWSMA<TS>::PostUpdate( void ) {
  Prices::Append( Price( m_dt, m_sum / m_n ) );
}

// == ou::tf::Option specialization
/* in complete
template<>
class TSSWSMA<ou::tf::Quotes>:
  public TimeSeriesSlidingWindow<TSSWSMA<ou::tf::Quotes>, typename ou::tf::Quotes::datum_t>
{
public:
  TSSWSMA( Quotes& series, size_t nPeriods, time_duration tdPeriodWidth, size_type stWindowSize = 0 );
protected:
  void Add( const ou::tf::Quote& datum );
  void Expire( const ou::tf::Quote& datum );
  void PostUpdate( void );
private:
};

TSSWSMA<ou::tf::Quotes>::TSSWSMA( Quotes& series, size_t nPeriods, time_duration tdPeriodWidth, size_type stWindowSize )
: TimeSeriesSlidingWindow<TSSWSMA<ou::tf::Quotes>, ou::tf::Quote>( series, nPeriods, tdPeriodWidth, stWindowSize )
{

}

void TSSWSMA<ou::tf::Quotes>::Add( const ou::tf::Quote& datum ) {
}

void TSSWSMA<ou::tf::Quotes>::Expire( const ou::tf::Quote& datum ) {
}

void TSSWSMA<ou::tf::Quotes>::PostUpdate() {
}
*/
} // namespace tf
} // namespace ou


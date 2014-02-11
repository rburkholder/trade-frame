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

#include "TimeSeriesSlidingWindow.h"

// need to add something in order to provide discrete intervals, but doesn't use the homogenization process, which will filter out ticks

namespace ou { // One Unified
namespace tf { // TradeFrame

template<typename TS> // TS => TimeSeries
class TSSWTickFrequency: 
  public TimeSeriesSlidingWindow<TSSWTickFrequency<TS>, typename TS::datum_t>,
  public Prices
{
public:
  typedef typename TimeSeries<typename TS::datum_t>::size_type size_type;
  TSSWTickFrequency( TS& series, time_duration tdWindowWidth, size_type stWindowSize = 0 );
  virtual ~TSSWTickFrequency(void);
  ou::Delegate<const typename TS::datum_t&> OnAppend;
protected:
  typedef typename TS::datum_t datum_t;
  void Add( const datum_t& datum );
  void Expire( const datum_t& datum );
  void PostUpdate( void );
private:
  unsigned int m_n;
  ptime m_dt;
};

template<typename TS>
TSSWTickFrequency<TS>::TSSWTickFrequency( TS& series, time_duration tdWindowWidth, size_type stWindowSize ):
  TimeSeriesSlidingWindow<TSSWTickFrequency<TS>, datum_t>( series, tdWindowWidth, stWindowSize ),
    m_n( 0 )
{
}

template<typename TS>
TSSWTickFrequency<TS>::~TSSWTickFrequency(void) {
}

template<typename TS>
void TSSWTickFrequency<TS>::Add( const datum_t& datum ) {
  ++m_n;
  m_dt = datum.DateTime();
}

template<typename TS>
void TSSWTickFrequency<TS>::Expire( const datum_t& datum ) {
  --m_n;
}

template<typename TS>
void TSSWTickFrequency<TS>::PostUpdate( void ) {
  Prices::Append( Price( m_dt, (double) m_n ) );
}

} // namespace tf
} // namespace ou


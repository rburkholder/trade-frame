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

template<typename D> // D => DatedDatum
class TSSWTickFrequency: 
  public TimeSeriesSlidingWindow<TSSWTickFrequency<D>, D>,
  public Prices
{
public:
  typedef typename TimeSeries<D>::size_type size_type;
  TSSWTickFrequency<D>( TimeSeries<D>& series, time_duration tdWindowWidth, size_type stWindowSize = 0 );
  virtual ~TSSWTickFrequency<D>(void);
  ou::Delegate<const D&> OnAppend;
protected:
  void Add( const D& datum );
  void Expire( const D& datum );
  void PostUpdate( void );
private:
  unsigned int m_n;
  ptime m_dt;
};

template<typename D>
TSSWTickFrequency<D>::TSSWTickFrequency( TimeSeries<D>& series, time_duration tdWindowWidth, size_type stWindowSize ):
  TimeSeriesSlidingWindow<TSSWTickFrequency<D>, D>( series, tdWindowWidth, stWindowSize ),
    m_n( 0 )
{
}

template<typename D>
TSSWTickFrequency<D>::~TSSWTickFrequency(void) {
}

template<typename D>
void TSSWTickFrequency<D>::Add( const D& datum ) {
  ++m_n;
  m_dt = datum.DateTime();
}

template<typename D>
void TSSWTickFrequency<D>::Expire( const D& datum ) {
  --m_n;
}

template<typename D>
void TSSWTickFrequency<D>::PostUpdate( void ) {
  Prices::Append( Price( m_dt, (double) m_n ) );
}

} // namespace tf
} // namespace ou


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

#include <vector>

#include <boost/random.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace ou { // One Unified
namespace gp { // genetic programming
namespace rng {  // dependent upon linking OUGP/NodeDouble.cpp
  extern boost::random::mt19937 common;
}

template<typename TS>  // Trades, Quotes, Prices
class TimeSeriesRegistration {
public:
  typedef TS TimeSeries_t;
  typedef typename TS::size_type size_type;
  TimeSeriesRegistration(void);
  ~TimeSeriesRegistration(void);
  void Register( TS* series );
  static void SetTimeSeries( TS** series, size_type ix ) { *series = m_this->m_vTimeSeries[ ix ]; };
protected:
private:
  typedef std::vector<TS*> vTimeSeries_t;
  vTimeSeries_t m_vTimeSeries;
  static TimeSeriesRegistration<TS>* m_this; // used for static reconstruction
};

template<typename TS>
TimeSeriesRegistration<TS>* TimeSeriesRegistration<TS>::m_this;

template<typename TS>
TimeSeriesRegistration<TS>::TimeSeriesRegistration(void) {
  m_this = this;
}

template<typename TS>
TimeSeriesRegistration<TS>::~TimeSeriesRegistration(void) { 
  m_vTimeSeries.clear();
}

template<typename TS>
void TimeSeriesRegistration<TS>::Register( TS* series ) {
  m_vTimeSeries.push_back( series );
}

} // namespace gp
} // namespace ou

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

#include "TimeSeriesForNode.h"

namespace ou { // One Unified
namespace gp { // genetic programming
namespace rng {  // dependent upon linking OUGP/NodeDouble.cpp
  extern boost::random::mt19937 common;
}

template<typename TS>  // CTrade, CQuotes, CPrices
class TimeSeriesRegistration {
public:
  TimeSeriesRegistration(void);
  ~TimeSeriesRegistration(void);

  void Register( TS* series );
protected:
private:
  typedef std::vector<TS*> vTimeSeries_t;
  vTimeSeries_t m_vTimeSeries;
  void HandleTimeSeriesRequest( TimeSeriesForNode<TS>& node );
};

template<typename TS>
TimeSeriesRegistration<TS>::TimeSeriesRegistration(void) {
  TimeSeriesForNode<TS>::Set( MakeDelegate( this, &TimeSeriesRegistration<TS>::HandleTimeSeriesRequest ) );
}

template<typename TS>
TimeSeriesRegistration<TS>::~TimeSeriesRegistration(void) { 
  TimeSeriesForNode<TS>::Clear();
  m_vTimeSeries.clear();
}

template<typename TS>
void TimeSeriesRegistration<TS>::Register( TS* series ) {
  m_vTimeSeries.push_back( series );
}

template<typename TS>
void TimeSeriesRegistration<TS>::HandleTimeSeriesRequest( TimeSeriesForNode<TS>& node ) {
  if ( 0 == m_vTimeSeries.size() ) {
    assert( false );  // need non-zero sized timeseries.  probably logic error somewhere.
  }
  else {
    boost::random::uniform_int_distribution<vTimeSeries_t::size_type> dist( 0, m_vTimeSeries.size() - 1 );  // closed range
    vTimeSeries_t::size_type ix( dist( rng::common ) );
    node.Set( m_vTimeSeries[ ix ] );
  }
}

} // namespace gp
} // namespace ou

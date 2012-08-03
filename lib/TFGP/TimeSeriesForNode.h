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

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

namespace ou { // One Unified
namespace gp { // genetic programming

template<typename TS> // TS TimeSeries
class TimeSeriesForNode {
public:
private:
protected:
  typename TS::size_type m_ixTimeSeries;  // supplied through template parameter
  TS* m_pTimeSeries;  // supplied through Node PreProcessing, after strategy instantiated
public:

  TimeSeriesForNode( void );
  TimeSeriesForNode( const TimeSeriesForNode& rhs );
  ~TimeSeriesForNode( void ) {};

  // doesnt' appear to be used, but included for anyway
  TimeSeriesForNode& operator=( const TimeSeriesForNode& rhs ) {
    if ( &rhs != this ) {
      m_ixTimeSeries = rhs.m_ixTimeSeries;
      m_pTimeSeries = rhs.m_pTimeSeries;
    }
    return *this;
  }

  TS* TimeSeries( void ) const { return m_pTimeSeries; };

};

template<typename TS>
TimeSeriesForNode<TS>::TimeSeriesForNode( void ): m_pTimeSeries( 0 ) {
}

template<typename TS>
TimeSeriesForNode<TS>::TimeSeriesForNode( const TimeSeriesForNode& rhs ) 
  : m_ixTimeSeries( rhs.m_ixTimeSeries ), m_pTimeSeries( rhs.m_pTimeSeries ) { // handles copy constructor to keep existing timeseries
}

} // namespace gp
} // namespace ou

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

  typedef FastDelegate1<TimeSeriesForNode<TS>&> OnNodeTimeSeriesCreatedHandler;

  TimeSeriesForNode( void ): m_pTimeSeries( 0 ) {
    if ( 0 != m_OnNodeTimeSeriesCreated )
      m_OnNodeTimeSeriesCreated( *this ); // obtain a new value for m_pTimeSeries
  };
  TimeSeriesForNode( const TimeSeriesForNode& rhs ): m_pTimeSeries( rhs.m_pTimeSeries ) {  // handles copy constructor to keep existing timeseries
  }
  ~TimeSeriesForNode( void ) {};

  TimeSeriesForNode& operator=( const TimeSeriesForNode& rhs ) {
    if ( &rhs != this ) {
      m_pTimeSeries = rhs.m_pTimeSeries;
    }
    return *this;
  }

  static void Set( OnNodeTimeSeriesCreatedHandler function ) { m_OnNodeTimeSeriesCreated = function; };
  static void Clear( void ) { m_OnNodeTimeSeriesCreated = 0; };

  void Set( TS* pTimeSeries ) { m_pTimeSeries = pTimeSeries; };
  TS* TimeSeries( void ) const { return m_pTimeSeries; };
protected:
  static OnNodeTimeSeriesCreatedHandler m_OnNodeTimeSeriesCreated;
private:
  TS* m_pTimeSeries;

};

} // namespace gp
} // namespace ou

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

#include <boost/thread/tss.hpp>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

namespace ou { // One Unified
namespace gp { // genetic programming

template<typename TS> // TS TimeSeries
class TimeSeriesForNode {
public:
  typedef FastDelegate1<TimeSeriesForNode<TS>&> OnNodeTimeSeriesCreatedHandler;
private:
  static boost::thread_specific_ptr<OnNodeTimeSeriesCreatedHandler> m_tsp;  
protected:
  TS* m_pTimeSeries;
public:

  TimeSeriesForNode( void );
  TimeSeriesForNode( const TimeSeriesForNode& rhs );
  ~TimeSeriesForNode( void ) {};

  TimeSeriesForNode& operator=( const TimeSeriesForNode& rhs ) {
    if ( &rhs != this ) {
      m_pTimeSeries = rhs.m_pTimeSeries;
    }
    return *this;
  }

  static void Set( OnNodeTimeSeriesCreatedHandler function ) { 
    if ( 0 != m_tsp.get() ) {
      assert( false );
    }
    OnNodeTimeSeriesCreatedHandler* pHandler = new OnNodeTimeSeriesCreatedHandler;
    m_tsp.reset( pHandler );
    *pHandler = function;
  };
  static void Clear( void ) { 
    OnNodeTimeSeriesCreatedHandler* pHandler = m_tsp.get();
    *pHandler = 0;
    m_tsp.reset();
  };

  void Set( TS* pTimeSeries ) { m_pTimeSeries = pTimeSeries; };
  TS* TimeSeries( void ) const { return m_pTimeSeries; };

};

template<typename TS>
boost::thread_specific_ptr<typename TimeSeriesForNode<TS>::OnNodeTimeSeriesCreatedHandler> TimeSeriesForNode<TS>::m_tsp;  

template<typename TS>
TimeSeriesForNode<TS>::TimeSeriesForNode( void ): m_pTimeSeries( 0 ) {
  OnNodeTimeSeriesCreatedHandler* pHandler = m_tsp.get();
  if ( 0 != pHandler )
    if ( 0 != *pHandler ) 
      (*pHandler)( *this ); // obtain a new value for m_pTimeSeries
}

template<typename TS>
TimeSeriesForNode<TS>::TimeSeriesForNode( const TimeSeriesForNode<TS>& rhs ): m_pTimeSeries( rhs.m_pTimeSeries ) {  
  // handles copy constructor to keep existing timeseries
}

} // namespace gp
} // namespace ou

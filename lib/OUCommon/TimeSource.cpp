/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include "TimeSource.h"

// use https://github.com/HowardHinnant/date or C++20 chrono
// https://en.wikipedia.org/wiki/Tz_database

namespace ou {

bool TimeSource::m_bTzLoaded( false );
boost::local_time::tz_database TimeSource::m_tzDb;
boost::local_time::time_zone_ptr TimeSource::m_tzChicago;
boost::local_time::time_zone_ptr TimeSource::m_tzNewYork;

TimeSource::TimeSource()
: m_dtLastRetrievedExternalTime( boost::posix_time::microsec_clock::universal_time() )
{
  // http://www.boost.org/doc/libs/1_54_0/doc/html/date_time/examples.html#date_time.examples.local_utc_conversion
  try {
    if ( !m_bTzLoaded ) {
  //    m_tzDb.load_from_file( "../../boost/libs/date_time/data/date_time_zonespec.csv" );
  //    m_tzDb.load_from_file( "..\\..\\boost\\libs\\date_time\\data\\date_time_zonespec.csv" );
      m_tzDb.load_from_file( "../date_time_zonespec.csv" );
      m_tzChicago = m_tzDb.time_zone_from_region( "America/Chicago");
      m_tzNewYork = m_tzDb.time_zone_from_region( "America/New_York");
      m_bTzLoaded = true;
    }
  }
  catch ( std::exception ) {
    // this may not make it to the gui console if this is called prior to gui setup
    std::cout << "TimeSource::TimeSource: can't load date_time_zonespec.csv" << std::endl;
  }
}

boost::local_time::time_zone_ptr TimeSource::LoadTimeZone( const std::string& sRegion ) {
  return m_tzDb.time_zone_from_region( sRegion );
}

boost::posix_time::ptime TimeSource::External( boost::posix_time::ptime* dt ) {
  // this ensures we always have a monotonically increasing time (for use in simulations and time time stamping )
  // TODO:  can this be rewritten without a mutex?  maybe with an atomic increment?
  std::scoped_lock<std::mutex> lock( m_mutex );
  boost::posix_time::ptime& dt_ = *dt;  // create reference to existing location for ease of use
//  dt_ = boost::posix_time::microsec_clock::local_time();
  dt_ = boost::posix_time::microsec_clock::universal_time(); // changed 2013/08/29
  if ( m_dtLastRetrievedExternalTime >= dt_ ) {
    m_dtLastRetrievedExternalTime += boost::posix_time::microsec( 1 );
    dt_ = m_dtLastRetrievedExternalTime;
  }
  else {
    m_dtLastRetrievedExternalTime = dt_;
  }
  return dt_;
}

boost::posix_time::ptime TimeSource::Local() {
  return boost::posix_time::microsec_clock::local_time();
}

TimeSource::SimulationContext* TimeSource::AcquireSimulationContext() {
  return m_contexts.CheckOutL();
}

void TimeSource::ReleaseSimulationContext( SimulationContext* context ) {
  m_contexts.CheckInL( context );
}

void TimeSource::Internal( boost::posix_time::ptime* dt, SimulationContext* context ) {
  *dt = Internal( context );
}

boost::posix_time::ptime TimeSource::Internal( SimulationContext* context ) {
  if ( context->m_bInSimulation )
    return context->m_dtSimulationTime;
  else
    return External();
}

} // ou
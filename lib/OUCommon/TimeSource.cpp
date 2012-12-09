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

#include "stdafx.h"

#include "TimeSource.h"

namespace ou {

TimeSource::TimeSource(void)
: m_dtLastRetrievedExternalTime( boost::posix_time::microsec_clock::local_time() ) {
}

ptime TimeSource::External( ptime* dt ) { 
  // this ensures we always have a monotonically increasing time (for use in simulations)
  boost::mutex::scoped_lock lock( m_mutex );
  ptime& dt_ = *dt;  // create reference to existing location for ease of use
  dt_ = boost::posix_time::microsec_clock::local_time();
  if ( m_dtLastRetrievedExternalTime >= dt_ ) {  
    m_dtLastRetrievedExternalTime += boost::posix_time::microsec( 1 );
    dt_ = m_dtLastRetrievedExternalTime;
  }
  else {
    m_dtLastRetrievedExternalTime = dt_;
  }
  return dt_;
}

TimeSource::SimulationContext* TimeSource::AcquireSimulationContext( void ) {
  return m_contexts.CheckOutL();
}

void TimeSource::ReleaseSimulationContext( SimulationContext* context ) {
  m_contexts.CheckInL( context );
}

void TimeSource::Internal( ptime* dt, SimulationContext* context ) {
  *dt = Internal( context );
}

ptime TimeSource::Internal( SimulationContext* context ) {
  if ( context->m_bInSimulation ) 
    return context->m_dtSimulationTime;
  else 
    return External(); 
}

} // ou
/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "assert.h"

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;
#include <boost/thread/mutex.hpp>

#include "Singleton.h"

namespace ou {

class CTimeSource : public CSingleton<CTimeSource> {
public:

  CTimeSource(void)
  : m_bInSimulation( false ),
    m_dtSimulationTime( boost::date_time::not_a_date_time ),
    m_dtLastRetrievedExternalTime( boost::posix_time::microsec_clock::local_time() ) {};
  ~CTimeSource(void) {};

  ptime External( ptime* dt ) { 
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
  };

  inline ptime External( void ) {
    ptime dt;
    return External( &dt );
  };

  ptime Internal( void ) { 
    if ( m_bInSimulation ) 
      return m_dtSimulationTime;
    else 
      return External(); 
  };

  inline void Internal( ptime* dt ) {
    *dt = Internal();
  }

  void SetSimulationMode( bool bMode = true ) { m_bInSimulation = bMode; m_dtSimulationTime = boost::date_time::not_a_date_time; };
  void ResetSimulationMode( void ) { m_bInSimulation = false; };
  bool GetSimulationMode( void ) { return m_bInSimulation; };
  void SetSimulationTime(const ptime &dt) {
#ifdef _DEBUG
    if ( boost::date_time::not_a_date_time != m_dtSimulationTime ) {
      assert( m_dtSimulationTime <= dt );
    }
#endif
    m_dtSimulationTime = dt; 
  }
  void ForceSimulationTime( const ptime &dt ) { m_bInSimulation = true; m_dtSimulationTime = dt; };

protected:

  bool m_bInSimulation;
  ptime m_dtSimulationTime;
  ptime m_dtLastRetrievedExternalTime;

private:

  boost::mutex m_mutex;
};

} // ou
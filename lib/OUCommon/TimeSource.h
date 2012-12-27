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

#include <assert.h>

#include <vector>

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;
#include <boost/thread/mutex.hpp>

#include "Singleton.h"
#include "ReusableBuffers.h"

namespace ou {

class TimeSource : public Singleton<TimeSource> {
public:

  struct SimulationContext {
    ptime m_dtSimulationTime;
    bool m_bInSimulation;
    SimulationContext( void ) : m_bInSimulation( false ), m_dtSimulationTime( boost::date_time::not_a_date_time ) {};
  };

  TimeSource(void);
  ~TimeSource(void) {};

  ptime External( ptime* dt );

  inline ptime External( void ) {
    ptime dt;
    return External( &dt );
  };

  inline ptime Internal( void ) { 
    return Internal( &m_contextCommon );
  };

  inline void Internal( ptime* dt ) {
    *dt = Internal();
  }

  void Internal( ptime* dt, SimulationContext* );
  ptime Internal( SimulationContext* );

  void SetSimulationMode( bool bMode = true ) { m_contextCommon.m_bInSimulation = bMode; m_contextCommon.m_dtSimulationTime = boost::date_time::not_a_date_time; };
  void ResetSimulationMode( void ) { m_contextCommon.m_bInSimulation = false; };
  bool GetSimulationMode( void ) { return m_contextCommon.m_bInSimulation; };
  void SetSimulationTime(const ptime &dt) {
#ifdef _DEBUG
    if ( boost::date_time::not_a_date_time != m_contextCommon.m_dtSimulationTime ) {
      assert( m_contextCommon.m_dtSimulationTime <= dt );
    }
#endif
    m_contextCommon.m_dtSimulationTime = dt; 
  }
  void ForceSimulationTime( const ptime &dt ) { m_contextCommon.m_bInSimulation = true; m_contextCommon.m_dtSimulationTime = dt; };

  SimulationContext* AcquireSimulationContext( void );
  void ReleaseSimulationContext( SimulationContext* );

protected:
private:

  BufferRepository<SimulationContext> m_contexts;

  SimulationContext m_contextCommon;
  ptime m_dtLastRetrievedExternalTime;

  boost::mutex m_mutex;
};

} // ou
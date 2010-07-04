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


class CTimeSource : boost::noncopyable {
public:

  CTimeSource(void)
  : m_bInSimulation( false ),
    m_dtSimulationTime( boost::date_time::not_a_date_time ),
    m_dtLastRetrievedExternalTime( boost::posix_time::microsec_clock::local_time() ) {};
  ~CTimeSource(void) {};

  void External( ptime* dt ) { 
    // this ensures we always have a monotonically increasing time (for use in simulations)
    // is not thread safe
    ptime& dt_ = *dt;
    dt_ = boost::posix_time::microsec_clock::local_time();
    if ( m_dtLastRetrievedExternalTime >= dt_ ) {  
      m_dtLastRetrievedExternalTime += boost::posix_time::microsec( 1 );
      dt_ = m_dtLastRetrievedExternalTime;
    }
    else {
      m_dtLastRetrievedExternalTime = dt_;
    }
  };

  ptime External( void ) {
    ptime dt;
    External( &dt );
    return dt;
  };

  ptime Internal( void ) { 
    if ( m_bInSimulation ) 
      return m_dtSimulationTime;
    else 
      return External(); 
  };

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
};

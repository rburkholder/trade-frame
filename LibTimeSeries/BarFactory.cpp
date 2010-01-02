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

#include "StdAfx.h"

#include <algorithm>

#include "BarFactory.h"

CBarFactory::CBarFactory(long nSeconds) : 
  m_nBarWidthSeconds( std::max( 1l, nSeconds ) ), m_1Sec( time_duration( 0, 0, 1 ) )
{
}

CBarFactory::~CBarFactory(void) {
  OnNewBarStarted = NULL;
  OnBarUpdated = NULL;
  OnBarComplete = NULL;
}

void CBarFactory::Add(const ptime &dt, double val, unsigned int volume) {
  long seconds = dt.time_of_day().total_seconds();
  long interval = seconds / m_nBarWidthSeconds;
  if ( m_bar.IsNull() ) {
    m_bar.m_dblClose = m_bar.m_dblHigh = m_bar.m_dblLow = m_bar.m_dblOpen = val;
    m_bar.m_nVolume = volume;
    m_curInterval = interval;
    m_bar.m_dt = ptime( dt.date(), time_duration( 0, 0, interval * m_nBarWidthSeconds, 0 ) );
    m_dtLastIntermediateEmission = dt - m_1Sec; // prime the value
    if ( NULL != OnNewBarStarted ) OnNewBarStarted( m_bar );
  }
  else {
    if ( interval > m_curInterval ) { // emit bar and start again
      if ( NULL != OnBarComplete ) OnBarComplete( m_bar );
      m_bar.m_dblClose = m_bar.m_dblHigh = m_bar.m_dblLow = m_bar.m_dblOpen = val;
      m_bar.m_nVolume = volume;
      m_curInterval = interval;
      m_bar.m_dt = ptime( dt.date(), time_duration( 0, 0, interval * m_nBarWidthSeconds, 0 ) );
      if ( NULL != OnNewBarStarted ) OnNewBarStarted( m_bar );
    }
    else { // update current interval
      m_bar.m_dblClose = val;
      m_bar.m_dblHigh = std::max( m_bar.m_dblHigh, val );
      m_bar.m_dblLow = std::min( m_bar.m_dblLow, val );
      m_bar.m_nVolume += volume;

    }
  }
  if ( m_1Sec <= ( dt - m_dtLastIntermediateEmission ) ) {
    if ( NULL != OnBarUpdated ) OnBarUpdated( m_bar );
    m_dtLastIntermediateEmission = dt;
  }
  
}


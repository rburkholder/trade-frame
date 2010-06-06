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

void CBarFactory::Add(const ptime &dt, price_t val, volume_t volume) {
  long seconds = dt.time_of_day().total_seconds();
  long interval = seconds / m_nBarWidthSeconds;
  if ( m_bar.IsNull() ) {
    m_bar.Close( val );
    m_bar.High( val );
    m_bar.Low(  val );
    m_bar.Open( val );
    m_bar.Volume( volume );
    m_curInterval = interval;
    m_bar.DateTime( ptime( dt.date(), time_duration( 0, 0, interval * m_nBarWidthSeconds, 0 ) ) );
    m_dtLastIntermediateEmission = dt - m_1Sec; // prime the value
    if ( NULL != OnNewBarStarted ) OnNewBarStarted( m_bar );
  }
  else {
    if ( interval > m_curInterval ) { // emit bar and start again
      if ( NULL != OnBarComplete ) OnBarComplete( m_bar );
      m_bar.Close( val );
      m_bar.High( val );
      m_bar.Low( val );
      m_bar.Open( val );
      m_bar.Volume( volume );
      m_curInterval = interval;
      m_bar.DateTime( ptime( dt.date(), time_duration( 0, 0, interval * m_nBarWidthSeconds, 0 ) ) );
      if ( NULL != OnNewBarStarted ) OnNewBarStarted( m_bar );
    }
    else { // update current interval
      m_bar.Close( val );
      m_bar.High( std::max( m_bar.High(), val ) );
      m_bar.Low( std::min( m_bar.Low(), val ) );
      m_bar.Volume( m_bar.Volume() + volume ); 

    }
  }
  if ( m_1Sec <= ( dt - m_dtLastIntermediateEmission ) ) {
    if ( NULL != OnBarUpdated ) OnBarUpdated( m_bar );
    m_dtLastIntermediateEmission = dt;
  }
  
}


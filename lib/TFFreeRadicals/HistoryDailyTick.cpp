/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

//#include "stdafx.h"

#include "InstrumentState.h"
#include "HistoryDailyTick.h"

// code sliced out of OverUnderConsole::App
// code designed to retrieve a day's worth of tick data from iqfeed

HistoryDailyTick::HistoryDailyTick(void) {
}

HistoryDailyTick::~HistoryDailyTick(void) {
}

void HistoryDailyTick::OnHistoryConnected( void ) {
  InstrumentState& is( m_md.data );
  is.dblOpen = is.dblHigh = is.dblLow = is.dblClose = 0.0;
  ptime dtStart = m_pInstrument->GetTimeTrading().begin();
  ptime dtEnd = m_pInstrument->GetTimeTrading().end();
  if ( 0 == dtStart.date().day_of_week() ) {
    // crosses weekend
    RetrieveDatedRangeOfDataPoints(       
      m_pInstrument->GetInstrumentName( m_piqfeed->ID() ), dtStart - date_duration( 3 ), dtEnd - date_duration( 3 ) );
  }
  else {
    // regular day
    RetrieveDatedRangeOfDataPoints( 
      m_pInstrument->GetInstrumentName( m_piqfeed->ID() ), dtStart - date_duration( 1 ), dtEnd - date_duration( 1 ) );
  }
}

void HistoryDailyTick::OnHistoryDisconnected( void ) {
}

void HistoryDailyTick::OnHistoryTickDataPoint( structTickDataPoint* pDP ) {
  InstrumentState& is( m_md.data );
  // calculate open, high, low, close for period (equiv to end of day)
  if ( 0 == is.dblOpen ) {
    is.dblOpen = is.dblHigh = is.dblLow = is.dblClose = pDP->Last;
  }
  else {
    if ( pDP->Last > is.dblHigh ) is.dblHigh = pDP->Last;
    if ( pDP->Last < is.dblLow ) is.dblLow = pDP->Last;
    is.dblClose = pDP->Last;
  }
  is.history.Append( ou::tf::CTrade( pDP->DateTime, pDP->Last, pDP->LastSize ) );

}

void HistoryDailyTick::OnHistoryRequestDone( void ) {
  InstrumentState& is( m_md.data );
  std::cout << "History complete" << std::endl;
  StartStateMachine();
}


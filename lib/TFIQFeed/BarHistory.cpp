/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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

/*
 * File:    BarHistory.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFIQFeed
 * Created: August 7, 2021, 19:06
 */

 //#include <iostream>  // for test use, remove for production

 #include "BarHistory.h"

namespace ou {
namespace tf {
namespace iqfeed {

BarHistory::BarHistory(
  fConnected_t&& fConnected, fBar_t&& fBar, fDone_t&& fDone
)
: ou::tf::iqfeed::HistoryQuery<BarHistory>(),
  m_fConnected( std::move( fConnected ) ),
  m_fBar( std::move( fBar ) ),
  m_fDone( std::move( fDone ) )
{
  assert( m_fConnected );
  assert( m_fBar );
  assert( m_fDone );
}

void BarHistory::Set( fBar_t&& fBar, fDone_t&& fDone ) {
  assert( fBar );
  assert( fDone );
  m_fBar = std::move( fBar );
  m_fDone = std::move( fDone );
}

void BarHistory::Connect() {
  ou::tf::iqfeed::HistoryQuery<BarHistory>::Connect();
}

void BarHistory::RequestNBars( const std::string& sSymbol, unsigned int nSeconds, unsigned int nBars ) {
  RetrieveNIntervals( sSymbol, nSeconds, nBars );
}

void BarHistory::RequestNDaysOfBars( const std::string& sSymbol, unsigned int nSeconds, unsigned int nDays ) {
  RetrieveNDaysOfIntervals( sSymbol, nSeconds, nDays );
}

void BarHistory::RequestNEndOfDay( const std::string& sSymbol, unsigned int nDays ) {
  RetrieveNEndOfDays( sSymbol, nDays );
}

void BarHistory::OnHistoryConnected() {
  //std::cout << "OnHistoryConnected" << std::endl;
  m_fConnected();
};

void BarHistory::OnHistorySendDone() {
  //std::cout << "OnHistorySendDone" << std::endl;
  m_fDone();
}

void BarHistory::OnHistoryIntervalData( Interval* pDP ) {
  ou::tf::Bar bar( pDP->DateTime, pDP->Open, pDP->High, pDP->Low, pDP->Close, pDP->PeriodVolume );
  m_fBar( bar );
  ou::tf::iqfeed::HistoryQuery<BarHistory>::OnHistoryIntervalData( pDP );
}

void BarHistory::OnHistoryEndOfDayData( EndOfDay* pDP ) {
  //std::cout << "OnHistoryEndOfDayData: " << pDP->Close << std::endl;
  ou::tf::Bar bar( pDP->DateTime, pDP->Open, pDP->High, pDP->Low, pDP->Close, pDP->PeriodVolume );
  m_fBar( bar );
  ou::tf::iqfeed::HistoryQuery<BarHistory>::OnHistoryEndOfDayData( pDP );
};

void BarHistory::OnHistoryRequestDone() {
  //std::cout << "OnHistoryRequestDone" << std::endl;
  m_fDone();
};

void BarHistory::OnHistoryError( size_t e ) {
  std::cout << "BarHistory::OnHistoryError: " << e << std::endl;
};

void BarHistory::Disconnect() {
  ou::tf::iqfeed::HistoryQuery<BarHistory>::Disconnect();
}

} // namespace iqfeed
} // namespace tf
} // namespace ou

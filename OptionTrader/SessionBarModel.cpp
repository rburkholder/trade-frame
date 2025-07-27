/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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

/*
 * File:    SessionBarModel.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 25, 2025 11:17:40
 */

// Inspiration: IndicatorTrading/SessionChart.cpp

#include <OUCommon/TimeSource.h>

#include "SessionBarModel.hpp"

// TODO: add pivots

SessionBarModel::SessionBarModel()
: m_bWatchStarted( false )
, m_bfPrice1Minute( 60 )
{
  m_cePriceBars.SetName( "Trades" );

  m_bfPrice1Minute.SetOnBarComplete( MakeDelegate( this, &SessionBarModel::HandleBarCompletionPrice ) );
}

SessionBarModel::~SessionBarModel() {
  StopWatch();
}

void SessionBarModel::Set( pWatch_t& pWatch ) {

  assert( pWatch );
  assert( !m_pWatch );

  m_pWatch = pWatch;

}

void SessionBarModel::Set( ou::ChartEntryMark& cem2, ou::ChartEntryMark& cem1 ) {

  m_dvChart.Add( EChartSlot::Price, &cem1 );
  m_dvChart.Add( EChartSlot::Price, &cem2 );
  m_dvChart.Add( EChartSlot::Price, &m_cePriceBars );
  m_dvChart.Add( EChartSlot::Volume, &m_ceVolume );

  //const std::string& sIQFeedName = pWatch->GetInstrument()->GetInstrumentName( ou::tf::keytypes::EProviderIQF );
}

void SessionBarModel::OnHistoryBar( const ou::tf::Bar& bar_ ) {
  ptime dtUtc = ou::TimeSource::ConvertEasternToUtc( bar_.DateTime() );
  ou::tf::Bar bar( dtUtc, bar_.Open(), bar_.High(), bar_.Low(), bar_.Close(), bar_.Volume( ) );
  //std::cout << "bar close " << bar.Close() << "@" << dtUtc << std::endl;
  HandleBarCompletionPrice( bar );
}

void SessionBarModel::OnHistoryDone() {
  StartWatch();
}

void SessionBarModel::HandleTrade( const ou::tf::Trade& trade ) {
  m_bfPrice1Minute.Add( trade );
}

void SessionBarModel::HandleBarCompletionPrice( const ou::tf::Bar& bar ) {
  m_cePriceBars.AppendBar( bar );
  m_ceVolume.Append( bar );
}

void SessionBarModel::StartWatch() {
  m_bWatchStarted = true;
  m_pWatch->OnTrade.Add( MakeDelegate( this, &SessionBarModel::HandleTrade ) );
  m_pWatch->StartWatch();
}

void SessionBarModel::StopWatch() {
  if ( m_bWatchStarted ) {
    m_bWatchStarted = false;
    m_pWatch->StopWatch();
    m_pWatch->OnTrade.Remove( MakeDelegate( this, &SessionBarModel::HandleTrade ) );
  }
  m_pWatch.reset();
}

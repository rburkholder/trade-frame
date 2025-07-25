/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    SessionChart.cpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: May 16, 2022 20:03
 */

#include <OUCommon/TimeSource.h>

#include "SessionChart.hpp"

// TODO: add pivots

SessionChart::SessionChart()
: ou::tf::WinChartView()
, m_bWatchStarted( false )
, m_bfPrice1Minute( 60 )
{}

SessionChart::SessionChart(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
)
: ou::tf::WinChartView( parent, id, pos, size, style )
, m_bWatchStarted( false )
, m_bfPrice1Minute( 60 )
{
  Init();
}

bool SessionChart::Create(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
) {
  bool bOk = WinChartView::Create( parent, id, pos, size, style );
  Init();
  return bOk;
}

SessionChart::~SessionChart() {
}

void SessionChart::Init() {

  m_cePriceBars.SetName( "Trades" );

  m_bfPrice1Minute.SetOnBarComplete( MakeDelegate( this, &SessionChart::HandleBarCompletionPrice ) );

  BindEvents();

  SetChartDataView( &m_dvChart );

}

void SessionChart::SetPosition( pPosition_t pPosition, ou::ChartEntryMark& cem ) {

  m_dvChart.Add( EChartSlot::Price, &cem );

  m_dvChart.Add( EChartSlot::Price, &m_cePriceBars );
  m_dvChart.Add( EChartSlot::Volume, &m_ceVolume );

  m_pPosition = pPosition;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  pWatch_t pWatch = m_pPosition->GetWatch();
  const std::string& sIQFeedName = pWatch->GetInstrument()->GetInstrumentName( ou::tf::keytypes::EProviderIQF );
  m_pBarHistory = std::make_unique<ou::tf::iqfeed::BarHistory>(
    [this,sIQFeedName](){ // m_fConnected
      m_pBarHistory->RequestNDaysOfBars( sIQFeedName, 60,  1 );
    },
    [this](const ou::tf::Bar& bar_ ){ // m_fBar
      //m_barsSessionHistory.Append( bar );
      ptime dtUtc = ou::TimeSource::ConvertEasternToUtc( bar_.DateTime() );
      ou::tf::Bar bar( dtUtc, bar_.Open(), bar_.High(), bar_.Low(), bar_.Close(), bar_.Volume( ) );
      //std::cout << "bar close " << bar.Close() << "@" << dtUtc << std::endl;
      HandleBarCompletionPrice( bar );
    },
    [this](){  // m_fDone
      m_bWatchStarted = true;
      using pWatch_t = ou::tf::Watch::pWatch_t;
      pWatch_t pWatch = m_pPosition->GetWatch();
      pWatch->OnTrade.Add( MakeDelegate( this, &SessionChart::HandleTrade ) );
    }
  );
  m_pBarHistory->Connect();
}

void SessionChart::HandleTrade( const ou::tf::Trade& trade ) {
  m_bfPrice1Minute.Add( trade );
}

void SessionChart::HandleBarCompletionPrice( const ou::tf::Bar& bar ) {
  m_cePriceBars.AppendBar( bar );
  m_ceVolume.Append( bar );
}

void SessionChart::BindEvents() {
  Bind( wxEVT_DESTROY, &SessionChart::OnDestroy, this );
  //Bind( wxEVT_KEY_UP, &SessionChart::OnKey, this );
  //Bind( wxEVT_CHAR, &SessionChart::OnChar, this );
}

void SessionChart::UnBindEvents() {
  Unbind( wxEVT_DESTROY, &SessionChart::OnDestroy, this );
  //Unbind( wxEVT_KEY_UP, &SessionChart::OnKey, this );
  //Unbind( wxEVT_CHAR, &SessionChart::OnChar, this );
}

void SessionChart::OnDestroy( wxWindowDestroyEvent& event ) {
  if ( m_bWatchStarted ) {
    m_bWatchStarted = false;
    using pWatch_t = ou::tf::Watch::pWatch_t;
    pWatch_t pWatch = m_pPosition->GetWatch();
    pWatch->OnTrade.Remove( MakeDelegate( this, &SessionChart::HandleTrade ) );
  }
  SetChartDataView( nullptr );
  UnBindEvents();
  event.Skip();
}

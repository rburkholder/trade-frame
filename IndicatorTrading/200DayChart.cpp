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
 * File:    200DayChart.cpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: May 26, 2022 21:55
 */

#include <TFTimeSeries/TimeSeries.h>

#include "200DayChart.hpp"

Chart200Day::Chart200Day()
: ou::tf::WinChartView()
{}

Chart200Day::Chart200Day(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
)
: ou::tf::WinChartView( parent, id, pos, size, style )
{
  Init();
}

bool Chart200Day::Create(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
) {
  bool bOk = WinChartView::Create( parent, id, pos, size, style );
  Init();
  return bOk;
}

Chart200Day::~Chart200Day() {
}

void Chart200Day::Init() {

  m_cePriceBars.SetName( "Daily" );

  BindEvents();

  m_dvChart.Add( EChartSlot::Price, &m_cePriceBars );
  m_dvChart.Add( EChartSlot::Volume, &m_ceVolume );

  SetChartDataView( &m_dvChart );

}

void Chart200Day::Add( const ou::tf::Bars& bars ) {
  bars.ForEach( [this]( const ou::tf::Bar& bar ){
    m_cePriceBars.AppendBar( bar );
    m_ceVolume.Append( bar );
  } );
}

void Chart200Day::BindEvents() {
  Bind( wxEVT_DESTROY, &Chart200Day::OnDestroy, this );
  Bind( wxEVT_SIZE, &Chart200Day::HandleOnResize, this );
  //Bind( wxEVT_KEY_UP, &SessionChart::OnKey, this );
  //Bind( wxEVT_CHAR, &SessionChart::OnChar, this );
}

void Chart200Day::HandleOnResize( wxSizeEvent& event ) {
  DrawChart();
  event.Skip();
}

void Chart200Day::UnBindEvents() {
  Unbind( wxEVT_DESTROY, &Chart200Day::OnDestroy, this );
  Unbind( wxEVT_SIZE, &Chart200Day::HandleOnResize, this );
  //Unbind( wxEVT_KEY_UP, &SessionChart::OnKey, this );
  //Unbind( wxEVT_CHAR, &SessionChart::OnChar, this );
}

void Chart200Day::OnDestroy( wxWindowDestroyEvent& event ) {
  SetChartDataView( nullptr );
  UnBindEvents();
  event.Skip( true );
}

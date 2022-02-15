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
 * File:    ActivityChart.cpp
 * Author:  raymond@burkholder.net
 * Project: AutoTrade
 * Created: February 14, 2022 11:01
 */

#include <TFIndicators/TSSWStats.h>

#include "ActivityChart.h"

ActivityChart::ActivityChart()
: WinChartView::WinChartView()
{
  Init();
}

ActivityChart::ActivityChart(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
  )
: WinChartView::WinChartView( parent, id, pos, size, style )
{
  Init();
}

bool ActivityChart::Create(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style )
{
  bool bOk = WinChartView::Create( parent, id, pos, size, style );
  Init();
  return bOk;
}

ActivityChart::~ActivityChart() {
}

void ActivityChart::Init() {

  SetChartDataView( &m_dvChart );
}

void ActivityChart::Connect() {

  if ( m_pPosition ) {
    if ( !m_bConnected ) {
      pWatch_t pWatch = m_pPosition->GetWatch();
      m_pIndicatorStochastic1 = std::make_shared<ou::tf::TSSWStochastic>(
        pWatch->GetQuotes(), 14, time_duration( 0, 0, 20 ),
        [this]( const ou::tf::Price& price ){
          m_ceStochastic1.Append( price );
        }
      );
      m_pIndicatorStochastic2 = std::make_shared<ou::tf::TSSWStochastic>(
        pWatch->GetQuotes(), 14, time_duration( 0, 0, 60 ),
        [this]( const ou::tf::Price& price ){
          m_ceStochastic2.Append( price );
        }
      );
      m_pIndicatorStochastic3 = std::make_shared<ou::tf::TSSWStochastic>(
        pWatch->GetQuotes(), 14, time_duration( 0, 0, 180 ),
        [this]( const ou::tf::Price& price ){
          m_ceStochastic3.Append( price );
        }
      );
      m_bConnected = true;
      pWatch->OnQuote.Add( MakeDelegate( this, &InteractiveChart::HandleQuote ) );
      pWatch->OnTrade.Add( MakeDelegate( this, &InteractiveChart::HandleTrade ) );
    }
  }

}

void ActivityChart::Disconnect() { // TODO: may also need to clear indicators
  if ( m_pPosition ) {
    if ( m_bConnected ) {
      m_pIndicatorStochastic1.reset();
      m_pIndicatorStochastic2.reset();
      m_pIndicatorStochastic3.reset();
      pWatch_t pWatch = m_pPosition->GetWatch();
      m_bConnected = false;
      pWatch->OnQuote.Remove( MakeDelegate( this, &InteractiveChart::HandleQuote ) );
      pWatch->OnTrade.Remove( MakeDelegate( this, &InteractiveChart::HandleTrade ) );
    }
  }
}


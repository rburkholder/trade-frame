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
 * File:    Chart.cpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: February 8, 2022 13:38
 */

#include <memory>

#include "InteractiveChart.h"
#include "TFTrading/Watch.h"

InteractiveChart::InteractiveChart()
: WinChartView::WinChartView()
, m_bConnected( false )
, m_bfTrades( 10 )
, m_ceShortEntries( ou::ChartEntryShape::EShort, ou::Colour::Red )
, m_ceLongEntries( ou::ChartEntryShape::ELong, ou::Colour::Blue )
, m_ceShortFills( ou::ChartEntryShape::EFillShort, ou::Colour::Red )
, m_ceLongFills( ou::ChartEntryShape::EFillLong, ou::Colour::Blue )
, m_ceShortExits( ou::ChartEntryShape::EShortStop, ou::Colour::Red )
, m_ceLongExits( ou::ChartEntryShape::ELongStop, ou::Colour::Blue )
{
  Init();
}

InteractiveChart::InteractiveChart(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
  )
: WinChartView::WinChartView( parent, id, pos, size, style )
, m_bConnected( false )
, m_bfTrades( 10 )
, m_ceShortEntries( ou::ChartEntryShape::EShort, ou::Colour::Red )
, m_ceLongEntries( ou::ChartEntryShape::ELong, ou::Colour::Blue )
, m_ceShortFills( ou::ChartEntryShape::EFillShort, ou::Colour::Red )
, m_ceLongFills( ou::ChartEntryShape::EFillLong, ou::Colour::Blue )
, m_ceShortExits( ou::ChartEntryShape::EShortStop, ou::Colour::Red )
, m_ceLongExits( ou::ChartEntryShape::ELongStop, ou::Colour::Blue )
{
  Init();
}

bool InteractiveChart::Create(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style )
{
  bool bOk = WinChartView::Create( parent, id, pos, size, style );
  Init();
  return bOk;
}

InteractiveChart::~InteractiveChart() {
  m_bfTrades.SetOnBarComplete( nullptr );
}

void InteractiveChart::Init() {
  m_dvChart.Add( 0, &m_ceTrade );
  m_dvChart.Add( 0, &m_ceQuoteAsk );
  m_dvChart.Add( 0, &m_ceQuoteBid );

  m_dvChart.Add( 2, &m_ceQuoteSpread );

  m_dvChart.Add( 0, &m_ceShortEntries );
  m_dvChart.Add( 0, &m_ceLongEntries );
  m_dvChart.Add( 0, &m_ceShortFills );
  m_dvChart.Add( 0, &m_ceLongFills );
  m_dvChart.Add( 0, &m_ceShortExits );
  m_dvChart.Add( 0, &m_ceLongExits );

  m_bfTrades.SetOnBarComplete( MakeDelegate( this, &InteractiveChart::HandleBarCompletionTrades ) );

  m_dvChart.Add( 0, &m_ceBars );

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );
  m_ceQuoteSpread.SetColour( ou::Colour::Black );

  m_ceQuoteSpread.SetName( "Spread" );

  SetChartDataView( &m_dvChart );
}

void InteractiveChart::Disconnect() { // TODO: may also need to clear indicators
  if ( m_pPosition ) {
    if ( m_bConnected ) {
      m_bConnected = false;
      pWatch_t pWatch = m_pPosition->GetWatch();
      pWatch->OnQuote.Remove( MakeDelegate( this, &InteractiveChart::HandleQuote ) );
      pWatch->OnTrade.Remove( MakeDelegate( this, &InteractiveChart::HandleTrade ) );
    }
  }
}

void InteractiveChart::SetPosition( pPosition_t pPosition ) {

  m_pPosition = pPosition;
  pWatch_t pWatch = m_pPosition->GetWatch();

  Disconnect();

  m_bConnected = true;
  pWatch->OnQuote.Add( MakeDelegate( this, &InteractiveChart::HandleQuote ) );
  pWatch->OnTrade.Add( MakeDelegate( this, &InteractiveChart::HandleTrade ) );
}

void InteractiveChart::HandleQuote( const ou::tf::Quote& quote ) {

  if ( !quote.IsValid() ) {
    return;
  }

  ptime dt( quote.DateTime() );

  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );
  m_ceQuoteSpread.Append( dt, quote.Ask() - quote.Bid() );

}

void InteractiveChart::HandleTrade( const ou::tf::Trade& trade ) {

  ptime dt( trade.DateTime() );
  ou::tf::Trade::price_t price = trade.Price();

  m_ceTrade.Append( dt, price );
}

void InteractiveChart::HandleBarCompletionTrades( const ou::tf::Bar& bar ) {
  m_ceBars.AppendBar( bar );
}


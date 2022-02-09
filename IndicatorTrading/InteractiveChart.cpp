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

namespace {
  static const size_t nBarSeconds = 3;
}

InteractiveChart::InteractiveChart()
: WinChartView::WinChartView()
, m_bConnected( false )
, m_bfPrice( nBarSeconds )
, m_bfPriceUp( nBarSeconds )
, m_bfPriceDn( nBarSeconds )
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
, m_bfPrice( nBarSeconds )
, m_bfPriceUp( nBarSeconds )
, m_bfPriceDn( nBarSeconds )
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
  m_bfPrice.SetOnBarComplete( nullptr );
}

void InteractiveChart::Init() {
  m_dvChart.Add( 0, &m_ceTrade );
  m_dvChart.Add( 0, &m_ceQuoteAsk );
  m_dvChart.Add( 0, &m_ceQuoteBid );

  m_dvChart.Add( 0, &m_cePriceBars );

  m_dvChart.Add( 0, &m_ceShortEntries );
  m_dvChart.Add( 0, &m_ceLongEntries );
  m_dvChart.Add( 0, &m_ceShortFills );
  m_dvChart.Add( 0, &m_ceLongFills );
  m_dvChart.Add( 0, &m_ceShortExits );
  m_dvChart.Add( 0, &m_ceLongExits );

  m_dvChart.Add( 1, &m_ceVolume );
  //m_dvChart.Add( 1, &m_ceVolumeUp );
  //m_dvChart.Add( 1, &m_ceVolumeDn );

  m_dvChart.Add( 2, &m_ceQuoteSpread );

  m_bfPrice.SetOnBarComplete( MakeDelegate( this, &InteractiveChart::HandleBarCompletionPrice ) );
  m_bfPriceUp.SetOnBarComplete( MakeDelegate( this, &InteractiveChart::HandleBarCompletionPriceUp ) );
  m_bfPriceDn.SetOnBarComplete( MakeDelegate( this, &InteractiveChart::HandleBarCompletionPriceDn ) );

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );
  m_ceQuoteSpread.SetColour( ou::Colour::Black );

  m_ceQuoteSpread.SetName( "Spread" );

  m_ceVolumeUp.SetColour( ou::Colour::Green );
  m_ceVolumeDn.SetColour( ou::Colour::Red );

  SetChartDataView( &m_dvChart );
}

void InteractiveChart::Connect() {

  if ( m_pPosition ) {
    if ( !m_bConnected ) {
      pWatch_t pWatch = m_pPosition->GetWatch();
      m_bConnected = true;
      pWatch->OnQuote.Add( MakeDelegate( this, &InteractiveChart::HandleQuote ) );
      pWatch->OnTrade.Add( MakeDelegate( this, &InteractiveChart::HandleTrade ) );
    }
  }

}

void InteractiveChart::Disconnect() { // TODO: may also need to clear indicators
  if ( m_pPosition ) {
    if ( m_bConnected ) {
      pWatch_t pWatch = m_pPosition->GetWatch();
      m_bConnected = false;
      pWatch->OnQuote.Remove( MakeDelegate( this, &InteractiveChart::HandleQuote ) );
      pWatch->OnTrade.Remove( MakeDelegate( this, &InteractiveChart::HandleTrade ) );
    }
  }
}

void InteractiveChart::SetPosition( pPosition_t pPosition ) {

  bool bConnected = m_bConnected;
  Disconnect();
  m_pPosition = pPosition;
  if ( bConnected ) {
    Connect();
  }

}

void InteractiveChart::HandleQuote( const ou::tf::Quote& quote ) {

  if ( !quote.IsValid() ) {
    return;
  }

  ptime dt( quote.DateTime() );

  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );
  m_ceQuoteSpread.Append( dt, quote.Ask() - quote.Bid() );

  m_quote = quote;

}

void InteractiveChart::HandleTrade( const ou::tf::Trade& trade ) {

  ptime dt( trade.DateTime() );
  ou::tf::Trade::price_t price = trade.Price();

  m_ceTrade.Append( dt, price );

  double mid = m_quote.Midpoint();

  m_bfPrice.Add( dt, price, trade.Volume() );
  //if ( price >= mid ) {
  //  m_bfPriceUp.Add( dt, price, trade.Volume() );
  //}
  //else {
  //  m_bfPriceDn.Add( dt, price, -trade.Volume() );
  //}
}

void InteractiveChart::HandleBarCompletionPrice( const ou::tf::Bar& bar ) {
  m_ceVolume.Append( bar.DateTime(), bar.Volume() );
}

void InteractiveChart::HandleBarCompletionPriceUp( const ou::tf::Bar& bar ) {
  m_ceVolumeUp.Append( bar.DateTime(), bar.Volume() );
}

void InteractiveChart::HandleBarCompletionPriceDn( const ou::tf::Bar& bar ) {
  m_ceVolumeDn.Append( bar.DateTime(), bar.Volume() );
}


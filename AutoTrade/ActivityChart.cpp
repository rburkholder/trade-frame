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
, m_ceShortEntries( ou::ChartEntryShape::EShort, ou::Colour::Red )
, m_ceLongEntries( ou::ChartEntryShape::ELong, ou::Colour::Blue )
, m_ceShortFills( ou::ChartEntryShape::EFillShort, ou::Colour::Red )
, m_ceLongFills( ou::ChartEntryShape::EFillLong, ou::Colour::Blue )
, m_ceShortExits( ou::ChartEntryShape::EShortStop, ou::Colour::Red )
, m_ceLongExits( ou::ChartEntryShape::ELongStop, ou::Colour::Blue )
{
  Init();
}

ActivityChart::ActivityChart(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
  )
: WinChartView::WinChartView( parent, id, pos, size, style )
, m_ceShortEntries( ou::ChartEntryShape::EShort, ou::Colour::Red )
, m_ceLongEntries( ou::ChartEntryShape::ELong, ou::Colour::Blue )
, m_ceShortFills( ou::ChartEntryShape::EFillShort, ou::Colour::Red )
, m_ceLongFills( ou::ChartEntryShape::EFillLong, ou::Colour::Blue )
, m_ceShortExits( ou::ChartEntryShape::EShortStop, ou::Colour::Red )
, m_ceLongExits( ou::ChartEntryShape::ELongStop, ou::Colour::Blue )
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
  m_dvChart.Add( 0, &m_ceQuoteAsk );
  m_dvChart.Add( 0, &m_ceTrade );
  m_dvChart.Add( 0, &m_ceQuoteBid );

  m_dvChart.Add( 0, &m_cePriceBars );

  m_dvChart.Add( 0, &m_ceShortEntries );
  m_dvChart.Add( 0, &m_ceLongEntries );
  m_dvChart.Add( 0, &m_ceShortFills );
  m_dvChart.Add( 0, &m_ceLongFills );
  m_dvChart.Add( 0, &m_ceShortExits );
  m_dvChart.Add( 0, &m_ceLongExits );

  m_dvChart.Add( 1, &m_ceVolume );

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );

  m_ceQuoteAsk.SetName( "Ask" );
  m_ceTrade.SetName( "Tick" );
  m_ceQuoteBid.SetName( "Bid" );

  m_ceVolume.SetName( "Volume" );

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

void ActivityChart::HandleQuote( const ou::tf::Quote& quote ) {

  if ( !quote.IsValid() ) {
    return;
  }

  ptime dt( quote.DateTime() );

  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );
  m_ceQuoteSpread.Append( dt, quote.Ask() - quote.Bid() );

  m_quote = quote;

}

void ActivityChart::HandleTrade( const ou::tf::Trade& trade ) {

  ptime dt( trade.DateTime() );
  ou::tf::Trade::price_t price = trade.Price();

  m_ceTrade.Append( dt, price );

  double mid = m_quote.Midpoint();

  //m_bfPrice.Add( dt, price, trade.Volume() );
  if ( price >= mid ) {
    m_bfPriceUp.Add( dt, price, trade.Volume() );
  }
  else {
    m_bfPriceDn.Add( dt, price, -trade.Volume() );
  }
}


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
 * File:    Strategy.cpp
 * Author:  raymond@burkholder.net
 * Project: AutoTrade
 * Created: February 14, 2022 10:59
 */

 // https://learnpriceaction.com/3-moving-average-crossover-strategy/

#include <TFTrading/Watch.h>

#include "Config.h"
#include "OUCommon/Colour.h"
#include "Strategy.h"

using pWatch_t = ou::tf::Watch::pWatch_t;

Strategy::Strategy( ou::ChartDataView& cdv, const config::Options& options )
: ou::tf::DailyTradeTimeFrame<Strategy>()
, m_cdv( cdv )
, m_ceShortEntries( ou::ChartEntryShape::EShort, ou::Colour::Red )
, m_ceLongEntries( ou::ChartEntryShape::ELong, ou::Colour::Blue )
, m_ceShortFills( ou::ChartEntryShape::EFillShort, ou::Colour::Red )
, m_ceLongFills( ou::ChartEntryShape::EFillLong, ou::Colour::Blue )
, m_ceShortExits( ou::ChartEntryShape::EShortStop, ou::Colour::Red )
, m_ceLongExits( ou::ChartEntryShape::ELongStop, ou::Colour::Blue )
{

  assert( 0 < options.nPeriodWidth );

  m_nPeriodWidth = options.nPeriodWidth;
  m_vMAPeriods.push_back( options.nMA1Periods );
  m_vMAPeriods.push_back( options.nMA2Periods );
  m_vMAPeriods.push_back( options.nMA3Periods );

  assert( 3 == m_vMAPeriods.size() );
  for ( vMAPeriods_t::value_type value: m_vMAPeriods ) {
    assert( 0 < value );
  }

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );

  m_ceQuoteAsk.SetName( "Ask" );
  m_ceTrade.SetName( "Tick" );
  m_ceQuoteBid.SetName( "Bid" );

  m_ceVolume.SetName( "Volume" );

}

Strategy::~Strategy() {
  Clear();
}

void Strategy::SetupChart() {

  m_cdv.Add( EChartSlot::Price, &m_ceQuoteAsk );
  m_cdv.Add( EChartSlot::Price, &m_ceTrade );
  m_cdv.Add( EChartSlot::Price, &m_ceQuoteBid );

  m_cdv.Add( EChartSlot::Volume, &m_ceVolume );

  m_cdv.Add( EChartSlot::Price, &m_ceShortEntries );
  m_cdv.Add( EChartSlot::Price, &m_ceLongEntries );
  m_cdv.Add( EChartSlot::Price, &m_ceShortFills );
  m_cdv.Add( EChartSlot::Price, &m_ceLongFills );
  m_cdv.Add( EChartSlot::Price, &m_ceShortExits );
  m_cdv.Add( EChartSlot::Price, &m_ceLongExits );

}

void Strategy::SetPosition( pPosition_t pPosition ) {

  assert( pPosition );

  Clear();

  m_pPosition = pPosition;
  pWatch_t pWatch = m_pPosition->GetWatch();

  m_cdv.SetNames( "Moving Average Strategy", pWatch->GetInstrument()->GetInstrumentName() );

  SetupChart();

  time_duration td = time_duration( 0, 0, m_nPeriodWidth );

  m_vMA.emplace_back( MA( pWatch->GetQuotes(), m_vMAPeriods[0], td, ou::Colour::Gold, "ma1" ) );
  m_vMA.emplace_back( MA( pWatch->GetQuotes(), m_vMAPeriods[1], td, ou::Colour::Coral, "ma2" ) );
  m_vMA.emplace_back( MA( pWatch->GetQuotes(), m_vMAPeriods[2], td, ou::Colour::Brown, "ma3" ) );

  for ( vMA_t::value_type& ma: m_vMA ) {
    ma.AddToView( m_cdv );
  }

  pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );

}

void Strategy::Clear() {
  if  ( m_pPosition ) {
    pWatch_t pWatch = m_pPosition->GetWatch();
    pWatch->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuote ) );
    pWatch->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTrade ) );
    m_cdv.Clear();
    m_vMA.clear();
    m_pPosition.reset();
  }
}

void Strategy::HandleQuote( const ou::tf::Quote& quote ) {
  // position has the quotes via the embedded watch
  // indicators are also attached to the embedded watch
  // feed the quote into DailyTradeTimeFrame for proper processing of the trading day

  if ( !quote.IsValid() ) {
    return;
  }

  ptime dt( quote.DateTime() );

  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );

  for ( vMA_t::value_type& ma: m_vMA ) {
    ma.Update( dt );
  }

  TimeTick( quote );
}

void Strategy::HandleTrade( const ou::tf::Trade& trade ) {

  ptime dt( trade.DateTime() );

  m_ceTrade.Append( dt, trade.Price() );
  m_ceVolume.Append( dt, trade.Volume() );

}

void Strategy::HandleRHTrading( const ou::tf::Quote& quote ) { // DailyTradeTimeFrame
  // test the ma crossings here
}

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

Strategy::Strategy( pPosition_t pPosition, ou::ChartDataView& cdv, const config::Options& options )
: ou::tf::DailyTradeTimeFrame<Strategy>()
, m_pPosition( pPosition )
, m_ceShortEntries( ou::ChartEntryShape::EShort, ou::Colour::Red )
, m_ceLongEntries( ou::ChartEntryShape::ELong, ou::Colour::Blue )
, m_ceShortFills( ou::ChartEntryShape::EFillShort, ou::Colour::Red )
, m_ceLongFills( ou::ChartEntryShape::EFillLong, ou::Colour::Blue )
, m_ceShortExits( ou::ChartEntryShape::EShortStop, ou::Colour::Red )
, m_ceLongExits( ou::ChartEntryShape::ELongStop, ou::Colour::Blue )
{
  assert( m_pPosition );

  pWatch_t pWatch = m_pPosition->GetWatch();

  cdv.SetNames( "Moving Average Strategy", pWatch->GetInstrument()->GetInstrumentName() );

  cdv.Add( EChartSlot::Price, &m_ceQuoteAsk );
  cdv.Add( EChartSlot::Price, &m_ceTrade );
  cdv.Add( EChartSlot::Price, &m_ceQuoteBid );

  cdv.Add( EChartSlot::Volume, &m_ceVolume );

  cdv.Add( EChartSlot::Price, &m_ceShortEntries );
  cdv.Add( EChartSlot::Price, &m_ceLongEntries );
  cdv.Add( EChartSlot::Price, &m_ceShortFills );
  cdv.Add( EChartSlot::Price, &m_ceLongFills );
  cdv.Add( EChartSlot::Price, &m_ceShortExits );
  cdv.Add( EChartSlot::Price, &m_ceLongExits );

  m_vMA.push_back( MA( pWatch->GetQuotes(), options.nMA1Periods, time_duration( 0, 0, options.nPeriodWidth ), ou::Colour::Coral, "ma1" ) );
  m_vMA.push_back( MA( pWatch->GetQuotes(), options.nMA2Periods, time_duration( 0, 0, options.nPeriodWidth ), ou::Colour::Brown, "ma2" ) );
  m_vMA.push_back( MA( pWatch->GetQuotes(), options.nMA3Periods, time_duration( 0, 0, options.nPeriodWidth ), ou::Colour::Gold, "ma3" ) );

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );

  m_ceQuoteAsk.SetName( "Ask" );
  m_ceTrade.SetName( "Tick" );
  m_ceQuoteBid.SetName( "Bid" );

  m_ceVolume.SetName( "Volume" );

  for ( vMA_t::value_type& ma: m_vMA ) {
    ma.AddToView( cdv );
  }

  pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );

}

Strategy::~Strategy() {
  assert( m_pPosition );
  pWatch_t pWatch = m_pPosition->GetWatch();

  pWatch->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuote ) );
  pWatch->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTrade ) );
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

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

Strategy::Strategy( pPosition_t pPosition, pChartDataView_t pChartDataView, const config::Options& options )
: ou::tf::DailyTradeTimeFrame<Strategy>()
, m_pPosition( pPosition )
, m_pChartDataView( pChartDataView )
, m_ceShortEntries( ou::ChartEntryShape::EShort, ou::Colour::Red )
, m_ceLongEntries( ou::ChartEntryShape::ELong, ou::Colour::Blue )
, m_ceShortFills( ou::ChartEntryShape::EFillShort, ou::Colour::Red )
, m_ceLongFills( ou::ChartEntryShape::EFillLong, ou::Colour::Blue )
, m_ceShortExits( ou::ChartEntryShape::EShortStop, ou::Colour::Red )
, m_ceLongExits( ou::ChartEntryShape::ELongStop, ou::Colour::Blue )
{
  assert( m_pPosition );
  assert( m_pChartDataView );

  pWatch_t pWatch = m_pPosition->GetWatch();

  m_pChartDataView->SetNames( "Moving Average Strategy", pWatch->GetInstrument()->GetInstrumentName() );

  m_pChartDataView->Add( EChartSlot::Price, &m_ceShortEntries );
  m_pChartDataView->Add( EChartSlot::Price, &m_ceLongEntries );
  m_pChartDataView->Add( EChartSlot::Price, &m_ceShortFills );
  m_pChartDataView->Add( EChartSlot::Price, &m_ceLongFills );
  m_pChartDataView->Add( EChartSlot::Price, &m_ceShortExits );
  m_pChartDataView->Add( EChartSlot::Price, &m_ceLongExits );

  m_vMA.push_back( MA( pWatch->GetQuotes(), options.nMA1Periods, time_duration( 0, 0, options.nPeriodWidth ), ou::Colour::Coral, "ma1" ) );
  m_vMA.push_back( MA( pWatch->GetQuotes(), options.nMA2Periods, time_duration( 0, 0, options.nPeriodWidth ), ou::Colour::Brown, "ma2" ) );
  m_vMA.push_back( MA( pWatch->GetQuotes(), options.nMA3Periods, time_duration( 0, 0, options.nPeriodWidth ), ou::Colour::Gold, "ma3" ) );

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
  TimeTick( quote );
}

void Strategy::HandleTrade( const ou::tf::Trade& ) {
}

void Strategy::HandleRHTrading( const ou::tf::Quote& quote ) {
  // test the ma crossings here
}

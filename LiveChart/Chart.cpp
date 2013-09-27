/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include <TFTrading/InstrumentManager.h>

#include "Chart.h"

ChartTest::ChartTest( pProvider_t pProvider ) 
  : m_dvChart( "Chart1", "GC" ),
  m_bfTrades( 10 )
{
  ou::tf::Instrument::pInstrument_t pInstrument
    = ou::tf::InstrumentManager::Instance().ConstructInstrument( "+GC#", "SMART", ou::tf::InstrumentType::Future );
  m_pWatch = new ou::tf::Watch( pInstrument, pProvider );
  m_pWatch->SetOnQuote( MakeDelegate( this, &ChartTest::HandleQuote ) );
  m_pWatch->SetOnTrade( MakeDelegate( this, &ChartTest::HandleTrade ) );
  m_pWatch->StartWatch();

  m_dvChart.Add( 0, m_ceBars );

  m_bfTrades.SetOnBarComplete( MakeDelegate( this, &ChartTest::HandleBarCompletionTrades ) );
}
 
ChartTest::~ChartTest(void) {
  m_pWatch->StopWatch();
  m_pWatch->SetOnQuote( 0 );
  m_pWatch->SetOnTrade( 0 );
}

void ChartTest::HandleQuote( const ou::tf::Quote& quote ) {
  m_quotes.Append( quote );
}

void ChartTest::HandleTrade( const ou::tf::Trade& trade ) {
  m_trades.Append( trade );
  m_bfTrades.Add( trade );
  //std::cout << trade.Price() << std::endl;
}

void ChartTest::HandleBarCompletionTrades( const ou::tf::Bar& bar ) {
  m_ceBars.AddBar( bar );
}


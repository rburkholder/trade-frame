/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * Project: CurrencyTrader
 * Created: March 14, 2024 21:32:07
 */

#include "Strategy.hpp"

Strategy::Strategy( pPosition_t pPosition )
: m_pPosition( pPosition )
, m_bfQuotes01Sec( 1 )
, m_bfTrading( 3 * 60 ) // TODO: obtain from config file
{

  assert( m_pPosition );

  m_pWatch = m_pPosition->GetWatch();

  m_ceQuoteAsk.SetName( "Ask" );
  m_ceTrade.SetName(    "Tick" );
  m_ceQuoteBid.SetName( "Bid" );

  m_ceVolume.SetName(   "Volume" );

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );

  m_cdv.Add( EChartSlot::Price, &m_ceQuoteAsk );
  m_cdv.Add( EChartSlot::Price, &m_ceTrade );
  m_cdv.Add( EChartSlot::Price, &m_ceQuoteBid );

  m_cdv.Add( EChartSlot::Volume, &m_ceVolume );

  m_cdv.SetNames( "AutoTrade", m_pPosition->GetInstrument()->GetInstrumentName() );

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarQuotes01Sec ) );
  m_bfTrading.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarTrading ) );

  //m_pWatch->RecordSeries( false );
  m_pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );

}

Strategy::~Strategy() {
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuote ) );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTrade ) );
  //m_pWatch->RecordSeries( true );

  m_bfQuotes01Sec.SetOnBarComplete( nullptr );
  m_bfTrading.SetOnBarComplete( nullptr );

  m_cdv.Clear();
}

void Strategy::HandleQuote( const ou::tf::Quote& quote ) {

  ptime dt( quote.DateTime() );

  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );

  //m_quote = quote;
  //m_quotes.Append( quote );

  //TimeTick( quote );

  m_bfQuotes01Sec.Add( dt, quote.Midpoint(), 1 ); // provides a 1 sec pulse for checking the algorithm

}

void Strategy::HandleTrade( const ou::tf::Trade& trade ) {

  ptime dt( trade.DateTime() );

  m_ceTrade.Append( dt, trade.Price() );
  m_ceVolume.Append( dt, trade.Volume() );

  m_bfTrading.Add( dt, trade.Price(), trade.Volume() );

  //const double mid = m_quote.Midpoint();
  //const ou::tf::Trade::price_t price = trade.Price();

}

void Strategy::HandleBarQuotes01Sec( const ou::tf::Bar& ) {
  // calculate ema
}

void Strategy::HandleBarTrading( const ou::tf::Bar& ) {
  // calculate ATR to determine volatility
  // calculate swing points
}

void Strategy::SaveWatch( const std::string& sPrefix ) {
  if ( m_pWatch ) {
    m_pWatch->SaveSeries( sPrefix );
  }
}

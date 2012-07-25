/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include "StrategyEquity.h"

StrategyEquity::StrategyEquity( pProviderSim_t pProvider, pInstrument_t pInstrument, const boost::gregorian::date& dateStart ) 
  : m_pProvider( pProvider ), m_pUnderlying( pInstrument ),
    m_stateTimeFrame( EPreOpen ), m_stateTrading( ENeutral ),
    m_pfnLong( 0 ), m_pfnShort( 0 ),
    m_portfolio( "gp" ), 
    m_timeOpeningBell( 19, 0, 0 ),
    m_timeCancelTrades( 16, 40, 0 ),
    m_timeClosePositions( 16, 45, 0 ),
    m_timeClosingBell( 17, 0, 0 ),
    m_dtOpeningBell( dateStart, m_timeOpeningBell ),
    m_dtStartTrading( m_dtOpeningBell + time_duration( 0, 16, 0 ) ),
    m_dtCancelTrades( dateStart + date_duration( 1 ), m_timeCancelTrades ),
    m_dtClosePositions( dateStart + date_duration( 1 ), m_timeClosePositions ),
    m_dtClosingBell( dateStart + date_duration( 1 ), m_timeClosingBell ),
    m_emaQuotes1( m_quotes, time_duration( 0,  2, 0 ) ), //  2 minutes
    m_emaQuotes2( m_quotes, time_duration( 0,  8, 0 ) ), //  8 minutes
    m_emaQuotes3( m_quotes, time_duration( 0, 32, 0 ) ) // 32 minutes
{
  m_emaQuotes1.SetName( "Q1" );
  m_emaQuotes2.SetName( "Q2" );
  m_emaQuotes3.SetName( "Q3" );
  m_quotes.SetName( "Q" );
  m_trades.SetName( "T" );
  m_quotes.Reserve( 400000 );
  m_trades.Reserve( 100000 );
}

StrategyEquity::~StrategyEquity(void) {
}

void StrategyEquity::Start( void ) {

  Register( &m_quotes );
  Register( &m_trades );
  Register( &m_emaQuotes1 );
  Register( &m_emaQuotes2 );
  Register( &m_emaQuotes3 );

  m_pPosition.reset( new ou::tf::CPosition( m_pUnderlying, m_pProvider, m_pProvider ) );
  m_portfolio.AddPosition( "pos", m_pPosition );


  m_pProvider->AddQuoteHandler( m_pUnderlying, MakeDelegate( this, &StrategyEquity::HandleQuote ) );
  m_pProvider->AddTradeHandler( m_pUnderlying, MakeDelegate( this, &StrategyEquity::HandleTrade ) );

}

double StrategyEquity::GetPL( void ) {
  return m_portfolio.GetRow().dblRealizedPL;
}

void StrategyEquity::Stop( void ) {
  m_pProvider->RemoveQuoteHandler( m_pUnderlying, MakeDelegate( this, &StrategyEquity::HandleQuote ) );
  m_pProvider->RemoveTradeHandler( m_pUnderlying, MakeDelegate( this, &StrategyEquity::HandleTrade ) );
}

void StrategyEquity::HandleQuote( const ou::tf::Quote& quote ) {

  ptime dt( quote.DateTime() );

  if ( quote.IsValid() ) {
    m_quotes.Append( quote );

    switch ( m_stateTimeFrame ) {
    case EPreOpen:
      if ( m_dtOpeningBell <= dt ) {
        m_stateTimeFrame = EBellHeard;
      }
      break;
    case EBellHeard:
      m_stateTimeFrame = EPauseForQuotes;
      break;
    case EPauseForQuotes:
      if ( m_dtStartTrading <= dt ) {
        m_stateTimeFrame = EAfterBell;
      }
      break;
    case EAfterBell:
      m_stateTimeFrame = ETrading;
      break;
    case ETrading:
      if ( m_dtCancelTrades <= dt ) {
        m_pPosition->CancelOrders();
        m_stateTimeFrame = ECancelling;
      }
      else {
        Trade();
      }
      break;
    case ECancelling:
      if ( m_dtClosePositions <= dt ) {
        m_pPosition->ClosePosition();
        m_stateTimeFrame = EClosing;
      }
      break;
    case EGoingNeutral:
      assert( false );
      break;
    case EClosing:
      if ( m_dtClosingBell <= dt ) {
        m_stateTimeFrame = EAfterHours;
      }
      break;
    case EAfterHours:
      break;

    }
  }
}

void StrategyEquity::Trade( void ) {

  assert( 0 != m_pfnLong );
  assert( 0 != m_pfnShort );

  // run the gp generated formula
  bool bLong = m_pfnLong();
  bool bShort = m_pfnShort();

  // execute with result
  switch ( m_stateTrading ) {
  case ENeutral:
    if ( bLong && !bShort ) { // go long
      m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
      m_stateTrading = ELong;
    }
    else {
      if ( !bLong && bShort ) { // go short
        m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 100 );
        m_stateTrading = EShort;
      }
    }
    break;
  case ELong:
    if ( bShort ) { // exit
      m_pPosition->ClosePosition();
      m_stateTrading = ENeutral;
      if ( !bLong ) { // go short
        m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 100 );
        m_stateTrading = EShort;
      }
    }
    break;
  case EShort:
    if ( bLong ) { // exit
      m_pPosition->ClosePosition();
      m_stateTrading = ENeutral;
      if ( !bShort ) { // go long
        m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
        m_stateTrading = ELong;
      }
    }
    break;
  }
}

void StrategyEquity::HandleTrade( const ou::tf::Trade& trade ) {
  m_trades.Append( trade );
}

void StrategyEquity::Register( ou::tf::Prices* series ) {
  m_RegisteredPrices.Register( series );
}

void StrategyEquity::Register( ou::tf::Quotes* series ) {
  m_RegisteredQuotes.Register( series );
}

void StrategyEquity::Register( ou::tf::Trades* series ) {
  m_RegisteredTrades.Register( series );
}

void StrategyEquity::Set( fdEvaluate_t pfnLong, fdEvaluate_t pfnShort ) {
  m_pfnLong = pfnLong;
  m_pfnShort = pfnShort;
}
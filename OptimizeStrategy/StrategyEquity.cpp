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

StrategyEquity::StrategyEquity( pProviderSim_t pProvider, pInstrument_t pInstrument ) 
  : m_pProvider( pProvider ), m_pUnderlying( pInstrument ),
    m_stateTimeFrame( EPreOpen ), m_stateTrading( ENeutral ),
    m_pfnLong( 0 ), m_pfnShort( 0 ),
    m_portfolio( "gp" ), 
    m_emaQuotes1( m_quotes, time_duration( 0,  2, 0 ) ), //  2 minutes
    m_emaQuotes2( m_quotes, time_duration( 0,  8, 0 ) ), //  8 minutes
    m_emaQuotes3( m_quotes, time_duration( 0, 32, 0 ) ) // 32 minutes
{
}

StrategyEquity::~StrategyEquity(void) {
}

void StrategyEquity::Start( void ) {

  Register( &m_emaQuotes1 );
  Register( &m_emaQuotes2 );
  Register( &m_emaQuotes3 );

  m_pPositionLong.reset( new ou::tf::CPosition( m_pUnderlying, m_pProvider, m_pProvider ) );
  m_portfolio.AddPosition( "long", m_pPositionLong );
  m_pPositionShort.reset( new ou::tf::CPosition( m_pUnderlying, m_pProvider, m_pProvider ) );
  m_portfolio.AddPosition( "shrt", m_pPositionShort );


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
  assert( 0 != m_pfnLong );
  assert( 0 != m_pfnShort );
  if ( quote.IsValid() ) {
    m_quotes.Append( quote );

    // run the gp generated formula
    bool bLong = m_pfnLong();
    bool bShort = m_pfnShort();

    // execute with result
    switch ( m_stateTrading ) {
    case ENeutral:
      if ( bLong && !bShort ) { // go long
        m_pPositionLong->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
        m_stateTrading = ELong;
      }
      else {
        if ( !bLong && bShort ) { // go short
          m_pPositionShort->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 100 );
          m_stateTrading = EShort;
        }
      }
      break;
    case ELong:
      if ( bShort ) { // exit
        m_pPositionLong->ClosePosition();
        m_stateTrading = ENeutral;
        if ( !bLong ) { // go short
          m_pPositionShort->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 100 );
          m_stateTrading = EShort;
        }
      }
      break;
    case EShort:
      if ( bLong ) { // exit
        m_pPositionShort->ClosePosition();
        m_stateTrading = ENeutral;
        if ( !bShort ) { // go long
          m_pPositionLong->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
          m_stateTrading = ELong;
        }
      }
      break;
    }

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
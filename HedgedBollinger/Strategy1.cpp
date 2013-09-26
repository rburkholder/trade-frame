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

#include "Strategy1.h"


Strategy::Strategy( ou::tf::option::MultiExpiryBundle* meb ) 
  : m_pBundle( meb ),
  m_dvChart( "Strategy1", "GLD" ),
  m_ema1( *meb->GetWatchUnderlying()->Quotes(), boost::posix_time::seconds(   55 ) ),
  m_ema2( *meb->GetWatchUnderlying()->Quotes(), boost::posix_time::seconds(  233 ) ),
  m_ema3( *meb->GetWatchUnderlying()->Quotes(), boost::posix_time::seconds( 1597 ) ),
  m_bfTrades( 60 ),
  m_bfBuys( 60 ),
  m_bfSells( 60 )
{


  //m_pIndStats1 = new ou::tf::TSSWStatsMidQuote( *m_pBundle->GetWatchUnderlying()->Quotes(), time_duration( 0, 0,   55 ), 0 );
  //m_pIndStats2 = new ou::tf::TSSWStatsMidQuote( *m_pBundle->GetWatchUnderlying()->Quotes(), time_duration( 0, 0,  233 ), 0 );
  //m_pIndStats3 = new ou::tf::TSSWStatsMidQuote( *m_pBundle->GetWatchUnderlying()->Quotes(), time_duration( 0, 0, 1597 ), 0 );

  m_pBundle->GetWatchUnderlying()->SetOnQuote( MakeDelegate( this, &Strategy::HandleQuoteUnderlying ) );
  m_pBundle->GetWatchUnderlying()->SetOnTrade( MakeDelegate( this, &Strategy::HandleTradeUnderlying ) );

  m_bfTrades.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarCompletionTrades ) );
  //m_bfBuys.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarCompletionBuys ) );
  //m_bfSells.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarCompletionSells ) );

  m_dvChart.Add( 0, m_ceBars );

  m_dvChart.Add( 0, m_ceEma1 );
  m_dvChart.Add( 0, m_ceEma2 );
  m_dvChart.Add( 0, m_ceEma3 );
}

Strategy::~Strategy(void) {
  m_pBundle->GetWatchUnderlying()->SetOnQuote( 0 );
  m_pBundle->GetWatchUnderlying()->SetOnTrade( 0 );
}

void Strategy::HandleTradeUnderlying( const ou::tf::Trade& trade ) {
  TimeTick( trade );
}

void Strategy::HandleQuoteUnderlying( const ou::tf::Quote& quote ) {
  if ( !quote.IsValid() ) {
    return;
  }
  TimeTick( quote );
}

void Strategy::HandleRHTrading( const ou::tf::Quote& quote ) {
}

void Strategy::HandleBarCompletionTrades( const ou::tf::Bar& bar ) {
  m_ceBars.AddBar( bar );
}

void Strategy::HandleCommon( const ou::tf::Quote& quote ) {
  ptime dt( quote.DateTime() );
  m_ceEma1.Add( dt, m_ema1.Ago( 0 ).Value() );
  m_ceEma2.Add( dt, m_ema2.Ago( 0 ).Value() );
  m_ceEma3.Add( dt, m_ema3.Ago( 0 ).Value() );
}

void Strategy::HandleCommon( const ou::tf::Trade& trade ) {
  m_bfTrades.Add( trade );
}
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

#include "Trader.h"


Trader::Trader( ou::tf::option::MultiExpiryBundle* meb ) 
  : m_pBundle( meb )
{
  m_pIndStats1 = new ou::tf::TSSWStatsMidQuote( *m_pBundle->GetWatchUnderlying()->Quotes(), time_duration( 0, 0,   55 ), 0 );
  m_pIndStats2 = new ou::tf::TSSWStatsMidQuote( *m_pBundle->GetWatchUnderlying()->Quotes(), time_duration( 0, 0,  233 ), 0 );
  m_pIndStats3 = new ou::tf::TSSWStatsMidQuote( *m_pBundle->GetWatchUnderlying()->Quotes(), time_duration( 0, 0, 1597 ), 0 );

  m_pBundle->GetWatchUnderlying()->SetOnQuote( MakeDelegate( this, &Trader::HandleQuoteUnderlying ) );
  m_pBundle->GetWatchUnderlying()->SetOnTrade( MakeDelegate( this, &Trader::HandleTradeUnderlying ) );
}

Trader::~Trader(void) {
  m_pBundle->GetWatchUnderlying()->SetOnQuote( 0 );
  m_pBundle->GetWatchUnderlying()->SetOnTrade( 0 );
}

void Trader::HandleTradeUnderlying( const ou::tf::Trade& trade ) {
}

void Trader::HandleQuoteUnderlying( const ou::tf::Quote& quote ) {
  TimeTick( quote );
}

void Trader::HandleRHTrading( const ou::tf::Quote& quote ) {
}
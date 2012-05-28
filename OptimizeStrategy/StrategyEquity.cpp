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

StrategyEquity::StrategyEquity( pProviderSim_t pProvider ) 
  : m_pProvider( pProvider )
  // 2 minutes, 8 minutes, 32 minutes
{
}

StrategyEquity::~StrategyEquity(void) {
}

void StrategyEquity::Start( void ) {

  m_pPositionLong.reset( new ou::tf::CPosition( m_pUnderlying, m_pProvider, m_pProvider ) );
  m_pPositionShort.reset( new ou::tf::CPosition( m_pUnderlying, m_pProvider, m_pProvider ) );

  m_pProvider->AddQuoteHandler( m_pUnderlying, MakeDelegate( this, &StrategyEquity::HandleQuote ) );
  m_pProvider->AddTradeHandler( m_pUnderlying, MakeDelegate( this, &StrategyEquity::HandleTrade ) );

}

void StrategyEquity::Stop( void ) {
  m_pProvider->RemoveQuoteHandler( m_pUnderlying, MakeDelegate( this, &StrategyEquity::HandleQuote ) );
  m_pProvider->RemoveTradeHandler( m_pUnderlying, MakeDelegate( this, &StrategyEquity::HandleTrade ) );
}

void StrategyEquity::HandleQuote( const ou::tf::CQuote& ) {
}

void StrategyEquity::HandleTrade( const ou::tf::CTrade& ) {
}


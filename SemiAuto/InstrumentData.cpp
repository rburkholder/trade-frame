/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

//#include "StdAfx.h"
#include "InstrumentData.h"

InstrumentData::InstrumentData( const CInstrument::pInstrument_t& pInstrument ) 
  : m_pInstrument( pInstrument )
{
}

InstrumentData::InstrumentData( CInstrument* pInstrument ) 
  : m_pInstrument( pInstrument )
{
}

InstrumentData::InstrumentData( const InstrumentData& data ) 
  : m_pInstrument( data.m_pInstrument ), m_quotes( data.m_quotes ), m_trades( data.m_trades )
{
}

InstrumentData::~InstrumentData(void) {
}

void InstrumentData::HandleQuote( const CQuote& quote ) {
  m_quotes.Append( quote );
}

void InstrumentData::HandleTrade( const CTrade& trade ) {
  m_trades.Append( trade );
}

void InstrumentData::AddQuoteHandler( CProviderManager::pProvider_t pProvider ) {
  pProvider->AddQuoteHandler(m_pInstrument, MakeDelegate( this, &InstrumentData::HandleQuote ) );
}

void InstrumentData::RemoveQuoteHandler( CProviderManager::pProvider_t pProvider ) {
  pProvider->RemoveQuoteHandler(m_pInstrument, MakeDelegate( this, &InstrumentData::HandleQuote ) );
}

void InstrumentData::AddTradeHandler( CProviderManager::pProvider_t pProvider ) {
  pProvider->AddTradeHandler(m_pInstrument, MakeDelegate( this, &InstrumentData::HandleTrade ) );
}

void InstrumentData::RemoveTradeHandler( CProviderManager::pProvider_t pProvider ) {
  pProvider->RemoveTradeHandler(m_pInstrument, MakeDelegate( this, &InstrumentData::HandleTrade ) );
}


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

#include <sstream>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "InstrumentData.h"

InstrumentData::InstrumentData( const CInstrument::pInstrument_t& pInstrument ) 
  : m_pInstrument( pInstrument ), 
    m_stats( &m_quotes, 14*60 ), m_stoch( &m_quotes, 9*60 ), m_bHasData( false )
{
}

InstrumentData::InstrumentData( CInstrument* pInstrument ) 
  : m_pInstrument( pInstrument ), 
    m_stats( &m_quotes, 14*60 ), m_stoch( &m_quotes, 9*60 ), m_bHasData( false )
{
}

InstrumentData::InstrumentData( const InstrumentData& data ) 
  : m_pInstrument( data.m_pInstrument ),  // only instrument is copied, everything else starts at scratch
    m_stats( &m_quotes, 14*60 ), m_stoch( &m_quotes, 9*60 ), m_bHasData( false )
{
}

InstrumentData::~InstrumentData(void) {
}

void InstrumentData::HandleQuote( const CQuote& quote ) {

  m_quotes.Append( quote );
  m_stats.Update();
  m_stoch.Update();

  m_rSummary[ Roc ] = m_stats.Slope();
  m_rSummary[ Stochastic ] = m_stoch.K();
}

void InstrumentData::HandleTrade( const CTrade& trade ) {
  m_trades.Append( trade );

  // may need to protect cross thread updates
  // may need to do this calculation in a lockable thread, so main stream processor can continue on
  double dblPrice = trade.Trade();
  m_rSummary[ Price ] = dblPrice;
  if ( m_rSummary[ High ] < m_rSummary[ Price ] ) m_rSummary[ High ] = dblPrice;
  if ( m_rSummary[ Low ].Value() == 0 ) m_rSummary[ Low ] = dblPrice;
  else {
    if ( dblPrice < m_rSummary[ Low ].Value() ) m_rSummary[ Low ] = dblPrice;
  }
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

void InstrumentData::SaveSeries( const std::string& sPrefix ) {

  std::string sPathName;

  CHDF5Attributes::structFuture future( m_pInstrument->GetExpiryYear(), m_pInstrument->GetExpiryMonth(), m_pInstrument->GetExpiryDay() );

  if ( 0 != m_quotes.Size() ) {
    sPathName = sPrefix + "/quotes/" + m_pInstrument->GetInstrumentName();
    CHDF5WriteTimeSeries<CQuotes, CQuote> wtsQuotes;
    wtsQuotes.Write( sPathName, &m_quotes );
    CHDF5Attributes attrQuotes( sPathName, future );
    //attributes.SetProviderType( m_pDataProvider->ID() );
  }

  if ( 0 != m_trades.Size() ) {
    sPathName = sPrefix + "/trades/" + m_pInstrument->GetInstrumentName();
    CHDF5WriteTimeSeries<CTrades, CTrade> wtsTrades;
    wtsTrades.Write( sPathName, &m_trades );
    CHDF5Attributes attrTrades( sPathName, future );
    //attributes.SetProviderType( m_pDataProvider->ID() );
  }
}


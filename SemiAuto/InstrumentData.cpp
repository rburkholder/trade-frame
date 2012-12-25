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

InstrumentData::InstrumentData( const Instrument::pInstrument_t& pInstrument, unsigned int nSigDigits ) 
  : m_pInstrument( pInstrument ), 
    m_stats( m_quotes, time_duration( 0, 14, 0 ) ), m_stoch( m_quotes, time_duration( 0, 9, 0 ) ), m_nSignificantDigits( nSigDigits )
{
  Init();
}

InstrumentData::InstrumentData( Instrument* pInstrument, unsigned int nSigDigits ) 
  : m_pInstrument( pInstrument ), 
    m_stats( m_quotes, time_duration( 0, 14, 0 ) ), m_stoch( m_quotes, time_duration( 0, 9, 0 ) ), m_nSignificantDigits( nSigDigits )
{
  Init();
}

InstrumentData::InstrumentData( const InstrumentData& data ) 
  : m_pInstrument( data.m_pInstrument ),  // only instrument is copied, everything else starts at scratch
    m_stats( m_quotes, time_duration( 0, 14, 0 ) ), m_stoch( m_quotes, time_duration( 0, 9, 0 ) ),
    m_nSignificantDigits( data.m_nSignificantDigits )
{
  Init();
}

void InstrumentData::Init( void ) {
  for ( int ix = 0; ix < _Count; ++ix ) m_rSummary[ ix ] = 0.0;
}

InstrumentData::~InstrumentData(void) {
}

void InstrumentData::HandleQuote( const Quote& quote ) {

  m_quotes.Append( quote );
  m_stats.Update();
  m_stoch.Update();

  m_rSummary[ Roc ] = m_stats.Slope() * 100.0;
  m_rSummary[ Stochastic ] = m_stoch.K();
}

void InstrumentData::HandleTrade( const Trade& trade ) {
  m_trades.Append( trade );

  // may need to protect cross thread updates
  // may need to do this calculation in a lockable thread, so main stream processor can continue on
  double dblPrice = trade.Price();
  m_rSummary[ Price ] = dblPrice;
  if ( m_rSummary[ High ] < m_rSummary[ Price ] ) m_rSummary[ High ] = dblPrice;
  if ( m_rSummary[ Low ] == 0 ) m_rSummary[ Low ] = dblPrice;
  else {
    if ( dblPrice < m_rSummary[ Low ] ) m_rSummary[ Low ] = dblPrice;
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

  ou::tf::HDF5Attributes::structFuture future( m_pInstrument->GetExpiryYear(), m_pInstrument->GetExpiryMonth(), m_pInstrument->GetExpiryDay() );

  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );

  if ( 0 != m_quotes.Size() ) {
    sPathName = sPrefix + "/quotes/" + m_pInstrument->GetInstrumentName();
    ou::tf::HDF5WriteTimeSeries<ou::tf::Quotes> wtsQuotes( dm );
    wtsQuotes.Write( sPathName, &m_quotes );
    ou::tf::HDF5Attributes attrQuotes( dm, sPathName, future );
    //attrQuotes.SetMultiplier( 1 );
    //attrQuotes.SetSignificantDigits( 2 );
    //attrTrades.SetProviderType( m_pDataProvider->ID() );
  }

  if ( 0 != m_trades.Size() ) {
    sPathName = sPrefix + "/trades/" + m_pInstrument->GetInstrumentName();
    ou::tf::HDF5WriteTimeSeries<ou::tf::Trades> wtsTrades( dm );
    wtsTrades.Write( sPathName, &m_trades );
    ou::tf::HDF5Attributes attrTrades( dm, sPathName, future );
    //attrTrades.SetMultiplier( 1 );
    //attrTrades.SetSignificantDigits( 2 );
    //attrTrades.SetProviderType( m_pDataProvider->ID() );
  }
}

void InstrumentData::Reset( void ) {
  m_quotes.Clear();
  m_trades.Clear();
  m_stats.Reset();
  m_stoch.Reset();
  Init();  // takes care of m_rSummary
}
/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "InstrumentData.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

InstrumentData::InstrumentData( const CInstrument::pInstrument_t& pInstrument, unsigned int nSigDigits ) 
  : m_pInstrument( pInstrument ), m_nSignificantDigits( nSigDigits )
{
  Init();
}

InstrumentData::InstrumentData( CInstrument* pInstrument, unsigned int nSigDigits ) 
  : m_pInstrument( pInstrument ), m_nSignificantDigits( nSigDigits )
{
  Init();
}

InstrumentData::InstrumentData( const InstrumentData& data ) 
  : m_pInstrument( data.m_pInstrument ),  m_nSignificantDigits( data.m_nSignificantDigits )
{
  Init();
}

void InstrumentData::Init( void ) {
}

InstrumentData::~InstrumentData(void) {
}

void InstrumentData::HandleQuote( const CQuote& quote ) {

  m_quotes.Append( quote );
}

void InstrumentData::HandleTrade( const CTrade& trade ) {
  m_trades.Append( trade );
}

void InstrumentData::HandleGreek( const CGreek& greek ) {
  m_greeks.Append( greek );
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

void InstrumentData::AddGreekHandler( CProviderManager::pProvider_t pProvider ) {
  pProvider->AddGreekHandler(m_pInstrument, MakeDelegate( this, &InstrumentData::HandleGreek ) );
}

void InstrumentData::RemoveGreekHandler( CProviderManager::pProvider_t pProvider ) {
  pProvider->RemoveGreekHandler(m_pInstrument, MakeDelegate( this, &InstrumentData::HandleGreek ) );
}

void InstrumentData::SaveSeries( const std::string& sPrefix ) {

  std::string sPathName;

//  CHDF5Attributes::structFuture future( m_pInstrument->GetExpiryYear(), m_pInstrument->GetExpiryMonth(), m_pInstrument->GetExpiryDay() );

  if ( 0 != m_quotes.Size() ) {
    sPathName = sPrefix + "/quotes/" + m_pInstrument->GetInstrumentName();
    CHDF5WriteTimeSeries<CQuotes, CQuote> wtsQuotes;
    wtsQuotes.Write( sPathName, &m_quotes );
//    CHDF5Attributes attrQuotes( sPathName, future );
    //attrQuotes.SetMultiplier( 1 );
    //attrQuotes.SetSignificantDigits( 2 );
    //attrTrades.SetProviderType( m_pDataProvider->ID() );
  }

  if ( 0 != m_trades.Size() ) {
    sPathName = sPrefix + "/trades/" + m_pInstrument->GetInstrumentName();
    CHDF5WriteTimeSeries<CTrades, CTrade> wtsTrades;
    wtsTrades.Write( sPathName, &m_trades );
//    CHDF5Attributes attrTrades( sPathName, future );
    //attrTrades.SetMultiplier( 1 );
    //attrTrades.SetSignificantDigits( 2 );
    //attrTrades.SetProviderType( m_pDataProvider->ID() );
  }

  if ( 0 != m_greeks.Size() ) {
    sPathName = sPrefix + "/greeks/" + m_pInstrument->GetInstrumentName();
    CHDF5WriteTimeSeries<CGreeks, CGreek> wtsGreeks;
    wtsGreeks.Write( sPathName, &m_greeks );
//    CHDF5Attributes attrTrades( sPathName, future );
    //attrTrades.SetMultiplier( 1 );
    //attrTrades.SetSignificantDigits( 2 );
    //attrTrades.SetProviderType( m_pDataProvider->ID() );
  }

}

void InstrumentData::Reset( void ) {
  m_quotes.Clear();
  m_trades.Clear();
  m_greeks.Clear();
  Init();  // takes care of m_rSummary
}

} // namespace tf
} // namespace ou

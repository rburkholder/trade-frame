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

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "Watch.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

Watch::Watch( pInstrument_t pInstrument, pProvider_t pDataProvider ) :
  m_pInstrument( pInstrument ), 
  m_pDataProvider( pDataProvider ), 
  m_dblBid( 0 ), m_dblAsk( 0 ), m_dblTrade( 0 ),
  m_cntWatching( 0 )
{
  Initialize();
}

Watch::Watch( const Watch& rhs ) :
  m_pInstrument( rhs.m_pInstrument ),
  m_pDataProvider( rhs.m_pDataProvider ),
  m_dblBid( rhs.m_dblBid ), m_dblAsk( rhs.m_dblAsk ), m_dblTrade( rhs.m_dblTrade ),
  m_cntWatching( 0 )
{
  assert( 0 == rhs.m_cntWatching );
  Initialize();
}

Watch::~Watch(void) {
//  StopWatch();  // issues here with inheriting class
}

Watch& Watch::operator=( const Watch& rhs ) {
  assert( 0 == rhs.m_cntWatching );
  assert( 0 == m_cntWatching );
  m_pInstrument = rhs.m_pInstrument;
  m_pDataProvider = rhs.m_pDataProvider;
  m_cntWatching = 0;
  Initialize();
  return *this;
}

void Watch::Initialize( void ) {
  assert( m_pDataProvider->ProvidesQuotes() );
  assert( m_pDataProvider->ProvidesTrades() );
}

void Watch::StartWatch( void ) {
  if ( 0 == m_cntWatching ) {
    m_pDataProvider->AddQuoteHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleQuote ) );
    m_pDataProvider->AddTradeHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleTrade ) );
  }
  ++m_cntWatching;
}

bool Watch::StopWatch( void ) {  // return true if actively stopped feed
  bool b = false;
  assert( 0 != m_cntWatching );
  --m_cntWatching;
  if ( 0 == m_cntWatching ) {
    b = true;
    m_pDataProvider->RemoveQuoteHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleQuote ) );
    m_pDataProvider->RemoveTradeHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleTrade ) );
  }
  return b;
}

void Watch::HandleQuote( const Quote& quote ) {
  m_dblBid = quote.Bid();
  m_dblAsk = quote.Ask();
  m_quotes.Append( quote );
}

void Watch::HandleTrade( const Trade& trade ) {
  m_dblTrade = trade.Price();
  m_trades.Append( trade );
}

void Watch::SaveSeries( const std::string& sPrefix ) {

  std::string sPathName;

  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );

  if ( 0 != m_quotes.Size() ) {
    sPathName = sPrefix + "/quotes/" + m_pInstrument->GetInstrumentName();
    HDF5WriteTimeSeries<ou::tf::Quotes> wtsQuotes( dm );
    wtsQuotes.Write( sPathName, &m_quotes );
    HDF5Attributes attrQuotes( dm, sPathName );
    attrQuotes.SetMultiplier( m_pInstrument->GetMultiplier() );
    attrQuotes.SetSignificantDigits( m_pInstrument->GetSignificantDigits() ); 
    attrQuotes.SetProviderType( m_pDataProvider->ID() );
  }

  if ( 0 != m_trades.Size() ) {
    sPathName = sPrefix + "/trades/" + m_pInstrument->GetInstrumentName();
    HDF5WriteTimeSeries<ou::tf::Trades> wtsTrades( dm );
    wtsTrades.Write( sPathName, &m_trades );
    HDF5Attributes attrTrades( dm, sPathName );
    attrTrades.SetMultiplier( m_pInstrument->GetMultiplier() );
    attrTrades.SetSignificantDigits( m_pInstrument->GetSignificantDigits() );
    attrTrades.SetProviderType( m_pDataProvider->ID() );
  }

}


} // namespace tf
} // namespace ou

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

#include "stdafx.h"

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include <OUCommon/TimeSource.h>

#include <TFIQFeed/IQFeedProvider.h>

#include "Watch.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

Watch::Watch( pInstrument_t pInstrument, pProvider_t pDataProvider ) :
  m_pInstrument( pInstrument ), 
  m_pDataProvider( pDataProvider ), 
  m_cntWatching( 0 ), m_bWatching( false ), m_bWatchingEnabled( false )
{
  assert( 0 != pInstrument.get() );
  assert( 0 != pDataProvider.get() );
  Initialize();
}

Watch::Watch( const Watch& rhs ) :
  m_pInstrument( rhs.m_pInstrument ),
  m_pDataProvider( rhs.m_pDataProvider ),
  m_quote( rhs.m_quote ), m_trade( rhs.m_trade ), 
  m_cntWatching( 0 ), m_bWatching( false ), m_bWatchingEnabled( false )
{
  assert( 0 == rhs.m_cntWatching );
  assert( !rhs.m_bWatching );
  Initialize();
}

Watch::~Watch(void) {
  while ( 0 != m_cntWatching ) {
    StopWatch();
  }
}

Watch& Watch::operator=( const Watch& rhs ) {
  assert( 0 == rhs.m_cntWatching );
  assert( 0 == m_cntWatching );
  assert( !rhs.m_bWatching );
  assert( !m_bWatching );
  m_pInstrument = rhs.m_pInstrument;
  m_pDataProvider = rhs.m_pDataProvider;
  m_cntWatching = 0;
  Initialize();
  return *this;
}

void Watch::Initialize( void ) {
  assert( 0 != m_pInstrument.get() );
  assert( 0 != m_pDataProvider.get() );
  assert( m_pDataProvider->ProvidesQuotes() );
  assert( m_pDataProvider->ProvidesTrades() );
  m_quotes.Reserve( 1024 );  // reduce startup allocations
  m_trades.Reserve( 1024 );  // reduce startup allocations
  AddEvents();
}

void Watch::AddEvents( void ) {
  m_pDataProvider->OnConnected.Add( MakeDelegate( this, &Watch::HandleConnected ) );
  m_pDataProvider->OnDisconnecting.Add( MakeDelegate( this, &Watch::HandleDisconnecting ) );
}

void Watch::RemoveEvents( void ) {
  m_pDataProvider->OnConnected.Remove( MakeDelegate( this, &Watch::HandleConnected ) );
  m_pDataProvider->OnDisconnecting.Remove( MakeDelegate( this, &Watch::HandleDisconnecting ) );
}

void Watch::SetProvider( pProvider_t pDataProvider ) {
  if ( m_pDataProvider.get() != pDataProvider.get() ) {
    DisableWatch();
    RemoveEvents();
  }
  m_pDataProvider = pDataProvider;
  AddEvents();
  EnableWatch();
}

// non gui thread
void Watch::HandleConnecting( int ) {
}

// non gui thread
void Watch::HandleConnected( int ) {
  EnableWatch();
}

// non gui thread
void Watch::HandleDisconnecting( int ) {
  DisableWatch();
}

// non gui thread
void Watch::HandleDisconnected( int ) {
  
}

void Watch::EnableWatch( void ) {
  if ( m_bWatchingEnabled && !m_bWatching && m_pDataProvider->Connected() ) {
    m_bWatching = true;
    m_pDataProvider->AddQuoteHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleQuote ) );
    m_pDataProvider->AddTradeHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleTrade ) );
    std::cout << "Start Watching " << m_pInstrument->GetInstrumentName() << std::endl;
    // these two message types come second so that the symbol gets registered in previous statements
    if ( ou::tf::keytypes::EProviderIQF == m_pDataProvider->ID() ) {
      ou::tf::IQFeedProvider::pProvider_t pIQFeedProvider;
      pIQFeedProvider = boost::dynamic_pointer_cast<IQFeedProvider>( m_pDataProvider );
      ou::tf::IQFeedProvider::pSymbol_t pSymbol
        = pIQFeedProvider->GetSymbol( m_pInstrument->GetInstrumentName( ou::tf::keytypes::EProviderIQF ) );
      pSymbol->OnFundamentalMessage.Add( MakeDelegate( this, &Watch::HandleIQFeedFundamentalMessage ) );
      pSymbol->OnSummaryMessage.Add( MakeDelegate( this, &Watch::HandleIQFeedSummaryMessage ) );
    }
    else {
      std::cout << m_pInstrument->GetInstrumentName() << ": Watch works best with IQFeed" << std::endl;
    }
  }
}

void Watch::StartWatch( void ) {
  if ( 0 == m_cntWatching ) {
    m_bWatchingEnabled = true;
    EnableWatch();
  }
  ++m_cntWatching;
}

void Watch::DisableWatch( void ) {
  if ( m_bWatching ) {
    m_bWatching = false;
    m_pDataProvider->RemoveQuoteHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleQuote ) );
    m_pDataProvider->RemoveTradeHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleTrade ) );
    std::cout << "Stop Watching " << m_pInstrument->GetInstrumentName() << std::endl;
    if ( ou::tf::keytypes::EProviderIQF == m_pDataProvider->ID() ) {
      ou::tf::IQFeedProvider::pProvider_t pIQFeedProvider;
      pIQFeedProvider = boost::dynamic_pointer_cast<IQFeedProvider>( m_pDataProvider );
      ou::tf::IQFeedProvider::pSymbol_t pSymbol
        = pIQFeedProvider->GetSymbol( m_pInstrument->GetInstrumentName( ou::tf::keytypes::EProviderIQF ) );
      pSymbol->OnSummaryMessage.Remove( MakeDelegate( this, &Watch::HandleIQFeedSummaryMessage ) );
      pSymbol->OnFundamentalMessage.Remove( MakeDelegate( this, &Watch::HandleIQFeedFundamentalMessage ) );
    }
  }
}

bool Watch::StopWatch( void ) {  // return true if actively stopped feed
  assert( 0 != m_cntWatching );
  --m_cntWatching;
  if ( 0 == m_cntWatching ) {
    m_bWatchingEnabled = false;
    DisableWatch();
  }
  return 0 == m_cntWatching;
}

void Watch::EmitValues( void ) const {
  std::cout << m_pInstrument->GetInstrumentName() << ": " 
    << "Cnt=" << m_quotes.Size() << "(q)," << m_trades.Size() << "(t)"
    << ",P=" << m_trade.Price()
    << ",B=" << m_quote.Bid() 
    << ",A=" << m_quote.Ask()
    << std::endl;
}

void Watch::HandleQuote( const Quote& quote ) {
  m_quote = quote;
  //OnPossibleResizeBegin( stateTimeSeries_t( m_quotes.Capacity(), m_quotes.Size() ) );
  {
    //boost::mutex::scoped_lock lock(m_mutexLockAppend);
    m_quotes.Append( quote );
  }
  
  //OnPossibleResizeEnd( stateTimeSeries_t( m_quotes.Capacity(), m_quotes.Size() ) );
  //if ( 0 != m_OnQuote ) m_OnQuote( quote );
  OnQuote( quote );
}

void Watch::HandleTrade( const Trade& trade ) {
  m_trade = trade;
  //OnPossibleResizeBegin( stateTimeSeries_t( m_trades.Capacity(), m_trades.Size() ) );
  {
    //boost::mutex::scoped_lock lock(m_mutexLockAppend);
    m_trades.Append( trade );
  }
  //OnPossibleResizeEnd( stateTimeSeries_t( m_trades.Capacity(), m_trades.Size() ) );
  //if ( 0 != m_OnTrade ) m_OnTrade( trade );
  OnTrade( trade );
}

void Watch::HandleIQFeedFundamentalMessage( ou::tf::IQFeedSymbol& symbol ) {
  m_fundamentals.dblHistoricalVolatility = symbol.m_dblHistoricalVolatility;
  m_fundamentals.nShortInterest = symbol.m_nShortInterest;
  m_fundamentals.dblPriceEarnings = symbol.m_dblPriceEarnings;
  m_fundamentals.dbl52WkHi = symbol.m_dbl52WkHi;
  m_fundamentals.dbl52WkLo = symbol.m_dbl52WkLo;
  m_fundamentals.dblDividendAmount = symbol.m_dblDividendAmount;
  m_fundamentals.dblDividendRate = symbol.m_dblDividendRate;
  m_fundamentals.dblDividendYield = symbol.m_dblDividendYield;
  m_fundamentals.dateExDividend = symbol.m_dateExDividend;
}

void Watch::HandleIQFeedSummaryMessage( ou::tf::IQFeedSymbol& symbol ) {
  m_summary.nOpenInterest = symbol.m_nOpenInterest;
  m_summary.nTotalVolume = symbol.m_nTotalVolume;
  m_summary.dblOpen = symbol.m_dblOpen;
  m_quote = ou::tf::Quote( ou::TimeSource::Instance().External(), symbol.m_dblBid, 0, symbol.m_dblAsk, 0 );
  m_trade = ou::tf::Trade( ou::TimeSource::Instance().External(), symbol.m_dblTrade, 0 );
}

void Watch::SaveSeries( const std::string& sPrefix ) {

  std::string sPathName;

  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );

  try {

    if ( 0 != m_quotes.Size() ) {
      sPathName = sPrefix + "/quotes/" + m_pInstrument->GetInstrumentName();
      HDF5WriteTimeSeries<ou::tf::Quotes> wtsQuotes( dm, true, true, 5, 256 );
      wtsQuotes.Write( sPathName, &m_quotes );
      HDF5Attributes attrQuotes( dm, sPathName );
      attrQuotes.SetSignature( ou::tf::Quote::Signature() );
      attrQuotes.SetMultiplier( m_pInstrument->GetMultiplier() );
      attrQuotes.SetSignificantDigits( m_pInstrument->GetSignificantDigits() ); 
      attrQuotes.SetProviderType( m_pDataProvider->ID() );
    }

    if ( 0 != m_trades.Size() ) {
      sPathName = sPrefix + "/trades/" + m_pInstrument->GetInstrumentName();
      HDF5WriteTimeSeries<ou::tf::Trades> wtsTrades( dm, true, true, 5, 256 );
      wtsTrades.Write( sPathName, &m_trades );
      HDF5Attributes attrTrades( dm, sPathName );
      attrTrades.SetSignature( ou::tf::Trade::Signature() );
      attrTrades.SetMultiplier( m_pInstrument->GetMultiplier() );
      attrTrades.SetSignificantDigits( m_pInstrument->GetSignificantDigits() );
      attrTrades.SetProviderType( m_pDataProvider->ID() );
    }

  }
  catch (...) {
    std::cout << "Watch::SaveSeries error: " << sPrefix << std::endl;
  }

}


} // namespace tf
} // namespace ou

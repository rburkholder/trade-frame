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

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include <OUCommon/TimeSource.h>

#include <TFIQFeed/Provider.h>

#include "Watch.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

Watch::Watch( pInstrument_t& pInstrument, pProvider_t pDataProvider )
: m_pInstrument( pInstrument )
, m_pDataProvider( pDataProvider )
, m_PriceMax( 0 ), m_PriceMin( 0 ), m_VolumeTotal( 0 )
, m_bWatching( false ), m_bWatchingEnabled( false ), m_bRecordSeries( true )
, m_cntWatching {}, m_nEnableStats {}
, m_cntBestSpread {}, m_dblBestSpread {}, m_cntTotalSpread {}
, m_bEventsAttached( false )
, m_bAddedDepthByMMHandler( false ), m_bAddedDepthByOrderHandler( false )
, m_bAddedQuoteHandler( false )
{
  assert( pInstrument );
  assert( pDataProvider );
  Initialize();
}

Watch::Watch( const Watch& rhs )
: m_pInstrument( rhs.m_pInstrument )
, m_pDataProvider( rhs.m_pDataProvider )
, m_PriceMax( rhs.m_PriceMax ), m_PriceMin( rhs.m_PriceMin ), m_VolumeTotal( rhs.m_VolumeTotal )
, m_quote( rhs.m_quote ), m_trade( rhs.m_trade )
, m_bWatching( false ), m_bWatchingEnabled( false ), m_bRecordSeries( rhs.m_bRecordSeries )
, m_cntWatching {}, m_nEnableStats {}
, m_cntBestSpread {}, m_dblBestSpread {}, m_cntTotalSpread {}
, m_bEventsAttached( false )
, m_bAddedDepthByMMHandler( false ), m_bAddedDepthByOrderHandler( false )
, m_bAddedQuoteHandler( false )
{
  assert( 0 == rhs.m_cntWatching );
  assert( 0 == rhs.m_nEnableStats );
  assert( !rhs.m_bWatching );
  Initialize();
}

Watch::~Watch() {
  RemoveEvents();
  while ( 0 != m_cntWatching ) {
    StopWatch();
  }
}

// TODO: need to test this code.  Initialize state properly?
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

void Watch::Initialize() {
  assert( m_pInstrument );
  assert( m_pDataProvider );
  //assert(  );
  assert( m_pDataProvider->ProvidesTrades() );
  if ( m_pDataProvider->ProvidesQuotes() ) {
    m_quotes.Reserve( 1024 );  // reduce startup allocations
  }
  m_trades.Reserve( 1024 );  // reduce startup allocations
  m_depths_mm.Reserve( 1024 );  // reduce startup allocations
  m_depths_order.Reserve( 1024 );  // reduce startup allocations
  AddEvents();
  // TODO: check that instrument name, or alt instrument name matches provider type:
  //    contract exists for IBTWS provider, IQFeedName exists for IQFeed provider
  //   also needs to be validated in SetProvider
  //  EnableWatch takes care of some of that, but doesn't confirm contract if using IBTWS as provider
}

void Watch::AddEvents() {
  assert( !m_bEventsAttached );
  m_pDataProvider->OnConnected.Add( MakeDelegate( this, &Watch::HandleConnected ) );
  m_pDataProvider->OnDisconnecting.Add( MakeDelegate( this, &Watch::HandleDisconnecting ) );
  m_bEventsAttached = true;
}

void Watch::RemoveEvents() {
  assert( m_bEventsAttached );
  m_pDataProvider->OnConnected.Remove( MakeDelegate( this, &Watch::HandleConnected ) );
  m_pDataProvider->OnDisconnecting.Remove( MakeDelegate( this, &Watch::HandleDisconnecting ) );
  m_bEventsAttached = false;
}

void Watch::SetProvider( pProvider_t& pDataProvider ) {
  if ( m_pDataProvider.get() != pDataProvider.get() ) {
    DisableWatch();
    RemoveEvents();
  }
  m_pDataProvider = pDataProvider;
  AddEvents();
  EnableWatch();
}

void Watch::EnableStatsAdd() {
  m_nEnableStats++;
}

void Watch::EnableStatsRemove() {
  assert( 0 != m_nEnableStats );
  m_nEnableStats--;
  if ( 0 == m_nEnableStats ) {
    m_cntTotalSpread = m_cntBestSpread = 0;
    m_dblBestSpread = 0.0;
  }
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

void Watch::EnableWatch() {
  if ( m_bWatchingEnabled && !m_bWatching && m_pDataProvider->Connected() ) {
    // std::cout << "Start Watching " << m_pInstrument->GetInstrumentName() << std::endl;

    if (
         ( 0 == OnQuote.Size() )
      && ( 0 == OnTrade.Size() )
      && ( 0 == OnDepthByMM.Size() )
      && ( 0 == OnDepthByOrder.Size() )
      && ( 0 == OnFundamentals.Size() )
      && ( 0 == OnSummary.Size() )
    ) {
      std::cout << m_pInstrument->GetInstrumentName() << ": warning, no OnXXX events assigned" << std::endl;
    }

    m_bWatching = true;

    if ( ou::tf::keytypes::EProviderIQF == m_pDataProvider->ID() ) { // hook up prior to watch start
      // NOTE: if there are link errors for this, re-order TFIQFeed to after TFTrading
      ou::tf::iqfeed::Provider::pProvider_t pIQFeedProvider
        = ou::tf::iqfeed::Provider::Cast( m_pDataProvider );
      ou::tf::iqfeed::Provider::pSymbol_t pSymbol
        = pIQFeedProvider->GetSymbol( m_pInstrument );
      m_pFundamentals = pSymbol->GetFundamentals(); // preload
      pSymbol->OnFundamentalMessage.Add( MakeDelegate( this, &Watch::HandleIQFeedFundamentalMessage ) );
      pSymbol->OnSummaryMessage.Add( MakeDelegate( this, &Watch::HandleIQFeedSummaryMessage ) );
    }
    else {
      std::cout << m_pInstrument->GetInstrumentName() << ": Watch works best with IQFeed" << std::endl;
    }

    // these two message types come second so that the symbol gets registered in previous statements
    if ( 0 != OnQuote.Size() ) {
      m_pDataProvider->AddQuoteHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleQuote ) );
      m_bAddedQuoteHandler = true;
    }
    m_pDataProvider->AddTradeHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleTrade ) );
    if ( m_pDataProvider->ProvidesDepth() ) {
      if ( 0 != OnDepthByMM.Size() ) {
        m_pDataProvider->AddDepthByMMHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleDepthByMM ) );
        m_bAddedDepthByMMHandler = true;
      }
      if ( 0 != OnDepthByOrder.Size() ) {
        m_pDataProvider->AddDepthByOrderHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleDepthByOrder ) );
        m_bAddedDepthByOrderHandler = true;
      }

    }
  }
}

bool Watch::StartWatch() {
//  std::cout << "Watch::StartWatch: "  << this->m_pInstrument->GetInstrumentName() << " " << m_cntWatching << std::endl;
  if ( 0 == m_cntWatching ) {
    m_bWatchingEnabled = true;
    EnableWatch();
  }
  ++m_cntWatching;
  return ( 1 == m_cntWatching );
}

void Watch::DisableWatch() {
  if ( m_bWatching ) {
    //std::cout << "Stop Watching " << m_pInstrument->GetInstrumentName() << std::endl;
    if ( m_pDataProvider->ProvidesDepth() ) {
      if ( m_bAddedDepthByMMHandler ) {
        m_pDataProvider->RemoveDepthByMMHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleDepthByMM ) );
        m_bAddedDepthByMMHandler = false;
      }
      if ( m_bAddedDepthByOrderHandler ) {
        m_pDataProvider->RemoveDepthByOrderHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleDepthByOrder ) );
        m_bAddedDepthByOrderHandler = false;
      }

    }
    m_pDataProvider->RemoveTradeHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleTrade ) );
    if ( m_bAddedQuoteHandler ) {
      m_pDataProvider->RemoveQuoteHandler( m_pInstrument, MakeDelegate( this, &Watch::HandleQuote ) );
      m_bAddedQuoteHandler = false;
    }
    m_bWatching = false;
    if ( ou::tf::keytypes::EProviderIQF == m_pDataProvider->ID() ) {
      ou::tf::iqfeed::Provider::pProvider_t pIQFeedProvider;
      // NOTE: if there are link errors for this, re-order TFIQFeed to after TFTrading
      pIQFeedProvider = ou::tf::iqfeed::Provider::Cast( m_pDataProvider );
      ou::tf::iqfeed::Provider::pSymbol_t pSymbol
        = pIQFeedProvider->GetSymbol( m_pInstrument );
      pSymbol->OnSummaryMessage.Remove( MakeDelegate( this, &Watch::HandleIQFeedSummaryMessage ) );
      pSymbol->OnFundamentalMessage.Remove( MakeDelegate( this, &Watch::HandleIQFeedFundamentalMessage ) );
    }
  }
}

bool Watch::StopWatch() {  // return true if actively stopped feed
//  std::cout << "Watch::StopWatch: " << this->m_pInstrument->GetInstrumentName() << " " << m_cntWatching << std::endl;
  assert( 0 != m_cntWatching );
  --m_cntWatching;
  if ( 0 == m_cntWatching ) {
    m_bWatchingEnabled = false;
    DisableWatch();
  }
  return 0 == m_cntWatching;
}

void Watch::EmitValues( bool bEmitName ) const {
  if ( bEmitName ) {
    std::cout << m_pInstrument->GetInstrumentName() << ": ";
  }
  if ( m_pDataProvider->ProvidesQuotes() ) {
    std::cout
      << "Cnt=" << m_quotes.Size() << "(q)," << m_trades.Size() << "(t)"
      << ",P=" << m_trade.Price()
      << ",B=" << m_quote.Bid()
      << ",A=" << m_quote.Ask()
      //<< std::endl
      ;
  }
  else {
    std::cout
      << "Cnt=" << m_trades.Size() << "(t)"
      << ",P=" << m_trade.Price()
      //<< std::endl
      ;
  }
}

void Watch::HandleQuote( const Quote& quote ) {

  // TODO: mean, median, mode on spread to determine 'normal' spread for actionable events
  //   * sliding window for n quotes or n seconds?
  //   * need to filter quotes when value is at zero as end of life otm
  //   * build map for mean/median/mode to filter, emit values for review
  if ( quote.IsNonZero() ) {
    if ( 0 < m_nEnableStats ) {

      double spread = quote.Spread();

      // TODO: need a periodic sampler to evaluate median
      //   Them implement a query call to go/no-go value usage for ordering
      // NOTE: spread may change in and out of regular trading hours
      //   may require a reset on the calcuation

      mapQuoteDistribution_t::iterator iterMapQuoteDistribution = m_mapQuoteDistribution.find( spread );
      if ( m_mapQuoteDistribution.end() == iterMapQuoteDistribution ) {
        auto pair = m_mapQuoteDistribution.emplace( std::make_pair( spread, 1 ) );
        assert( pair.second );
        iterMapQuoteDistribution = pair.first;
      }
      else {
        iterMapQuoteDistribution->second++;
      }
      m_cntTotalSpread++;

      // keep track of most used spread
      if ( m_cntBestSpread < iterMapQuoteDistribution->second ) {
        m_dblBestSpread = iterMapQuoteDistribution->first;
        m_cntBestSpread = iterMapQuoteDistribution->second;
      }
      else {
        if ( m_cntBestSpread == iterMapQuoteDistribution->second ) {
          if ( spread < m_dblBestSpread ) {
            // use smaller spread when count is same as larger spread
            m_dblBestSpread = iterMapQuoteDistribution->first;
            m_cntBestSpread = iterMapQuoteDistribution->second;
          }
        }
      }

      m_quote = quote;
      if ( m_bRecordSeries ) {
        m_quotes.Append( quote );
      }

      OnQuote( quote );
    }
    else {
        m_quote = quote;
        //OnPossibleResizeBegin( stateTimeSeries_t( m_quotes.Capacity(), m_quotes.Size() ) );
        {
          //boost::mutex::scoped_lock lock(m_mutexLockAppend);
          if ( m_bRecordSeries ) m_quotes.Append( quote );
        }

        //OnPossibleResizeEnd( stateTimeSeries_t( m_quotes.Capacity(), m_quotes.Size() ) );
        OnQuote( quote );
    }
  }
  else {
        //    std::cout
        //      << quote.DateTime().time_of_day() << ","
        //      << m_pInstrument->GetInstrumentName()
        //      << " zero quote "
        //      << "b=" << quote.Bid() << "x" << quote.BidSize() << ","
        //      << "a=" << quote.Ask() << "x" << quote.AskSize()
        //      << std::endl;
  }
}

void Watch::HandleTrade( const Trade& trade ) {
  m_trade = trade;
  if ( trade.Price() > m_PriceMax ) m_PriceMax = trade.Price();
  if ( trade.Price() < m_PriceMin ) m_PriceMin = trade.Price();
  m_VolumeTotal += trade.Volume();
  //OnPossibleResizeBegin( stateTimeSeries_t( m_trades.Capacity(), m_trades.Size() ) );
  {
    //boost::mutex::scoped_lock lock(m_mutexLockAppend);
    if ( m_bRecordSeries ) m_trades.Append( trade );
  }
  //OnPossibleResizeEnd( stateTimeSeries_t( m_trades.Capacity(), m_trades.Size() ) );
  //if ( 0 != m_OnTrade ) m_OnTrade( trade );
  OnTrade( trade );
}

void Watch::HandleDepthByMM( const DepthByMM& depth ) {
  if ( m_bRecordSeries ) m_depths_mm.Append( depth );
  OnDepthByMM( depth );
}

void Watch::HandleDepthByOrder( const DepthByOrder& depth ) {
  if ( m_bRecordSeries ) m_depths_order.Append( depth );
  OnDepthByOrder( depth );
}

void Watch::HandleIQFeedFundamentalMessage( ou::tf::iqfeed::IQFeedSymbol::pFundamentals_t pFundamentals ) {
  m_pFundamentals = pFundamentals;
  OnFundamentals( *m_pFundamentals );
}

void Watch::HandleIQFeedSummaryMessage( ou::tf::iqfeed::IQFeedSymbol::pSummary_t pSummary ) {

  const ou::tf::iqfeed::IQFeedSymbol::Summary& summary( *pSummary );

  m_summary.nOpenInterest = summary.nOpenInterest;
  m_summary.nTotalVolume = summary.nTotalVolume;
  if ( 0.0 == m_summary.dblOpen ) {
    m_PriceMax = m_PriceMin = summary.dblOpen;
  }
  m_summary.dblOpen = summary.dblOpen;
  m_summary.dblTrade = summary.dblTrade;

  m_quote = ou::tf::Quote( ou::TimeSource::GlobalInstance().External(), summary.dblBid, 0, summary.dblAsk, 0 );
  m_trade = ou::tf::Trade( ou::TimeSource::GlobalInstance().External(), summary.dblTrade, 0 );

  OnSummary( m_summary );
}

void Watch::SaveSeries( const std::string& sPrefix ) {

  //size_t step {};
  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );

  try {

    std::string sPathName;

    if ( 0 != m_quotes.Size() ) {
      sPathName = sPrefix + Quotes::Directory() + m_pInstrument->GetInstrumentName();
      HDF5WriteTimeSeries<ou::tf::Quotes> wtsQuotes( dm, true, true, 5, 256 );
      wtsQuotes.Write( sPathName, &m_quotes );
      HDF5Attributes attrQuotes( dm, sPathName );
      attrQuotes.SetSignature( ou::tf::Quote::Signature() );
      attrQuotes.SetMultiplier( m_pInstrument->GetMultiplier() );
      attrQuotes.SetSignificantDigits( m_pInstrument->GetSignificantDigits() );
      attrQuotes.SetProviderType( m_pDataProvider->ID() );
    }

    if ( 0 != m_trades.Size() ) {
      sPathName = sPrefix + Trades::Directory() + m_pInstrument->GetInstrumentName();
      //step = 1;
      HDF5WriteTimeSeries<ou::tf::Trades> wtsTrades( dm, true, true, 5, 256 );
      //step = 2;
      wtsTrades.Write( sPathName, &m_trades );
      //step = 3;
      HDF5Attributes attrTrades( dm, sPathName );
      //step = 4;
      attrTrades.SetSignature( ou::tf::Trade::Signature() );
      //step = 5;
      attrTrades.SetMultiplier( m_pInstrument->GetMultiplier() );
      //step = 6;
      attrTrades.SetSignificantDigits( m_pInstrument->GetSignificantDigits() );
      //step = 7;
      attrTrades.SetProviderType( m_pDataProvider->ID() );
    }

    if ( 0 != m_depths_mm.Size() ) {
      sPathName = sPrefix + DepthsByMM::Directory() + m_pInstrument->GetInstrumentName();
      HDF5WriteTimeSeries<ou::tf::DepthsByMM> wtsDepths( dm, true, true, 5, 256 );
      wtsDepths.Write( sPathName, &m_depths_mm );
      HDF5Attributes attrDepths( dm, sPathName );
      attrDepths.SetSignature( ou::tf::DepthByMM::Signature() );
      //attrDepths.SetMultiplier( m_pInstrument->GetMultiplier() );
      //attrDepths.SetSignificantDigits( m_pInstrument->GetSignificantDigits() );
      attrDepths.SetProviderType( m_pDataProvider->ID() );
    }

    if ( 0 != m_depths_order.Size() ) {
      sPathName = sPrefix + DepthsByOrder::Directory() + m_pInstrument->GetInstrumentName();
      HDF5WriteTimeSeries<ou::tf::DepthsByOrder> wtsDepths( dm, true, true, 5, 256 );
      wtsDepths.Write( sPathName, &m_depths_order );
      HDF5Attributes attrDepths( dm, sPathName );
      attrDepths.SetSignature( ou::tf::DepthByOrder::Signature() );
      //attrDepths.SetMultiplier( m_pInstrument->GetMultiplier() );
      //attrDepths.SetSignificantDigits( m_pInstrument->GetSignificantDigits() );
      attrDepths.SetProviderType( m_pDataProvider->ID() );
    }

  }
  catch (...) {
    std::cout
      << "Watch::SaveSeries error: "
      << sPrefix << ","
      << m_pInstrument->GetInstrumentName()
      //<< ",step=" << step
      << std::endl;
  }

}

void Watch::SaveSeries( const std::string& sPrefix, const std::string& sDaily ) {

  SaveSeries( sPrefix );

  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );

  std::string sPathName;

  try {

    if ( 0 != m_VolumeTotal ) {
      ou::tf::Bar bar( m_trade.DateTime(), m_summary.dblOpen, m_PriceMax, m_PriceMin, m_trade.Price(), m_VolumeTotal );
      ou::tf::Bars bars;
      bars.Append( bar );
      sPathName = sDaily + "/daily/" + m_pInstrument->GetInstrumentName();
      HDF5WriteTimeSeries<ou::tf::Bars> wtsBars( dm, true, true, 5, 256 );
      wtsBars.Write( sPathName, &bars );
      HDF5Attributes attrBars( dm, sPathName );
      attrBars.SetSignature( ou::tf::Bar::Signature() );
      attrBars.SetMultiplier( m_pInstrument->GetMultiplier() );
      attrBars.SetSignificantDigits( m_pInstrument->GetSignificantDigits() );
      attrBars.SetProviderType( m_pDataProvider->ID() );
    }

  }
  catch (...) {
    std::cout << "Watch::SaveSeries2 error: " << sPrefix << std::endl;
  }

}

void Watch::ClearSeries() {
  m_quotes.Clear();
  m_trades.Clear();
  m_depths_mm.Clear();
  m_depths_order.Clear();
}

} // namespace tf
} // namespace ou

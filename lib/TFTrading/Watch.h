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

#pragma once

#include <memory>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <OUCommon/Delegate.h>

#include <TFTimeSeries/TimeSeries.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderInterface.h>

#include <TFIQFeed/Symbol.h>

// 20151228 convert the delegate to a signal?  a little slower maybe.

// 20160122 will want to set signals on provider so watch/unwatch as provider transitions connection states

namespace ou { // One Unified
namespace tf { // TradeFrame

class Watch {
public:

  using pWatch_t = std::shared_ptr<Watch>;
  using idInstrument_t = Instrument::idInstrument_t;
  using pInstrument_t = Instrument::pInstrument_t;
  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;

  using Fundamentals = ou::tf::iqfeed::Fundamentals;

  struct Summary {
    int nOpenInterest;
    int nTotalVolume;
    double dblOpen;
    Summary(): nOpenInterest{}, nTotalVolume{}, dblOpen{} {}
  };

  Watch( pInstrument_t pInstrument, pProvider_t pDataProvider );
  Watch( const Watch& rhs );
  virtual ~Watch();

  virtual Watch& operator=( const Watch& rhs );

  bool operator< ( const Watch& rhs ) const { return m_pInstrument->GetInstrumentName() <  rhs.m_pInstrument->GetInstrumentName(); };
  bool operator<=( const Watch& rhs ) const { return m_pInstrument->GetInstrumentName() <= rhs.m_pInstrument->GetInstrumentName(); };

  pInstrument_t GetInstrument() { return m_pInstrument; }
  const idInstrument_t& GetInstrumentName() const { return m_pInstrument->GetInstrumentName(); }

  void SetProvider( pProvider_t pDataProvider );
  pProvider_t GetProvider() { return m_pDataProvider; };

  bool Watching() const { return 0 != m_cntWatching; };

  // TODO: these need spinlocks
  inline const Quote& LastQuote() const { return m_quote; };  // may have thread sync issue
  inline const Trade& LastTrade() const { return m_trade; };  // may have thread sync issue

  const Fundamentals& GetFundamentals() const { assert( m_pFundamentals ); return *m_pFundamentals; };
  const Summary& GetSummary() const { return m_summary; };

  const Quotes& GetQuotes() const { return m_quotes; };
  const Trades& GetTrades() const { return m_trades; };

  ou::Delegate<const Quote&> OnQuote;
  ou::Delegate<const Trade&> OnTrade;
  ou::Delegate<const Fundamentals&> OnFundamentals;
  ou::Delegate<const Summary&> OnSummary;

  //typedef std::pair<size_t,size_t> stateTimeSeries_t;
  //ou::Delegate<const stateTimeSeries_t&> OnPossibleResizeBegin;
  //ou::Delegate<const stateTimeSeries_t&> OnPossibleResizeEnd;

  virtual bool StartWatch();
  virtual bool StopWatch();

  void EnableStatsAdd();
  void EnableStatsRemove();

  virtual void EmitValues( bool bEmitName = true ) const;

  void RecordSeries( bool bRecord ) { m_bRecordSeries = bRecord; }
  bool RecordingSeries() const { return m_bRecordSeries; }

  virtual void SaveSeries( const std::string& sPrefix );
  virtual void SaveSeries( const std::string& sPrefix, const std::string& sDaily );

  virtual void ClearSeries();

  using tupleSpreadStats_t = std::tuple<bool, size_t,double>;
  tupleSpreadStats_t SpreadStats() const {
    return std::make_tuple( m_cntBestSpread > ( m_cntTotalSpread / 5 ), m_cntBestSpread, m_dblBestSpread );
  }

protected:

  // use an iterator instead?  or keep as is as it facilitates multi-thread append and access operations
  // or will the stuff in TBB help with this type of access?

  bool m_bRecordSeries;

  ou::tf::Quote m_quote;
  ou::tf::Trade m_trade;

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  pInstrument_t m_pInstrument;

  pProvider_t m_pDataProvider;

private:

  bool m_bWatchingEnabled;
  bool m_bWatching; // in/out of connected state
  bool m_bEventsAttached; // code validation

  size_t m_cntWatching;
  size_t m_nEnableStats;

  ou::tf::iqfeed::IQFeedSymbol::pFundamentals_t m_pFundamentals;

  Summary m_summary;

  ou::tf::Trade::price_t m_PriceMax;
  ou::tf::Trade::price_t m_PriceMin;
  ou::tf::Trade::volume_t m_VolumeTotal;

  // number of quotes at spread
  using mapQuoteDistribution_t = std::map<double,size_t>;
  mapQuoteDistribution_t m_mapQuoteDistribution;

  size_t m_cntTotalSpread;
  size_t m_cntBestSpread;
  double m_dblBestSpread;

  void Initialize();

  void AddEvents();
  void RemoveEvents();

  void HandleConnecting( int );
  void HandleConnected( int );
  void HandleDisconnecting( int );
  void HandleDisconnected( int );

  void EnableWatch();
  void DisableWatch();

  void HandleQuote( const Quote& );
  void HandleTrade( const Trade& );

  void HandleIQFeedFundamentalMessage( ou::tf::iqfeed::IQFeedSymbol::pFundamentals_t );
  void HandleIQFeedSummaryMessage( ou::tf::iqfeed::IQFeedSymbol::pSummary_t );

  void HandleTimeSeriesAllocation( Trades::size_type count );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    //ar & boost::serialization::base_object<const InstrumentInfo>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    //ar & boost::serialization::base_object<InstrumentInfo>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

} // namespace tf
} // namespace ou

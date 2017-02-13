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

#include <boost/smart_ptr.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <OUCommon/Delegate.h>

#include <TFTimeSeries/TimeSeries.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderInterface.h>

#include <TFIQFeed/IQFeedSymbol.h>

// 20151228 convert the delegate to a signal?  a little slower maybe.

// 20160122 will want to set signals on provider so watch/unwatch as provider transitions connection states

namespace ou { // One Unified
namespace tf { // TradeFrame

class Watch {
public:

  typedef boost::shared_ptr<Watch> pWatch_t;
  typedef Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;

  struct Fundamentals_t {
    double dblHistoricalVolatility;
    int nShortInterest;
    double dblPriceEarnings;
    double dbl52WkHi;
    double dbl52WkLo;
    double dblDividendAmount;
    double dblDividendRate;
    double dblDividendYield;
    date dateExDividend;
  };

  struct Summary_t {
    int nOpenInterest;
    int nTotalVolume;
    double dblOpen;
  };

  Watch( pInstrument_t pInstrument, pProvider_t pDataProvider );
  Watch( const Watch& rhs );
  virtual ~Watch(void);

  virtual Watch& operator=( const Watch& rhs );

  bool operator< ( const Watch& rhs ) const { return m_pInstrument->GetInstrumentName() <  rhs.m_pInstrument->GetInstrumentName(); };
  bool operator<=( const Watch& rhs ) const { return m_pInstrument->GetInstrumentName() <= rhs.m_pInstrument->GetInstrumentName(); };

  pInstrument_t GetInstrument( void ) { return m_pInstrument; };
  
  void SetProvider( pProvider_t pDataProvider );
  pProvider_t GetProvider( void ) { return m_pDataProvider; };

  bool Watching( void ) const { return 0 != m_cntWatching; };

  const Quote& LastQuote( void ) const { return m_quote; };  // may have thread sync issue
  const Trade& LastTrade( void ) const { return m_trade; };  // may have thread sync issue

  const Fundamentals_t& Fundamentals( void ) const { return m_fundamentals; };
  const Summary_t& Summary( void ) const { return m_summary; };

  const Quotes& GetQuotes( void ) const { return m_quotes; };
  const Trades& GetTrades( void ) const { return m_trades; };

  ou::Delegate<const Quote&> OnQuote;
  ou::Delegate<const Trade&> OnTrade;
  
  //typedef std::pair<size_t,size_t> stateTimeSeries_t;
  //ou::Delegate<const stateTimeSeries_t&> OnPossibleResizeBegin;
  //ou::Delegate<const stateTimeSeries_t&> OnPossibleResizeEnd;

  virtual void StartWatch( void );
  virtual bool StopWatch( void );

  virtual void EmitValues( void ) const;

  virtual void SaveSeries( const std::string& sPrefix );

protected:

  // use an iterator instead?  or keep as is as it facilitates multi-thread append and access operations
  // or will the stuff in TBB help with this type of access?

  ou::tf::Quote m_quote;
  ou::tf::Trade m_trade;

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  pInstrument_t m_pInstrument;

  pProvider_t m_pDataProvider;

  std::stringstream m_ss;

  unsigned int m_cntWatching;
  
private:

  bool m_bWatchingEnabled;
  bool m_bWatching; // in/out of connected state
  

  Fundamentals_t m_fundamentals;
  Summary_t m_summary;
  
  void Initialize( void );
  
  void AddEvents( void );
  void RemoveEvents( void );
  
  void HandleConnecting( int );
  void HandleConnected( int );
  void HandleDisconnecting( int );
  void HandleDisconnected( int );
  
  void EnableWatch();
  void DisableWatch();

  void HandleQuote( const Quote& quote );
  void HandleTrade( const Trade& trade );

  void HandleIQFeedFundamentalMessage( ou::tf::IQFeedSymbol& symbol );
  void HandleIQFeedSummaryMessage( ou::tf::IQFeedSymbol& symbol );
  
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

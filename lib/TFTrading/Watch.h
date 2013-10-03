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

#include <OUCommon/Delegate.h>

#include <TFTimeSeries/TimeSeries.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderInterface.h>

#include <TFIQFeed/IQFeedSymbol.h>

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

  bool Watching( void ) { return 0 != m_cntWatching; };

  const Quote& LastQuote( void ) const { return m_quote; };  // may have thread sync issue
  const Trade& LastTrade( void ) const { return m_trade; };  // may have thread sync issue

  const Fundamentals_t& Fundamentals( void ) const { return m_fundamentals; };
  const Summary_t& Summary( void ) const { return m_summary; };

  Quotes* Quotes( void ) { return &m_quotes; };
  Trades* Trades( void ) { return &m_trades; };

  ou::Delegate<const Quote&> OnQuote;
  ou::Delegate<const Trade&> OnTrade;

  virtual void StartWatch( void );
  virtual bool StopWatch( void );

  virtual void EmitValues( void );

  virtual void SaveSeries( const std::string& sPrefix );

protected:

  // use an interator instead?  or keep as is as it facilitates multithread append and access operations
  // or will the stuff in TBB help with this type of access?

  ou::tf::Quote m_quote;
  ou::tf::Trade m_trade;

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  pInstrument_t m_pInstrument;

  pProvider_t m_pDataProvider;

  std::stringstream m_ss;

  unsigned int m_cntWatching;  // maybe implement counter at some point to allow multiple calls

private:

  Fundamentals_t m_fundamentals;
  Summary_t m_summary;

  void Initialize( void );

  void HandleQuote( const Quote& quote );
  void HandleTrade( const Trade& trade );

  void HandleIQFeedFundamentalMessage( ou::tf::IQFeedSymbol& symbol );
  void HandleIQFeedSummaryMessage( ou::tf::IQFeedSymbol& symbol );

};

} // namespace tf
} // namespace ou

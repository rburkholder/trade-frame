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
    double dblDividendYield;
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

  double Bid( void ) const { return m_dblBid; };
  double Ask( void ) const { return m_dblAsk; };
  double Price( void ) const { return m_dblPrice; };

  Quotes* Quotes( void ) { return &m_quotes; };
  Trades* Trades( void ) { return &m_trades; };

  virtual void StartWatch( void );
  virtual bool StopWatch( void );

  virtual void SaveSeries( const std::string& sPrefix );

protected:

  // use an interator instead?  or keep as is as it facilitates multithread append and access operations
  // or will the stuff in TBB help with this type of access?
  double m_dblBid;
  double m_dblAsk;
  double m_dblPrice;

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

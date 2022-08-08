/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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

/*
 * File:      Server_impl.hpp
 * Author:    raymond@burkholder.net
 * Project:   TableTrader
 * Created:   2022/08/03 17:14:41
 */

#pragma once

#include <mutex>
#include <memory>
#include <string>
#include <unordered_map>

#include <TFTrading/Watch.h>
#include <TFTrading/Portfolio.h>
#include <TFTrading/Instrument.h>

#include <TFOptions/Chain.h>
#include <TFOptions/Chains.h>

namespace ou {
namespace tf {
  class db;
  class BuildInstrument;
namespace iqfeed {
  class IQFeedProvider;
  class OptionChainQuery;
} // namespace iqfeed
namespace ib {
  class TWS;
} // namespace ib
} // namespace tf
} // namespace ou

class Server_impl {
public:

  Server_impl();
  ~Server_impl();

  using fUpdateUnderlyingInfo_t = std::function<void(const std::string&,int)>; // generic name, contract size
  using fUpdateUnderlyingPrice_t = std::function<void(double,int)>; // price,precision
  using fAddExpiry_t = std::function<void(boost::gregorian::date)>;
  using fAddExpiryDone_t = std::function<void()>;

  using fPopulateStrike_t = std::function<void(double,int)>; // strike, precision
  using fPopulateStrikeDone_t = std::function<void()>;

  void SessionAttach( const std::string& sSessionId );
  void SessionDetach( const std::string& sSessionId );

  void Start(
    const std::string& sUnderlyingFuture,
    fUpdateUnderlyingInfo_t&&,
    fUpdateUnderlyingPrice_t&&,
    fAddExpiry_t&&,
    fAddExpiryDone_t&&
    );

  void PopulateStrikes(
    boost::gregorian::date,
    fPopulateStrike_t&&,
    fPopulateStrikeDone_t&&
  );

  void TriggerUpdates();

  const std::string& Ticker( ou::tf::OptionSide::EOptionSide, double ) const;

  void AddStrike( double );
  void DelStrike( double );

  void ChangeInvestment( double dblInvestment );
  void ChangeAllocation( double dblStrike, double dblPercent );

  void PlaceOrders();
  void CancelAll();
  void CloseAll();

protected:
private:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;
  using pOption_t = ou::tf::option::Option::pOption_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;
  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  enum EConnection { quiescent, connecting, connected, disconnecting, disconnected };
  EConnection m_state;

  std::mutex m_mutex;

  std::shared_ptr<ou::tf::ib::TWS> m_pProviderTWS;
  std::shared_ptr<ou::tf::iqfeed::IQFeedProvider> m_pProviderIQFeed;

  std::unique_ptr<ou::tf::db> m_pdb;

  std::unique_ptr<ou::tf::BuildInstrument> m_pBuildInstrumentBoth;
  std::unique_ptr<ou::tf::BuildInstrument> m_pBuildInstrumentIQFeed;

  pWatch_t m_pWatchUnderlying;

  pPortfolio_t m_pPortfolio;

  fUpdateUnderlyingInfo_t m_fUpdateUnderlyingInfo;
  fUpdateUnderlyingPrice_t m_fUpdateUnderlyingPrice;
  fAddExpiry_t m_fAddExpiry;
  fAddExpiryDone_t m_fAddExpiryDone;

  int m_nPrecision;
  unsigned int m_nMultiplier; // used to populate futures options multiplier (not supplied by iqf)

  ou::tf::Quote m_quoteUnderlying;
  ou::tf::Trade m_tradeUnderlying;

  struct BuiltOption: public ou::tf::option::chain::OptionName {
    pOption_t pOption;
  };

  using chain_t = ou::tf::option::Chain<BuiltOption>;
  using mapChains_t = std::map<boost::gregorian::date, chain_t>;
  mapChains_t m_mapChains;
  mapChains_t::const_iterator m_citerChains; // chosen date

  using pOptionChainQuery_t = std::shared_ptr<ou::tf::iqfeed::OptionChainQuery>;
  pOptionChainQuery_t m_pOptionChainQuery;

  size_t m_nOptionsNames;
  size_t m_nOptionsLoaded;

  double m_dblInvestment;

  struct Session {
  };

  using mapSession_t = std::unordered_map<std::string,Session>;
  mapSession_t m_mapSession;

  void Connected_IQFeed( int );
  void Connected_TWS( int );
  void Connected( int );

  void Disconnected_IQFeed( int );
  void Disconnected_TWS( int );
  void Disconnected( int );

  void UnderlyingInitialize( pInstrument_t );

  void InstrumentToOption( pInstrument_t );
  void PopulateExpiry();

  void UnderlyingFundamentals( const ou::tf::Watch::Fundamentals& );
  void UnderlyingQuote( const ou::tf::Quote& );
  void UnderlyingTrade( const ou::tf::Trade& );
};
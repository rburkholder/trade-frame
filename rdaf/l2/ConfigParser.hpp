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
 * File:    ConfigParser.hpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: March 15, 2022 12:56
 */

#pragma once

#include <map>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace ou {
namespace tf {
namespace config {

struct symbol_t {

  enum EFeed {
    L1   // L1 quotes/trades
  , L2M  // L1 + L2 market maker (nasdaq equities)
  , L2O  // L1 + L2 orders (CME, ICE futures)
  };

  // parsed direct from config file

  EFeed eFeed;
  bool bTradable;

  std::string sAlgorithm;
  std::string sSignalFrom; // link one instrument to another

  size_t nPriceBins;
  double dblPriceUpper;
  double dblPriceLower;

  size_t nVolumeBins;
  size_t nVolumeUpper;
  size_t nVolumeLower;

  // supplied after parsing

  std::string sSymbol;

  size_t nTimeBins;
  double dblTimeUpper;
  double dblTimeLower;

  symbol_t()
  : eFeed( EFeed::L1 ) // default
  , bTradable( true )  // default
  {} // optional for now

};

struct choices_t {

  bool bStartSimulator;
  std::string sGroupDirectory;

  size_t nThreads;

  size_t ib_client_id;

  std::string m_sAlpacaKey;
  std::string m_sAlpacaSecret;
  std::string m_sAlpacaDomain; // paper-api.alpaca.markets or api.alpaca.markets

  size_t nTimeBins;

  std::string sTimeUpper; // string from file
  boost::posix_time::ptime dtUpper; // convert to ptime
  double dblTimeUpper; // convert to double for use in rdaf

  std::string sTimeLower;
  boost::posix_time::ptime dtLower;
  double dblTimeLower;

  using mapInstance_t = std::map<std::string,symbol_t>; // std::string = symbol name
  mapInstance_t mapInstance;

  void Update() {

    std::time_t nTime;

    dtUpper = boost::posix_time::from_iso_string( sTimeUpper );
    nTime = boost::posix_time::to_time_t( dtUpper );
    dblTimeUpper = (double) nTime / 1000.0;

    dtLower = boost::posix_time::from_iso_string( sTimeLower );
    nTime = boost::posix_time::to_time_t( dtLower );
    dblTimeLower = (double) nTime / 1000.0;

    for ( mapInstance_t::value_type& vt: mapInstance ) {

      symbol_t& def( vt.second );
      def.sSymbol = vt.first;
      def.nTimeBins = nTimeBins;
      def.dblTimeUpper = dblTimeUpper;
      def.dblTimeLower = dblTimeLower;

    }
  }

};

bool Load( const std::string&, choices_t& );

} // namespace config
} // namespace tf
} // namespace ou

/* config file format:

time_bins=3600
#          yyyymmddThhmmss
time_upper=20220314T000000
time_lower=20220315T000000

#group_directory=/app/AutoTrade/20220223 20:05:28.214488-1
sim_start=off

[SPY]
price_bins=200
price_upper=200
price_lower=400
volume_bins= 100
volume_upper=10000
volume_lower=0

*/
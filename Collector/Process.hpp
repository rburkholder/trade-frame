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
 * File:    Process.hpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: October 20, 2022 21:07:40
 */

// inspiration from Dividend project

#pragma once

#include <set>
//#include <mutex>
#include <functional>
#include <unordered_map>

#include <TFTrading/Watch.h>

#include <TFOptions/Chains.h>
#include <TFOptions/Option.h>
#include <TFOptions/Engine.h>
#include <TFOptions/NoRiskInterestRateSeries.h>

#include <TFIQFeed/Provider.h>
#include <TFIQFeed/OptionChainQuery.h>

#include "Config.hpp"

namespace ou {
namespace tf {
  class ComposeInstrument;
} // namespace tf
} // namespace ou

namespace collect {
  class L1;
  class L2;
  class Greeks;
  class ATM;
}

class Process {
public:

  Process(
    const config::Choices&
  , const std::string& sTimeStamp
  );
  ~Process();

  void Write(); // incremental write

protected:
private:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  using fWatch_t = std::function<void(pWatch_t)>;

  enum class EToCollect { L1, L2, Greeks, ATM };
  using setToCollect_t = std::set<EToCollect>;

  const std::string m_sPathName;

  const config::Choices& m_choices;

  //std::mutex m_mutexScope_ConstructWatch;

  using pIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;
  pIQFeed_t m_piqfeed;

  std::unique_ptr<ou::tf::ComposeInstrument> m_pComposeInstrumentIQFeed;

  struct ToCollect {
    setToCollect_t setToCollect;
    pWatch_t pWatch;

    ToCollect( EToCollect e ) {
      setToCollect.emplace( e );
      }
  };

  using mapToCollect_t = std::unordered_map<std::string, ToCollect>;
  mapToCollect_t m_mapToCollect;

  using pCollectL1_t = std::unique_ptr<collect::L1>;
  using mapCollectL1_t = std::unordered_map<std::string, pCollectL1_t>;
  mapCollectL1_t m_mapCollectL1;

  using pCollectL2_t = std::unique_ptr<collect::L2>;
  using mapCollectL2_t = std::unordered_map<std::string, pCollectL2_t>;
  mapCollectL2_t m_mapCollectL2;

  using pCollectGreeks_t = std::unique_ptr<collect::Greeks>;
  using mapCollectGreeks_t = std::unordered_map<std::string, pCollectGreeks_t>;
  mapCollectGreeks_t m_mapCollectGreeks;

  using pCollectATM_t = std::unique_ptr<collect::ATM>;
  using mapCollectATM_t = std::unordered_map<std::string, pCollectATM_t>;
  mapCollectATM_t m_mapCollectATM;

  ou::tf::FedRateFromIQFeed m_fedrate;
  std::unique_ptr<ou::tf::option::Engine> m_pOptionEngine;
  std::unique_ptr<ou::tf::iqfeed::OptionChainQuery> m_pOptionChainQuery; // need to disconnect

  void QueryChains( pInstrument_t ); // underlying

  void StartIQFeed();
  void HandleIQFeedConnected( int );
  void InitializeComposeInstrument();
  void ConstructWatches();
  void ConstructCollectors( const setToCollect_t&, pWatch_t );
  void ConstructCollectorL1( pWatch_t );
  void ConstructCollectorL2( pWatch_t );
  void ConstructCollectorGreeks( pOption_t );
  void ConstructCollectorATM( pWatch_t );
};

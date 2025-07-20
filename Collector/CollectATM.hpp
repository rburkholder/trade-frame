/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    CollectATM.hpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 19, 2025 14:06:45
 */

#pragma once

#include <map>
#include <unordered_map>

#include <TFTrading/Instrument.h>

#include <TFOptions/Chain.h>
#include <TFOptions/Option.h>

#include "Collect.hpp"
#include "FillWrite.hpp"

namespace collect {

class ATM: public Base {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOption_t = ou::tf::option::Option::pOption_t;
  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  using fBuildOption_t = std::function<pOption_t(pInstrument_t)>;
  using fInstrumentOption_t = std::function<void(std::size_t, pInstrument_t /* option */)>;
  using fGatherOptions_t = std::function<void(pInstrument_t /* underlying */, fInstrumentOption_t&&)>;
  using fEngine_t = std::function<void( pOption_t&, pWatch_t& )>;  // start / stop watch in option engine

  ATM(
    const std::string& sFilePath,
    const std::string& sDataPathPrefix, pWatch_t /* underlying */,
    fBuildOption_t&&, fGatherOptions_t&&,
    fEngine_t&& start, fEngine_t&& stop,
    boost::gregorian::date dateStop // use for expiry calculation
    );
  ~ATM();

  void Write() override; // incremental write

protected:
private:

  pWatch_t m_pWatchUnderlying;

  fGatherOptions_t m_fGatherOptions;
  fBuildOption_t m_fBuildOption;

  using fwATM_t = ou::tf::FillWrite<ou::tf::PriceIVs>;
  std::unique_ptr<fwATM_t> m_pfwATM;

  struct Instance: public ou::tf::option::chain::OptionName {
    pInstrument_t pInstrument; // resident in all Options
    pOption_t pOption;  // includes Watch, just-in-time Watch construction
  };

  using chain_t = ou::tf::option::Chain<Instance>;
  using mapChains_t = std::map<boost::gregorian::date, chain_t>;
  mapChains_t m_mapChains;

  using pTrackATM_t = std::unique_ptr<chain_t::TrackATM>;
  pTrackATM_t m_pTrackATM;

  fEngine_t m_fEngineOptionStart;
  fEngine_t m_fEngineOptionStop;

  // track options used in engine for destructor cleanup
  struct LifeTime {
    size_t count;
    pOption_t pOption;
    LifeTime( pOption_t pOption_ )
    : count( 1 ), pOption( pOption_ )
    {}
  };
  using mapOptionLifeTime_t = std::unordered_map<std::string,LifeTime>;
  mapOptionLifeTime_t m_mapOptionLifeTime;

  void MapAddOption( pOption_t );
  void MapDelOption( pOption_t );

  void HandleWatchUnderlyingTrade( const ou::tf::Trade& );

};

} // namespace collect

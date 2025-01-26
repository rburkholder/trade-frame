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

#include <TFTrading/Instrument.h>

#include <TFOptions/Chain.h>
//#include <TFOptions/Option.h>

#include "Collect.hpp"
#include "FillWrite.hpp"

namespace collect {

class ATM: public Base {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  ATM( const std::string& sPathPrefix, pWatch_t );  // underlying
  ~ATM();

  void Write() override; // incremental write

protected:
private:

  pWatch_t m_pWatchUnderlying;

  using fwATM_t = ou::tf::FillWrite<ou::tf::Greeks>;
  std::unique_ptr<fwATM_t> m_pfwATM;

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  struct Instance: public ou::tf::option::chain::OptionName {
    pInstrument_t pInstrument; // resident in all Options
    pWatch_t pWatch; // resident only for just-in-time Watch construction
  };

  using chain_t = ou::tf::option::Chain<Instance>;
  using mapChains_t = std::map<boost::gregorian::date, chain_t>;
  mapChains_t m_mapChains;

  void HandleWatchGreeksPut( const ou::tf::Greek& );
  void HandleWatchGreeksCall( const ou::tf::Greek& );
};

} // namespace collect

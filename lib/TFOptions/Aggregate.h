/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    Aggregate.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on May 29, 2021, 20:09
 */

 // Testing / Proof of concept that GEX might be useful in some capacity
 // Relies on iqfeed trade-only message, reduces traffic, therefore more symbols available
 // the problem:  needs recent quotes to recalculate greeks at current pricing level
 //    might be ok, f<symbol> returns F, and P, responses, with P having the latest quote

 // TODO: attempt to understand option combinations transacted (if transactions are sparse)

#pragma once

#include <TFTimeSeries/DatedDatum.h>
#include <TFTimeSeries/TimeSeries.h>

#include <TFOptions/Option.h>

#include "Chain.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Aggregate {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  using fOption_t = std::function<void(pOption_t)>; // incrementally obtain built options
  using fGatherOptions_t = std::function<void(const std::string&, fOption_t&&)>; // request by IQFeed Symbol Name

  Aggregate(
    pWatch_t pWatchUnderlying
  );

  void LoadChains( fGatherOptions_t&& ); // start in constructor?

  // TODO:
  //   constructor needs engine add/remove
  //   will require registration to P message for current quote

protected:
private:

  using volume_t = ou::tf::DatedDatum::volume_t;

  pWatch_t m_pWatchUnderlying;

  struct OptionWithStats: public ou::tf::option::chain::OptionName {
    // should volumes net out?  seems they will monotonically increase
    //   without input from open interest numbers.  does it matter?
    // gamma calculation, serialize buy/sell for inter-session updates
    volume_t sell; // total sell side options for gex calc
    volume_t buy;  // total buy side options for gex calc
    pOption_t pOption; // might as well keep the fully decorated option around as well
    OptionWithStats()
    : sell {}, buy {} {}
  };

  using chain_t = ou::tf::option::Chain<OptionWithStats>;
  using mapChains_t = std::map<boost::gregorian::date, chain_t>;
  using mapChains_iterator_t = mapChains_t::iterator;
  mapChains_t m_mapChains;

};

} // namespace option
} // namespace tf
} // namespace ou

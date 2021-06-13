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

#include <TFTrading/Watch.h>

#include "Chain.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Aggregate {
  using volume_t = ou::tf::DatedDatum::volume_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
public:
  //Aggregate() {}
  Aggregate(
    pWatch_t pWatchUnderlying
  );
  // TODO:
  //   constructor needs engine add/remove, underlying, option construction (no IB contract, no registration)
  //   iqfeed sybmol will require trade-only watch request
  //   will require registration to P message for current quote
protected:
private:

  pWatch_t m_pWatchUnderlying;

  struct OptionStats: public ou::tf::option::chain::OptionName {
    // should volumes net out?  seems they will monotonically increase
    //   without input from open interest numbers.  does it matter?
    volume_t sell;
    volume_t buy;
    ou::tf::Quotes quotes;
    ou::tf::Trades trades;
    ou::tf::Greeks greeks;
  };

  using chain_t = ou::tf::option::Chain<OptionStats>;
  using mapChains_t = std::map<boost::gregorian::date, chain_t>;
  mapChains_t m_mapChains;

};

} // namespace option
} // namespace tf
} // namespace ou

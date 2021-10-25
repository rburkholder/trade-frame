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
 * File:    Chains.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on May 29, 2021, 21:51
 */

#pragma once

#include <string>
#include <iostream>

#include <boost/date_time/gregorian/greg_date.hpp>

#include "GatherOptions.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

template<typename mapChains_t>
void PopulateMap( mapChains_t& map, const std::string& sUnderlying, fGatherOptions_t& f ) {
  f(
    sUnderlying,
    [&map](pOption_t pOption){  // these are iqfeed based symbol names

        using chain_t = typename mapChains_t::mapped_type;
        using iterator_t = typename mapChains_t::iterator;

        const boost::gregorian::date date( pOption->GetExpiry() );

        iterator_t iterChains;

        { // find existing expiry, or create new one
          chain_t chain;

          iterChains = map.find( date ); // see if expiry date exists
          if ( map.end() == iterChains ) { // insert new expiry set if not
            std::cout
              << "PopulateMap chain: "
              << pOption->GetInstrumentName() << ","
              << date.year() << "/"
              << date.month().as_number() << "/"
              << date.day()
              << std::endl;
            iterChains = map.insert(
              map.begin(),
              typename mapChains_t::value_type( date, std::move( chain ) )
              );
          }
        }

        { // populate new call or put, no test for pre-existance

          chain_t& chain( iterChains->second );

          //std::cout << "  option: " << row.sSymbol << std::endl;

          try {
            switch ( pOption->GetOptionSide() ) {
              case ou::tf::OptionSide::Call:
                chain.SetIQFeedNameCall( pOption->GetStrike(), pOption->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );
                break;
              case ou::tf::OptionSide::Put:
                chain.SetIQFeedNamePut( pOption->GetStrike(), pOption->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );
                break;
            }
          }
          catch ( std::runtime_error& e ) {
            std::cout << "PopulateMap::fGatherOptionDefinitions error" << std::endl;
          }
        }
  });
}

} // namespace option
} // namespace tf
} // namespace ou

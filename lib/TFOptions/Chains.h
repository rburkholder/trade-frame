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

#include <boost/log/trivial.hpp>

#include <boost/date_time/gregorian/greg_date.hpp>

#include "GatherOptions.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

// for reference:
//    using fGatherOptions_t = std::function<void(const std::string& sUnderlying, fOption_t&&)>;

template<typename mapChains_t>
typename mapChains_t::iterator GetChain( mapChains_t& map, pOption_t pOption ) { // find existing expiry, or create new one

  using chain_t = typename mapChains_t::mapped_type;
  using iterator_t = typename mapChains_t::iterator;

  chain_t chain; // default chain for insertion into new entry

  const boost::gregorian::date date( pOption->GetExpiry() );

  iterator_t iterChains = map.find( date ); // see if expiry date exists
  if ( map.end() == iterChains ) { // insert new expiry set if not
    BOOST_LOG_TRIVIAL(info)
      << "GetChain created: "
      << date.year() << "/"
      << date.month().as_number() << "/"
      << date.day()
      << " with "
      << pOption->GetInstrumentName()
      ;
    iterChains = map.insert(
      map.begin(),
      typename mapChains_t::value_type( date, std::move( chain ) )
      );
  }
  return iterChains;
}

template<typename chain_t, typename OptionEntry>
OptionEntry* UpdateOption( chain_t& chain, pOption_t pOption ) {

  // populate new call or put, no test for pre-existance
  //std::cout << "  option: " << row.sSymbol << std::endl;

  OptionEntry* pOptionEntry {};

  try {
    switch ( pOption->GetOptionSide() ) {
      case ou::tf::OptionSide::Call:
        {
          OptionEntry& entry( chain.SetIQFeedNameCall( pOption->GetStrike(), pOption->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) ) );
          pOptionEntry = &entry;
        }
        break;
      case ou::tf::OptionSide::Put:
        {
          OptionEntry& entry( chain.SetIQFeedNamePut( pOption->GetStrike(), pOption->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) ) );
          pOptionEntry = &entry;
        }
        break;
      default:
        assert( false );
        break;
    }
  }
  catch ( std::runtime_error& e ) {
    BOOST_LOG_TRIVIAL(error) << "PopulateOption error";
  }
  return pOptionEntry;
}

template<typename mapChains_t> // used for populating option names into a default chain
void PopulateMap( mapChains_t& map, const std::string& sUnderlying, fGatherOptions_t&& fGatherOptions ) {
  fGatherOptions(
    sUnderlying,
    [&map](pOption_t pOption){  // these are iqfeed based symbol names

        using chain_t = typename mapChains_t::mapped_type;
        using iterator_t = typename mapChains_t::iterator;
        using OptionEntry = typename chain_t::option_t;

        iterator_t iterChains = GetChain( map, pOption );
        UpdateOption<chain_t,OptionEntry>( iterChains->second, pOption );

  });
}

} // namespace option
} // namespace tf
} // namespace ou

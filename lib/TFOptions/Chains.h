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

#include "Exceptions.h"
#include "GatherOptions.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

// for reference:
//    using fGatherOptions_t = std::function<void(const std::string& sUnderlying, fOption_t&&)>;

// TODO: redo or add another entry for pInstrument_t
template<typename mapChains_t>
typename mapChains_t::iterator GetChain( mapChains_t& map, pOption_t pOption ) { // find existing expiry, or create new one

  using chain_t = typename mapChains_t::mapped_type;
  using iterator_t = typename mapChains_t::iterator;

  chain_t chain; // default chain for insertion into new entry

  const boost::gregorian::date expiry( pOption->GetExpiry() );

  iterator_t iterChains = map.find( expiry ); // see if expiry date exists
  if ( map.end() == iterChains ) { // insert new expiry set if not
    const std::string& sInstrumentName( pOption->GetInstrumentName() );
    const std::string& sIQFeedSymbolName( pOption->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );
    BOOST_LOG_TRIVIAL(info)
      << "GetChain created: "
      << ou::tf::Instrument::BuildDate( expiry )
      << ',' << sIQFeedSymbolName
      << ',' << sInstrumentName
      ;
    auto result = map.emplace( expiry, std::move( chain ) );
    assert( result.second );
    iterChains = result.first;
  }
  return iterChains;
}

// TODO: redo or add another entry for pInstrument_t
template<typename chain_t, typename OptionEntry>
OptionEntry* UpdateOption( chain_t& chain, pOption_t pOption ) {

  // populate new call or put, no test for pre-existance
  //std::cout << "  option: " << row.sSymbol << std::endl;

  OptionEntry* pOptionEntry( nullptr );  // the put/call object at the strike
  const std::string& sIQFeedSymbolName( pOption->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

  try {
    switch ( pOption->GetOptionSide() ) {
      case ou::tf::OptionSide::Call:
        {
          OptionEntry& entry( chain.SetIQFeedNameCall( pOption->GetStrike(), sIQFeedSymbolName ) );
          pOptionEntry = &entry;
        }
        break;
      case ou::tf::OptionSide::Put:
        {
          OptionEntry& entry( chain.SetIQFeedNamePut( pOption->GetStrike(), sIQFeedSymbolName ) );
          pOptionEntry = &entry;
        }
        break;
      default:
        assert( false );
        break;
    }
  }
  catch ( std::runtime_error& e ) {
    BOOST_LOG_TRIVIAL(error) << "option::UpdateOption error";
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
        chain_t& chain( iterChains->second );
        UpdateOption<chain_t,OptionEntry>( chain, pOption );

  });
}

template<typename mapChains_t>
static typename mapChains_t::const_iterator SelectChain( const mapChains_t& mapChains, boost::gregorian::date date, boost::gregorian::days daysToExpiry ) {
  typename mapChains_t::const_iterator citerChain = std::find_if( mapChains.begin(), mapChains.end(),
    [date,daysToExpiry](const typename mapChains_t::value_type& vt)->bool{
      return daysToExpiry <= ( vt.first - date );  // first chain where trading date less than expiry date
  } );
  if ( mapChains.end() == citerChain ) {
    throw ou::tf::option::exception_chain_not_found( "option::SelectChain" );
  }
  return citerChain;
}

} // namespace option
} // namespace tf
} // namespace ou

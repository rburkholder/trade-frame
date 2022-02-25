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
 * File:    Aggregate.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on May 29, 2021, 20:09
 */

#include "Aggregate.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

Aggregate::Aggregate(
  pWatch_t pWatchUnderlying
)
: m_pWatchUnderlying( pWatchUnderlying )
{
}

void Aggregate::LoadChains( fGatherOptions_t&& fGatherOptions ) {
  fGatherOptions(
    m_pWatchUnderlying->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ),
    [this]( pOption_t pOption ){

      mapChains_iterator_t iterChains;

      { // find existing expiry, or create new one

        chain_t chain;

        const boost::gregorian::date expiry( pOption->GetExpiry() );

        const std::string& sIQFeedSymbolName( pOption->GetInstrumentName() );

        iterChains = m_mapChains.find( expiry ); // see if expiry date exists
        if ( m_mapChains.end() == iterChains ) { // insert new expiry set if not
          std::cout
            << "Aggregate chain: " << sIQFeedSymbolName
            << "," << expiry.year()
            << "/" << expiry.month().as_number()
            << "/" << expiry.day()
            << std::endl;
          iterChains = m_mapChains.insert(
            m_mapChains.begin(),
             mapChains_t::value_type( expiry, std::move( chain ) )
            );
        }
      }

      { // populate new call or put, no test for pre-existance

        //std::cout << "  option: " << row.sSymbol << std::endl;

        chain_t& chain( iterChains->second );
        chain_t::strike_t& strike( chain.GetStrike( pOption->GetStrike() ) );

        const std::string& sIQFeedSymbolName( pOption->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

        try {
          switch ( pOption->GetOptionSide() ) {
            case ou::tf::OptionSide::Call:
              chain.SetIQFeedNameCall( pOption->GetStrike(), sIQFeedSymbolName );
              strike.call.pOption = pOption;
              break;
            case ou::tf::OptionSide::Put:
              chain.SetIQFeedNamePut( pOption->GetStrike(), sIQFeedSymbolName );
              strike.put.pOption = pOption;
              break;
          }
        }
        catch ( std::runtime_error& e ) {
          std::cout << "LoadChains::fGatherOptions error" << std::endl;
        }

      }
    }
  );
}

void Aggregate::WalkChains( fOption_t&& fOption ) const {
  for ( const mapChains_t::value_type& vt: m_mapChains ) {
    const chain_t& chain( vt.second );
    chain.Strikes(
      [ fOption ]( double strike, const chain_t::strike_t& options ){
        if ( options.call.pOption ) fOption( options.call.pOption );
        if ( options.put.pOption ) fOption( options.put.pOption );
      });
  }
}

void Aggregate::WalkChain( boost::gregorian::date date, fOption_t&& fOption ) const {
  mapChains_t::const_iterator iter = m_mapChains.find( date );
  if ( m_mapChains.end() == iter ) {
    throw std::runtime_error( "Aggregate::WalkChain: date not found" );
  }
  else {
    iter->second.Strikes(
      [ fOption = std::move( fOption ) ]( double strike, const chain_t::strike_t& options ){
        fOption( options.call.pOption );
        fOption( options.put.pOption );
      } );
  }
}


} // namespace option
} // namespace tf
} // namespace ou

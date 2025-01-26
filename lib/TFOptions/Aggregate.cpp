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

#include <stdexcept>

#include <TFOptions/Chains.h>

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
  std::cout << "Aggregate::LoadChains" << std::endl;
  fGatherOptions(
    m_pWatchUnderlying->GetInstrument(),
    [this]( pOption_t pOption ){

      pInstrument_t pInstrument( pOption->GetInstrument() );

      // find existing expiry, or create new one
      mapChains_iterator_t iterChains;
      iterChains = ou::tf::option::GetChain( m_mapChains, pInstrument );

      // update put/call@strike with option
      chain_t& chain( iterChains->second );
      OptionWithStats* pEntry
        = ou::tf::option::UpdateOption<chain_t,OptionWithStats>( chain, pInstrument );
      pEntry->pOption = pOption; // put / call as appropriate

    }
  );
}

void Aggregate::FilterChains() {

  assert( 0 < m_mapChains.size() );

  std::vector<boost::gregorian::date> vChainsToBeRemoved;

  size_t nStrikesSum {};
  for ( const mapChains_t::value_type& vt: m_mapChains ) { // only use chains where all calls/puts available

    size_t nStrikesTotal {};
    size_t nStrikesMatch {};

    struct MisMatched {
      double strike;
      std::string side;
      MisMatched( double strike_, const std::string& side_ ): strike( strike_ ), side( std::move( side_ ) ) {}
    };

    std::vector<MisMatched> vMisMatch;
    vMisMatch.reserve( 10 );

    vt.second.Strikes(
      [&nStrikesTotal,&nStrikesMatch,&vMisMatch]( double strike, const chain_t::strike_t& options ){
        nStrikesTotal++;
        if ( options.call.sIQFeedSymbolName.empty() || options.put.sIQFeedSymbolName.empty() ) {
          if ( options.call.sIQFeedSymbolName.empty() ) {
            vMisMatch.emplace_back( MisMatched( strike, "C" ) );
          }
          if ( options.put.sIQFeedSymbolName.empty() ) {
            vMisMatch.emplace_back( MisMatched( strike, "P" ) );
          }
        }
        else {
          nStrikesMatch++;
        }
    } );

    const std::string sStrikeDate( ou::tf::Instrument::BuildDate( vt.first ) );

    bool bChainAdded( true );
    if ( nStrikesTotal == nStrikesMatch ) {
      nStrikesSum += nStrikesTotal;
      std::cout << "chain " << sStrikeDate << " added with " << nStrikesTotal << " strikes" << std::endl;
    }
    else {
      if ( 0 == nStrikesMatch ) {
        std::cout << "chain " << sStrikeDate << " skipped with " << nStrikesMatch << '/' << nStrikesTotal << " strikes" << std::endl;
        vChainsToBeRemoved.push_back( vt.first );
        bChainAdded = false;
      }
      else {
        std::cout
          << "chain " << sStrikeDate << " added " << nStrikesMatch << " strikes without";
        for ( const MisMatched& entry: vMisMatch ) {
          std::cout << " " << entry.side << entry.strike;
          const_cast<chain_t&>( vt.second ).Erase( entry.strike );
        }
        std::cout << std::endl;
        assert( 0 < vt.second.Size() );
      }
    }
  }

  assert( vChainsToBeRemoved.size() != m_mapChains.size() );
  for ( auto date: vChainsToBeRemoved ) { // remove chains with incomplete info
    mapChains_t::iterator iter = m_mapChains.find( date );
    m_mapChains.erase( iter );
  }

  size_t nAverageStrikes = nStrikesSum / m_mapChains.size();
  std::cout << "chain size average: " << nAverageStrikes << std::endl;

}

void Aggregate::WalkChains( fDate_t&& fDate ) const {
  for ( const mapChains_t::value_type& vt: m_mapChains ) {
    fDate( vt.first );
  }
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

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
 * File:    ComposeInstrument.cpp
 * Author:  raymond@burkholder.net
 * Project: TFTrading
 * Created: 2022/11/14 10:37:56
 */

#include <TFIQFeed/OptionChainQuery.h>

#include "ComposeInstrument.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

// instrument composition as IQFeed only
ComposeInstrument::ComposeInstrument(
  pProviderIQFeed_t pProviderIQFeed
, fInitDone_t&& fInitDone )
: m_pProviderIQFeed( std::move( pProviderIQFeed ) )
, m_fInitDone( std::move( fInitDone ) )
{
  assert( m_pProviderIQFeed ); // should also be connected and running
  assert( m_fInitDone );

  Initialize();
}

// instrument composition as IQFeed with IB contract
ComposeInstrument::ComposeInstrument(
  pProviderIQFeed_t pProviderIQFeed
, pProviderIBTWS_t  pProviderIBTWS
, fInitDone_t&& fInitDone )
: m_pProviderIQFeed( std::move( pProviderIQFeed ) )
, m_pProviderIBTWS(  std::move( pProviderIBTWS ) )
, m_fInitDone( std::move( fInitDone ) )
{
  assert( m_pProviderIQFeed ); // should also be connected and running
  assert( m_pProviderIBTWS ); // should also be connected and running
  assert( m_fInitDone );

  m_pBuildInstrumentBoth = std::make_unique<ou::tf::BuildInstrument>( m_pProviderIQFeed, m_pProviderIBTWS );
  Initialize();
}

ComposeInstrument::~ComposeInstrument() {
  if ( m_pOptionChainQuery ) {
    m_pOptionChainQuery->Disconnect();
    m_pOptionChainQuery.reset();
  }
  m_pBuildInstrumentBoth.reset();
  m_pBuildInstrumentIQFeed.reset();
}

void ComposeInstrument::Initialize() { // part of constructor
  m_pBuildInstrumentIQFeed = std::make_unique<ou::tf::BuildInstrument>( m_pProviderIQFeed );
  ConstructChainQuery();
}

void ComposeInstrument::ConstructChainQuery() { // part of constructor
  if ( m_pOptionChainQuery ) {
    assert( false );
  }
  else {
    m_pOptionChainQuery = std::make_shared<ou::tf::iqfeed::OptionChainQuery>(
      [this](){
        m_fInitDone();
        m_fInitDone = nullptr;
      }
    );
    m_pOptionChainQuery->Connect();
  }
}

void ComposeInstrument::Compose( const std::string& sIQFeedSymbol, fInstrument_t&& fInstrument ) {

  assert( 0 < sIQFeedSymbol.size() );
  bool bTrailingHash = '#' == sIQFeedSymbol.back();

  if ( bTrailingHash ) { // process as continuous future

    pMapQuery_t::iterator iterQuery;
    {
      std::lock_guard<std::mutex> lock( m_mutexMap );
      assert( m_pMapQuery.end() == m_pMapQuery.find( sIQFeedSymbol ) );
      auto result = m_pMapQuery.emplace( sIQFeedSymbol, std::move( Query( std::move( fInstrument ) ) ) );
      assert( result.second );
      iterQuery = result.first;
    }

    m_pBuildInstrumentIQFeed->Queue( // confirm instrument type is a future
      sIQFeedSymbol,
      [this,iterQuery]( pInstrument_t pInstrument, bool bExists ) {

        if ( InstrumentType::Future != pInstrument->GetInstrumentType() ) {
          assert( false ); // needs to be a future, if something else, process accordingly, maybe case statement
        }
        else {
          const std::string& sName( pInstrument->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );
          const std::string sBase( sName.substr( 0, sName.size() - 1 ) ); // remove trailing #
          boost::gregorian::date expiry( pInstrument->GetExpiry() ); // will be matching expiry of this continous future

          // todo: auto-calc the year range
          m_pOptionChainQuery->QueryFuturesChain(  // obtain a list of futures
            sBase, "", "5678" /* 2025, 2026, 2027, 2028 */ , "4" /* 4 months */,
            [this,expiry,iterQuery]( const iqfeed::OptionChainQuery::FuturesList& list ) mutable {

              if ( 0 == list.vSymbol.size() ) {
                assert( false );  // no likely symbols found
                // TODO: clean up and return something
              }
              else {
                Query& query( iterQuery->second );
                query.cntInstrumentsProcessed = list.vSymbol.size();
                for ( const iqfeed::OptionChainQuery::vSymbol_t::value_type sSymbol: list.vSymbol ) {

                  m_pBuildInstrumentIQFeed->Queue(
                    sSymbol,
                    [this,expiry,iterQuery]( pInstrument_t pInstrument, bool bConstructed ){
                      Query& query( iterQuery->second );
                      if ( expiry == pInstrument->GetExpiry() ) {
                        // over-write continuous instrument
                        query.bConstructed = bConstructed;
                        query.pInstrument = pInstrument;
                        // even when found, need to process all remaining arrivals
                      }
                      query.cntInstrumentsProcessed--;
                      if ( 0 == query.cntInstrumentsProcessed ) {
                        assert( query.pInstrument ); // assert we found an instrument
                        Finish( iterQuery );
                      }
                    } );
                }
              }
            }
            );
        }
      }
    );
  }
  else {
    // process as a regular build instrument
    if ( m_pBuildInstrumentBoth ) {
      m_pBuildInstrumentBoth->Queue( sIQFeedSymbol, std::move( fInstrument ) );
    }
    else {
      m_pBuildInstrumentIQFeed->Queue( sIQFeedSymbol, std::move( fInstrument ) );
    }
  }
}

void ComposeInstrument::Finish( pMapQuery_t::iterator iterQuery ) {

  Query& query( iterQuery->second );
  assert( query.pInstrument );

  if ( m_pBuildInstrumentBoth ) {
    // this does create a double query, first from above, and again here, but only on the final resolved future
    const std::string& sName( query.pInstrument->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );
    m_pBuildInstrumentBoth->Queue( sName, std::move( query.fInstrument ) );
  }
  else {
    //m_pBuildInstrumentIQFeed->Queue( sName, std::move( query.fInstrument ) );
    query.fInstrument( query.pInstrument, query.bConstructed );
  }

  {
    std::lock_guard<std::mutex> lock( m_mutexMap );
    m_pMapQuery.erase( iterQuery );
  }
}

} // namespace tf
} // namespace ou

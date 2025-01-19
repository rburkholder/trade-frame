/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    Process.cpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: October 20, 2022 21:07:40
 */

// start watch on l1
// write out at regular intervals

#include <boost/log/trivial.hpp>

#include <TFTrading/ComposeInstrument.hpp>

#include "Process.hpp"

namespace {
  static const std::string sSaveValuesRoot( "/app/collector" );
}

Process::Process(
  const config::Choices& choices
, const std::string& sTimeStamp
)
: m_choices( choices )
, m_sPathName( sSaveValuesRoot + "/" + sTimeStamp )
{
  StartIQFeed();
}

Process::~Process() {

  while( 0 < m_mapCollectL2.size() ) {
    m_mapCollectL2.erase( m_mapCollectL2.begin( ) );
  }

  while( 0 < m_mapCollectL1.size() ) {
    m_mapCollectL1.erase( m_mapCollectL1.begin( ) );
  }

  while ( 0 < m_mapWatch.size() ) {
    m_mapWatch.erase( m_mapWatch.begin() );
  }

  m_pComposeInstrumentIQFeed.reset();

  m_piqfeed->Disconnect();
  m_piqfeed.reset();
}

// need control c handler to terminate, as this is an ongoing process
void Process::StartIQFeed() {
  m_piqfeed = ou::tf::iqfeed::Provider::Factory();
  m_piqfeed->OnConnected.Add( MakeDelegate( this, &Process::HandleIQFeedConnected ) );
  m_piqfeed->Connect();
}

void Process::HandleIQFeedConnected( int ) {
  InitializeComposeInstrument();
}

void Process::InitializeComposeInstrument() {
  m_pComposeInstrumentIQFeed = std::make_unique<ou::tf::ComposeInstrument>(
    m_piqfeed,
    [this](){ // callback once started
      ConstructCollectors();
    }
    );
  assert( m_pComposeInstrumentIQFeed );
}

void Process::ConstructWatch( const std::string& sIQFeedSymbolName, fWatch_t&& fWatch ) {

  mapWatch_t::iterator iterWatch = m_mapWatch.find( sIQFeedSymbolName );

  if ( m_mapWatch.end() == iterWatch ) {
    assert( m_pComposeInstrumentIQFeed );
    m_pComposeInstrumentIQFeed->Compose(
      sIQFeedSymbolName,
      [this, fWatch_=std::move( fWatch ), sIQFeedSymbolName]( pInstrument_t pInstrument, bool bConstructed ){
        pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_piqfeed );
        m_mapWatch.emplace( sIQFeedSymbolName, pWatch );
        fWatch_( pWatch );
      }
    );
  }
  else {
    fWatch( iterWatch->second );
  }
}

void Process::ConstructCollectors() {
  for ( const config::Choices::vName_t::value_type& sIQFeedSymbolName: m_choices.m_vSymbolName_L1 ) {
    ConstructWatch(
      sIQFeedSymbolName,
      [this]( pWatch_t pWatch ){
        ConstructCollectorL1( pWatch );
      } );
  }
  for ( const config::Choices::vName_t::value_type& sIQFeedSymbolName: m_choices.m_vSymbolName_L2 ) {
    ConstructWatch(
      sIQFeedSymbolName,
      [this]( pWatch_t pWatch ){
        ConstructCollectorL2( pWatch );
      } );
  }
}

void Process::ConstructCollectorL1( pWatch_t pWatch ) {

  const std::string& sSymbolName( pWatch->GetInstrumentName() );
  const std::string& sIQFeedSymbolName( pWatch->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

  BOOST_LOG_TRIVIAL(info) << "symbol l1: " // should be able to identify type once composed
    << sSymbolName // from config file?
    << ", " << sIQFeedSymbolName  // resolved name
    ;

  mapCollectL1_t::iterator iterCollectL1 = m_mapCollectL1.find( sSymbolName );
  if ( m_mapCollectL1.end() == iterCollectL1 ) {
    auto result = m_mapCollectL1.emplace( sSymbolName, std::make_unique<collect::L1>( m_sPathName, pWatch ) );
    assert( result.second );
  }
  else {
    BOOST_LOG_TRIVIAL(info) << "symbol l1: " // should be able to identify type once composed
      << sSymbolName
      << " already collecting"
      ;
  }
}

void Process::ConstructCollectorL2( pWatch_t pWatch ) {

  const std::string& sSymbolName( pWatch->GetInstrumentName() );
  const std::string& sIQFeedSymbolName( pWatch->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

  BOOST_LOG_TRIVIAL(info) << "symbol l2: " // should be able to identify type once composed
    << sSymbolName // from config file?
    << ", " << sIQFeedSymbolName  // resolved name
    ;

  mapCollectL2_t::iterator iterCollectL2 = m_mapCollectL2.find( sSymbolName );
  if ( m_mapCollectL2.end() == iterCollectL2 ) {
    auto result = m_mapCollectL2.emplace( sSymbolName, std::make_unique<collect::L2>( m_sPathName, pWatch ) );
    assert( result.second );
  }
  else {
    BOOST_LOG_TRIVIAL(info) << "symbol l2: " // should be able to identify type once composed
      << sSymbolName
      << " already collecting"
      ;
  }
}

void Process::Write() {
  for ( mapCollectL1_t::value_type& vt: m_mapCollectL1 ) {
    vt.second->Write();
  }
  for ( mapCollectL2_t::value_type& vt: m_mapCollectL2 ) {
    vt.second->Write();
  }
}

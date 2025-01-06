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

#include <TFTrading/Watch.h>

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

void Process::ConstructCollectors() {
  assert( m_pComposeInstrumentIQFeed );
  for ( const config::Choices::vName_t::value_type& sIQFeedSymbolName: m_choices.m_vSymbolName_L1 ) {
    m_pComposeInstrumentIQFeed->Compose(
      sIQFeedSymbolName,
      [this]( pInstrument_t pInstrument, bool bConstructed ){
        ConstructCollector( pInstrument );
      }
    );
  }
}

void Process::ConstructCollector( pInstrument_t pInstrument ) {

  const std::string& sSymbolName( pInstrument->GetInstrumentName() );
  const std::string& sIQFeedSymbolName( pInstrument->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

  BOOST_LOG_TRIVIAL(info) << "symbol: " // should be able to identify type once composed
    << sSymbolName // from config file?
    << ", " << sIQFeedSymbolName  // resolved name
    ;

  using pWatch_t = ou::tf::Watch::pWatch_t;
  pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_piqfeed );
  auto result = m_mapCollect.emplace( sSymbolName, std::make_unique<Collect>( m_sPathName, pWatch ) );
  assert( result.second );
}

void Process::Write() {
  for ( mapCollect_t::value_type& vt: m_mapCollect ) {
    vt.second->Write();
  }
}

void Process::Finish() {
  while( 0 < m_mapCollect.size() ) {
    m_mapCollect.erase( m_mapCollect.begin( ) );
  }
}

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

// start watch on l1, l2
// write on regular intervals

#include <TFTrading/BuildInstrument.h>

#include <TFIQFeed/OptionChainQuery.h>
#include <TFTrading/Watch.h>

#include "Process.hpp"

namespace {
  static const std::string sSaveValuesRoot( "/app/collector" );
}

Process::Process(
  const config::Choices& choices
, const std::string& sTimeStamp
)
: m_choices( choices )
, m_sTimeStamp( sTimeStamp )
{
  StartIQFeed();
}

Process::~Process() {

  if ( m_pWatchUnderlying ) {
    m_pWatchUnderlying->StopWatch();
    m_pWatchUnderlying.reset();
  }

  m_pInstrumentUnderlying.reset();

  m_pOptionChainQuery->Disconnect();
  m_pOptionChainQuery.reset();

  m_pBuildInstrumentIQFeed.reset();
  m_pInstrumentUnderlying.reset();

  m_piqfeed->Disconnect();
  m_piqfeed.reset();
}

// need control c handler to terminate, as this is an ongoing process

void Process::StartIQFeed() {
  m_piqfeed = ou::tf::iqfeed::IQFeedProvider::Factory();

  m_piqfeed->OnConnected.Add( MakeDelegate( this, &Process::HandleIQFeedConnected ) );
  m_piqfeed->Connect();
}

void Process::HandleIQFeedConnected( int ) {
  StartChainQuery();
}

void Process::StartChainQuery() {
  if ( m_pOptionChainQuery ) {}
  else {
    m_pOptionChainQuery = std::make_unique<ou::tf::iqfeed::OptionChainQuery>(
      [this](){
        HandleChainQueryConnected( 0 );
      }
    );
    m_pOptionChainQuery->Connect(); // TODO: auto-connect instead?
  }
}

void Process::HandleChainQueryConnected( int ) {
  ConstructUnderlying();
}

void Process::ConstructUnderlying() {

  assert( m_pOptionChainQuery );

  m_pBuildInstrumentIQFeed = std::make_shared<ou::tf::BuildInstrument>( m_piqfeed );
  assert( m_pBuildInstrumentIQFeed );

  const std::string& sName( m_choices.m_sSymbolName );

  if ( '#' == sName.back() ) { // assumes a general future, and need to find actual symbol
    m_pBuildInstrumentIQFeed->Queue(
      sName,
      [this,sName]( pInstrument_t pInstrument ){

        using OptionChainQuery = ou::tf::iqfeed::OptionChainQuery;

        // determine the specific future which matches the continuous generic contract
        boost::gregorian::date expiry( pInstrument->GetExpiry() );
        std::string sBase( sName.substr( 0, sName.size() - 1 ) );
        m_pOptionChainQuery->QueryFuturesChain(  // obtain a list of futures
          sBase, "", "234" /* 2022, 2023, 2024 */ , "4" /* 4 months */,
          [this,expiry]( const OptionChainQuery::FuturesList& list ){
            m_cntInstrumentsProcessed = list.vSymbol.size();
            for ( const OptionChainQuery::vSymbol_t::value_type sSymbol: list.vSymbol ) {
              m_pBuildInstrumentIQFeed->Queue(
                sSymbol,
                [this,expiry]( pInstrument_t pInstrument ){
                  if ( expiry == pInstrument->GetExpiry() ) {
                    m_pInstrumentUnderlying = pInstrument;
                  }
                  m_cntInstrumentsProcessed--;
                  if ( 0 == m_cntInstrumentsProcessed ) {
                    if ( m_pInstrumentUnderlying ) {
                      StartWatch();
                    }
                  }
                } );
            }
          }
          );
      }
    );
  }
  else {
    m_pBuildInstrumentIQFeed->Queue(
      sName,
      [this]( pInstrument_t pInstrument ){
        m_pInstrumentUnderlying = pInstrument;
        StartWatch();
      } );
  }

}

void Process::StartWatch() {
// TODO: watch built elsewhere, needs to be restartable for a new day?
//       or, delete and rebuild for a new day?
//             this, so that can handle when new front month started

  assert( m_pInstrumentUnderlying );

  std::cout << "future: "
    << m_pInstrumentUnderlying->GetInstrumentName()
    << ", " << m_pInstrumentUnderlying->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF )
    << std::endl;

  m_pWatchUnderlying = std::make_shared<ou::tf::Watch>( m_pInstrumentUnderlying, m_piqfeed );
  m_pWatchUnderlying->StartWatch();

}

void Process::StopWatch() {
  const std::string sPathName = sSaveValuesRoot + "/" + m_sTimeStamp;
  m_pWatchUnderlying->StopWatch();
  m_pWatchUnderlying->SaveSeries( sPathName );
}

void Process::Finish() {
  if ( m_pWatchUnderlying ) {
    StopWatch();
    m_pWatchUnderlying.reset();
  }
}

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

#include <boost/log/trivial.hpp>

#include <TFTrading/ComposeInstrument.hpp>

#include "CollectL1.hpp"
#include "CollectL2.hpp"
#include "CollectGreeks.hpp"
#include "CollectATM.hpp"

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

  auto f =
    [this]( const std::string& s, EToCollect e ){
      mapToCollect_t::iterator iter = m_mapToCollect.find( s );
      if ( m_mapToCollect.end() == iter ) {
        m_mapToCollect.emplace( s, ToCollect( e ) );
      }
      else {
        auto result = iter->second.setToCollect.insert( e );
      }
    };

  for ( const config::Choices::vName_t::value_type& sIQFeedSymbolName: m_choices.m_vSymbolName_L1 ) {
    f( sIQFeedSymbolName, EToCollect::L1 );
  }
  for ( const config::Choices::vName_t::value_type& sIQFeedSymbolName: m_choices.m_vSymbolName_L2 ) {
    f( sIQFeedSymbolName, EToCollect::L2 );
  }
  for ( const config::Choices::vName_t::value_type& sIQFeedSymbolName: m_choices.m_vSymbolName_Greeks ) {
    f( sIQFeedSymbolName, EToCollect::Greeks );
  }
  for ( const config::Choices::vName_t::value_type& sIQFeedSymbolName: m_choices.m_vSymbolName_Atm ) {
    f( sIQFeedSymbolName, EToCollect::ATM );
  }

  StartIQFeed();
}

Process::~Process() {

  while( 0 < m_mapCollectATM.size() ) {
    m_mapCollectATM.erase( m_mapCollectATM.begin( ) );
  }

  while( 0 < m_mapCollectGreeks.size() ) {
    m_mapCollectGreeks.erase( m_mapCollectGreeks.begin( ) );
  }

  while( 0 < m_mapCollectL2.size() ) {
    m_mapCollectL2.erase( m_mapCollectL2.begin( ) );
  }

  while( 0 < m_mapCollectL1.size() ) {
    m_mapCollectL1.erase( m_mapCollectL1.begin( ) );
  }

  while ( 0 < m_mapToCollect.size() ) {
    m_mapToCollect.erase( m_mapToCollect.begin() );
  }

  m_pComposeInstrumentIQFeed.reset();

  m_pOptionEngine.reset();
  m_fedrate.SetWatchOff();
  m_fedrate.SaveSeries( m_sPathName );

  if ( m_pOptionChainQuery ) {
    m_pOptionChainQuery->Disconnect();
    m_pOptionChainQuery.reset();
  }

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
  m_fedrate.SetWatchOn( m_piqfeed );
  m_pOptionEngine = std::make_unique<ou::tf::option::Engine>( m_fedrate );
  if ( 0 < m_choices.m_vSymbolName_Atm.size() ) {
    m_pOptionChainQuery = std::make_unique<ou::tf::iqfeed::OptionChainQuery>(
      [this](){ // once query engine is started up
        InitializeComposeInstrument();
      }
    );
    m_pOptionChainQuery->Connect();
  }
  else {
    InitializeComposeInstrument();
  }

}

void Process::InitializeComposeInstrument() {
  m_pComposeInstrumentIQFeed = std::make_unique<ou::tf::ComposeInstrument>(
    m_piqfeed,
    [this](){ // callback once started
      ConstructWatches();
    }
    );
}

void Process::ConstructWatches() {

  assert( m_pComposeInstrumentIQFeed );

  for ( auto& [key, value]: m_mapToCollect ) {
    m_pComposeInstrumentIQFeed->Compose(
      key,
      [ this, &value ]( pInstrument_t pInstrument, bool bConstructed ){
        switch ( pInstrument->GetInstrumentType() ) {
          case ou::tf::InstrumentType::EInstrumentType::Future:
          case ou::tf::InstrumentType::EInstrumentType::Currency:
          case ou::tf::InstrumentType::EInstrumentType::Stock: {
            pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_piqfeed );
            value.pWatch = pWatch;
            ConstructCollectors( value.setToCollect, pWatch );
            }
            break;
          case ou::tf::InstrumentType::EInstrumentType::FuturesOption:
          case ou::tf::InstrumentType::EInstrumentType::Option: {
            pOption_t pOption = std::make_shared<ou::tf::option::Option>( pInstrument, m_piqfeed );
            value.pWatch = pOption;
            ConstructCollectors( value.setToCollect, pOption );
            }
            break;
          default:
            assert( false );
            break;
        }
      }
    );
  }

}

void Process::ConstructCollectors( const setToCollect_t& set, pWatch_t pWatch ) {

  for ( const setToCollect_t::value_type e: set ) {
    switch ( e ) {
      case EToCollect::L1:
        ConstructCollectorL1( pWatch );
        break;
      case EToCollect::L2:
        ConstructCollectorL2( pWatch );
        break;
      case EToCollect::Greeks: {
        ou::tf::InstrumentType::EInstrumentType type( pWatch->GetInstrument()->GetInstrumentType() );
        assert( ou::tf::InstrumentType::Option == type
             || ou::tf::InstrumentType::FuturesOption == type
        );
        pOption_t pOption( std::dynamic_pointer_cast<ou::tf::option::Option>( pWatch) );
        ConstructCollectorGreeks( pOption );
        }
        break;
      case EToCollect::ATM:
        ConstructCollectorATM( pWatch );
        break;
    }
  }
}

void Process::ConstructCollectorL1( pWatch_t pWatch ) {

  const std::string& sSymbolName( pWatch->GetInstrumentName() );
  const std::string& sIQFeedSymbolName( pWatch->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

  BOOST_LOG_TRIVIAL(info) << "symbol l1: " // should be able to identify type once composed
    << sSymbolName // generic name
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
    << sSymbolName // generic name
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

void Process::ConstructCollectorGreeks( pOption_t pOption ) {

  const std::string& sSymbolName( pOption->GetInstrumentName() );
  const std::string& sIQFeedSymbolName( pOption->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

  BOOST_LOG_TRIVIAL(info) << "symbol greeks: " // should be able to identify type once composed
    << sSymbolName // generic name
    << ", " << sIQFeedSymbolName  // resolved name
    ;

  mapCollectGreeks_t::iterator iterCollectGreeks = m_mapCollectGreeks.find( sSymbolName );
  if ( m_mapCollectGreeks.end() == iterCollectGreeks ) {
    auto result = m_mapCollectGreeks.emplace( sSymbolName, std::make_unique<collect::Greeks>( m_sPathName, pOption ) );
    assert( result.second );
  }
  else {
    BOOST_LOG_TRIVIAL(info) << "symbol greeks: " // should be able to identify type once composed
      << sSymbolName
      << " already collecting"
      ;
  }
}

void Process::ConstructCollectorATM( pWatch_t pWatch ) {

  const std::string& sSymbolName( pWatch->GetInstrumentName() );
  const std::string& sIQFeedSymbolName( pWatch->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

  BOOST_LOG_TRIVIAL(info) << "symbol atm: " // should be able to identify type once composed
    << sSymbolName // generic name
    << ", " << sIQFeedSymbolName  // resolved name
    ;

  assert( m_pOptionChainQuery );

  using pOption_t = collect::ATM::pOption_t;

  mapCollectATM_t::iterator iterCollectATM = m_mapCollectATM.find( sSymbolName );
  if ( m_mapCollectATM.end() == iterCollectATM ) {

    m_pOptionEngine->RegisterUnderlying( pWatch );

    auto result = m_mapCollectATM.emplace(
      sSymbolName,
      std::make_unique<collect::ATM>(
        m_sPathName,
        pWatch,
        [this]( collect::ATM::pInstrument_t pInstrument )->collect::ATM::pOption_t { // fBuildOption_t
          pOption_t pOption = std::make_shared<ou::tf::option::Option>( pInstrument, m_piqfeed );
          m_pOptionEngine->RegisterOption( pOption );
          return pOption;
        },
        [this]( pInstrument_t pInstrumentUnderlying, collect::ATM::fInstrumentOption_t&& fIO ){ // fGatherOptions_t
          QueryChains( pInstrumentUnderlying, std::move( fIO ) );
        },
        [this]( pOption_t& pOption, pWatch_t& pUnderlying ){
          m_pOptionEngine->Add( pOption, pUnderlying );
        },
        [this]( pOption_t& pOption, pWatch_t& pUnderlying ){
          m_pOptionEngine->Remove( pOption, pUnderlying );
        }
        ) );
    assert( result.second );
  }
  else {
    BOOST_LOG_TRIVIAL(info) << "symbol atm: " // should be able to identify type once composed
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
  for ( mapCollectGreeks_t::value_type& vt: m_mapCollectGreeks ) {
    vt.second->Write();
  }
  for ( mapCollectATM_t::value_type& vt: m_mapCollectATM ) {
    vt.second->Write();
  }
}

void Process::QueryChains( pInstrument_t pUnderlying, collect::ATM::fInstrumentOption_t&& fIO ) {

  using query_t = ou::tf::iqfeed::OptionChainQuery;
  auto f =
    [this,fIO_ = std::move( fIO )]( const query_t::OptionList& list ){
      BOOST_LOG_TRIVIAL(info)
        << "chain request " << list.sUnderlying << " has "
        << list.vSymbol.size() << " options"
        ;

      // TODO:  cache entries for each start?

      size_t zero = list.vSymbol.size(); // signal when done
      for ( const query_t::vSymbol_t::value_type& sSymbol: list.vSymbol ) {
        zero--;
        m_pComposeInstrumentIQFeed->Compose(
          sSymbol,
          [ zero, fIO_ /* make a copy */]( pInstrument_t pInstrument, bool bConstructed ){ // what is bConstructed?
            fIO_( zero, pInstrument );
          } );
      }
    };

  const std::string& sIQFeedUnderlying( pUnderlying->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );
  switch ( pUnderlying->GetInstrumentType() ) {
    case ou::tf::InstrumentType::Future:
      m_pOptionChainQuery->QueryFuturesOptionChain(
        sIQFeedUnderlying,
        "pc", "", "", "1", // 1 near month
        std::move( f )
      );
      break;
    case ou::tf::InstrumentType::Stock:
      m_pOptionChainQuery->QueryEquityOptionChain(
        sIQFeedUnderlying,
        "pc", "", "1", "0","0","0",  // 1 near month
        std::move( f )
      );
      break;
    default:
      assert( false );
      break;
  }
}


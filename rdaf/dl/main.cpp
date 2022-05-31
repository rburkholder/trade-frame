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
 * File:    Main.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/dl
 * Created: May 29, 2022  14:42
 */

#include <future> 

#include <TFIQFeed/Provider.h>
#include <TFIQFeed/SymbolLookup.h>
#include <TFIQFeed/HistoryQuery.h>

#include <TFTrading/AcquireFundamentals.h>

#include "Config.hpp"

using setNames_t = ou::tf::iqfeed::SymbolLookup::setNames_t;

namespace {

  static const size_t nMaxInTransit = 40;

  setNames_t setExchanges;
  setNames_t setSecurityTypes;

}

struct Security {
  std::string sName;
  key_t keyListedMarket;
  double dblPriceEarnings;
  size_t nAverageVolume;
  double dblAssets;
  double dblLiabilities;
  double dblCommonSharesOutstanding;
  size_t nTicks;
  Security( const std::string& sName_, key_t keyListedMarket_ )
  : sName( sName_ ), keyListedMarket( keyListedMarket_ )
  , dblPriceEarnings {}, nAverageVolume {}
  , dblAssets {}, dblLiabilities {}
  , dblCommonSharesOutstanding {}
  , nTicks {}
  {}
};

using pSecurity_t = std::shared_ptr<Security>;
using mapSecurity_t = std::map<std::string,pSecurity_t>;
using fSecurity_t = std::function<void(pSecurity_t)>;

class Symbols {
public:

  Symbols( double dblMinPrice, fSecurity_t&& fSecurity ) {

    m_dblMinPrice = dblMinPrice;
    m_fSecurity = std::move( fSecurity );

    m_piqfeed = ou::tf::iqfeed::IQFeedProvider::Factory();
    m_piqfeed->OnConnected.Add( MakeDelegate( this, &Symbols::HandleConnected ) );
    m_piqfeed->Connect();

    future = promise.get_future();
    future.wait();

    std::cout << countSymbols << " symbols processed"  << std::endl;

  }
protected:
private:

  double m_dblMinPrice;
  fSecurity_t m_fSecurity;

  using pIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;
  pIQFeed_t m_piqfeed;

  std::promise<int> promise;
  std::future<int> future;

  size_t countSymbols {};
  size_t countNonZeroCommonShares {};
  size_t countMinimumPrice {};

  using key_t = ou::tf::iqfeed::SymbolLookup::key_t;

  //using vSecurity_t = std::vector<Security>;
  //vSecurity_t m_vSecurity;
  //vSecurity_t::size_type m_ixSecurity {}; // feeds the acquisition process

  mapSecurity_t m_mapSecurity;

  void HandleConnected( int ) {
    std::cout << "connected" << std::endl;
    m_piqfeed->SymbolList(
      setExchanges, setSecurityTypes,
      [this](const std::string& sSymbol, key_t keyListedMarket){
        //std::cout << sSymbol << std::endl;
        m_mapSecurity.emplace( sSymbol, std::make_shared<Security>( sSymbol, keyListedMarket ) );
        if ( nMaxInTransit > m_mapAcquire.size() ) {
          GetFundamentals();
        }
        countSymbols++;
      },
      [this](){
        std::cout << "added " << countSymbols << " symbols" << std::endl;
        //promise.set_value( 0 ); // TODO: this will need to be moved to later in the pipeline
      }
    );
  }

  using pAcquireFundamentals_t = std::shared_ptr<ou::tf::AcquireFundamentals>;

  // assumes single thread
  pAcquireFundamentals_t m_pAcquireFundamentals_burial;

  struct Acquire {
    pSecurity_t pSecurity;
    pAcquireFundamentals_t pAcquireFundamentals;
    Acquire( pSecurity_t pSecurity_ ): pSecurity( pSecurity_ ) {}
  };

  using mapAcquire_t = std::map<std::string,Acquire>;
  mapAcquire_t m_mapAcquire;

  void GetFundamentals() {

    using pWatch_t = ou::tf::Watch::pWatch_t;
    using Fundamentals = ou::tf::Watch::Fundamentals;
    using pInstrument_t = ou::tf::Instrument::pInstrument_t;

    if ( 0 < m_mapSecurity.size() ) {

      mapSecurity_t::iterator iterSecurity = m_mapSecurity.begin();
      pSecurity_t pSecurity = iterSecurity->second;
      m_mapSecurity.erase( iterSecurity );

      const std::string& sName( pSecurity->sName );

      pInstrument_t pInstrument = std::make_shared<ou::tf::Instrument>( sName );
      pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, sName );
      pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_piqfeed );

      mapAcquire_t::iterator iterAcquire
        = m_mapAcquire.insert(
            m_mapAcquire.begin(),
            mapAcquire_t::value_type( sName, Acquire( pSecurity ) ) );

      iterAcquire->second.pAcquireFundamentals
        = ou::tf::AcquireFundamentals::Factory(
          std::move( pWatch ),
          [this,iterAcquire]( pWatch_t pWatch ){

            const Fundamentals& fundamentals( pWatch->GetFundamentals() );
            Security& security( *iterAcquire->second.pSecurity.get() );

            if ( 0 < fundamentals.dblCommonSharesOutstanding ) {
              countNonZeroCommonShares++;

              if ( m_dblMinPrice < fundamentals.dbl52WkLo ) {
                countMinimumPrice++;

                security.dblAssets = fundamentals.dblAssets;
                security.dblLiabilities = fundamentals.dblLiabilities;
                security.nAverageVolume = fundamentals.nAverageVolume;
                security.dblPriceEarnings = fundamentals.dblPriceEarnings;
                security.dblCommonSharesOutstanding = fundamentals.dblCommonSharesOutstanding;
                std::cout
                  << security.keyListedMarket << ","
                  << security.sName << ","
                  << security.dblCommonSharesOutstanding << ","
                  << pWatch->LastTrade().Price() << ","
                  << security.nAverageVolume << ","
                  << fundamentals.dbl52WkLo
                  << std::endl;

                m_fSecurity( iterAcquire->second.pSecurity );
              }
            }

            m_pAcquireFundamentals_burial = std::move( iterAcquire->second.pAcquireFundamentals );
            m_mapAcquire.erase( iterAcquire ); // needs to come before the lookup

            GetFundamentals();
          }
        );

      iterAcquire->second.pAcquireFundamentals->Start();
    }
    else {
      // TODO: finish up
      if ( 0 == m_mapAcquire.size() ) {
        std::cout 
          << countNonZeroCommonShares 
          << " with outstanding shares, "
          << countMinimumPrice
          << " with minimum price"
          << std::endl;
        promise.set_value( 0 );
      }
    }
  }

};

// alternative to this might have been HistoryBulkQuery
class RetrieveTicks: ou::tf::iqfeed::HistoryQuery<RetrieveTicks> {
  friend ou::tf::iqfeed::HistoryQuery<RetrieveTicks>;
public:

  using fConnected_t = std::function<void()>;

  RetrieveTicks( fConnected_t&& fConnected )
  : ou::tf::iqfeed::HistoryQuery<RetrieveTicks>()
  {
    assert( fConnected );
    m_fConnected = std::move( fConnected );
    Connect();
  }

  virtual ~RetrieveTicks() {
    Disconnect();
  }

  using fTick_t = std::function<void(const ou::tf::iqfeed::HistoryStructs::TickDataPoint& )>;
  using fDone_t = std::function<void()>;
  void RequestOneDayOfTicks( const std::string& sName, fTick_t&& fTick, fDone_t&& fDone ) {
    assert( fTick );
    assert( fDone );
    m_fTick = std::move( fTick );
    m_fDone = std::move( fDone );
    RetrieveNDaysOfDataPoints( sName, 4 );
  }

protected:
  void OnHistoryConnected() {
    std::cout << "History Connected" << std::endl;
    m_fConnected();
  }
  void OnHistoryDisconnected() {}
  void OnHistoryError( size_t e ) {}
  void OnHistoryTickDataPoint( TickDataPoint* pDP ) {
    m_fTick( *pDP );
    ReQueueTickDataPoint( pDP );
  };
  void OnHistoryRequestDone() {
    m_fDone();
  };
private:
  fConnected_t m_fConnected;
  fTick_t m_fTick;
  fDone_t m_fDone;
};

class ControlTickRetrieval {
public:
  ControlTickRetrieval( fSecurity_t&& fSecurity )
  : m_fSecurity( std::move( fSecurity ) )
  {
    assert( m_fSecurity );
    for ( uint32_t count = 0; count < maxStarts; count++ ) {
      m_vRetrieveTicks_Avail.emplace_back(
        std::make_shared<RetrieveTicks>(
          [this](){
            m_countStarted++;
            if ( maxStarts == m_countStarted ) {
              promise.set_value( 1 );
            }
          } )
        );
    }
    future = promise.get_future();
    future.wait();
    assert( maxStarts == m_countStarted );  // assumes sync startup, use future/promise if async
  }

  void Retrieve( pSecurity_t pSecurity ) {
    m_mapSecurity_Waiting.emplace( pSecurity->sName, pSecurity );
    StartRetrieval();
  }

  void Wait() {
    //future = promise.get_future();
    future.wait();
  }

protected:
private:

  fSecurity_t m_fSecurity;

  static const uint32_t maxStarts = 10;
  int m_countStarted {};
  using pRetrieveTicks_t = std::shared_ptr<RetrieveTicks>;
  using vRetrieveTicks_t = std::vector<pRetrieveTicks_t>;
  vRetrieveTicks_t m_vRetrieveTicks_Avail;
  //vRetrieveTicks_t m_vRetrieveTicks_Running;

  std::promise<int> promise;
  std::future<int> future;

  mapSecurity_t m_mapSecurity_Waiting;

  struct Running {
    pRetrieveTicks_t pRetrieveTicks;
    pSecurity_t pSecurity;
    Running( pRetrieveTicks_t pRetrieveTicks_, pSecurity_t pSecurity_ )
    : pRetrieveTicks( pRetrieveTicks_ ), pSecurity( pSecurity_ )
    {}
  };

  using mapRetrieveTicks_t = std::map<std::string, Running>;
  mapRetrieveTicks_t m_mapRetrieveTicks;

  void StartRetrieval() {
    if ( 0 < m_vRetrieveTicks_Avail.size() ) {
      if ( 0 < m_mapSecurity_Waiting.size() ) {
        pRetrieveTicks_t pRetrieveTicks = m_vRetrieveTicks_Avail.back();
        m_vRetrieveTicks_Avail.pop_back();
        mapSecurity_t::iterator iterSecurity = m_mapSecurity_Waiting.begin();
        pSecurity_t pSecurity = iterSecurity->second;
        m_mapSecurity_Waiting.erase( iterSecurity );

        auto result = m_mapRetrieveTicks.emplace( pSecurity->sName, Running( pRetrieveTicks, pSecurity ) );
        assert( result.second );

        pRetrieveTicks->RequestOneDayOfTicks(
          pSecurity->sName,
          [pSecurity](const ou::tf::iqfeed::HistoryStructs::TickDataPoint& tdp){  // fTick_t
            pSecurity->nTicks++;
          },
          [this,iter=result.first](){ // fDone_t
            pSecurity_t pSecurity = iter->second.pSecurity;
            std::cout << pSecurity->sName << " has " << pSecurity->nTicks << " ticks" << std::endl;
            m_fSecurity( iter->second.pSecurity );
            m_vRetrieveTicks_Avail.push_back( iter->second.pRetrieveTicks );
            m_mapRetrieveTicks.erase( iter );
            StartRetrieval();
          });
      }
      else {
        assert( 0 == m_mapSecurity_Waiting.size() );
        if ( maxStarts == m_vRetrieveTicks_Avail.size() ) {
          promise.set_value( 2 );
        }
      }
    }
  }
};

int main( int argc, char* argv[] ) {

  config::Choices choices;
  if ( config::Load( "rdaf/download.cfg", choices ) ) {

    for ( const config::vName_t::value_type& vt: choices.m_vExchange ) {
      setExchanges.emplace( vt );
    }

    for ( const config::vName_t::value_type& vt: choices.m_vSecurityType ) {
      setSecurityTypes.emplace( vt );
    }

    ControlTickRetrieval control(
      []( pSecurity_t pSecurity ){ // finished securities
        //std::cout << pSecurity->sName << " history processed." << std::endl;
      });

    Symbols symbols(
      choices.m_dblMinPrice
    , [&control]( pSecurity_t pSecurity ) {
        //std::cout << pSecurity->sName << " sent to history" << std::endl;
        control.Retrieve( pSecurity );
      }
    );

    control.Wait();

  }

  return 0;
}

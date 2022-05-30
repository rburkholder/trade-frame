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

#include <TFTrading/AcquireFundamentals.h>

#include "Config.hpp"

using setNames_t = ou::tf::iqfeed::SymbolLookup::setNames_t;

namespace {

static const size_t nMaxInTransit = 40;

setNames_t setExchanges;
setNames_t setSecurityTypes;

}

class Symbols {
public:
  Symbols( double dblMinPrice ) {

    m_dblMinPrice = dblMinPrice;
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

  using pIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;
  pIQFeed_t m_piqfeed;

  std::promise<int> promise;
  std::future<int> future;

  size_t countSymbols {};
  size_t countNonZeroCommonShares {};
  size_t countMinimumPrice {};

  using key_t = ou::tf::iqfeed::SymbolLookup::key_t;

  struct Instrument {
    std::string sName;
    key_t keyListedMarket;
    double dblPriceEarnings;
    size_t nAverageVolume;
    double dblAssets;
    double dblLiabilities;
    double dblCommonSharesOutstanding;
    Instrument( const std::string& sName_, key_t keyListedMarket_ )
    : sName( sName_ ), keyListedMarket( keyListedMarket_ )
    , dblPriceEarnings {}, nAverageVolume {}
    , dblAssets {}, dblLiabilities {}
    , dblCommonSharesOutstanding {}
    {}
  };

  using vInstrument_t = std::vector<Instrument>;
  vInstrument_t m_vInstrument;
  vInstrument_t::size_type m_ixInstrument {}; // feeds the acquisition process

  void HandleConnected( int ) {
    std::cout << "connected" << std::endl;
    m_piqfeed->SymbolList(
      setExchanges, setSecurityTypes,
      [this](const std::string& sSymbol, key_t keyListedMarket){
        //std::cout << sSymbol << std::endl;
        m_vInstrument.emplace_back( Instrument( sSymbol, keyListedMarket ) );
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
    vInstrument_t::size_type ixInstrument;
    pAcquireFundamentals_t pAcquireFundamentals;
  };

  using mapAcquire_t = std::map<std::string,Acquire>;
  mapAcquire_t m_mapAcquire;

  void GetFundamentals() {

    using pWatch_t = ou::tf::Watch::pWatch_t;
    using Fundamentals = ou::tf::Watch::Fundamentals;
    using pInstrument_t = ou::tf::Instrument::pInstrument_t;

    if ( m_vInstrument.size() > m_ixInstrument ) {

      const std::string sName( m_vInstrument[ m_ixInstrument ].sName );

      pInstrument_t pInstrument = std::make_shared<ou::tf::Instrument>( sName );
      pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, sName );
      pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_piqfeed );

      mapAcquire_t::iterator iterAcquire
        = m_mapAcquire.insert(
            m_mapAcquire.begin(),
            mapAcquire_t::value_type( sName, Acquire() ) );

      iterAcquire->second.ixInstrument = m_ixInstrument;

      iterAcquire->second.pAcquireFundamentals
        = ou::tf::AcquireFundamentals::Factory(
          std::move( pWatch ),
          [this,ix=m_ixInstrument,iterAcquire]( pWatch_t pWatch ){
            const Fundamentals& fundamentals( pWatch->GetFundamentals() );
            Instrument& instrument( m_vInstrument[ ix ] );
            instrument.dblAssets = fundamentals.dblAssets;
            instrument.dblLiabilities = fundamentals.dblLiabilities;
            instrument.nAverageVolume = fundamentals.nAverageVolume;
            instrument.dblPriceEarnings = fundamentals.dblPriceEarnings;
            instrument.dblCommonSharesOutstanding = fundamentals.dblCommonSharesOutstanding;
            std::cout 
              << instrument.keyListedMarket << "," 
              << instrument.sName << ","
              << instrument.dblCommonSharesOutstanding << ","
              << pWatch->LastTrade().Price() << ","
              << instrument.nAverageVolume << ","
              << fundamentals.dbl52WkLo
              << std::endl;
            if ( 0 < instrument.dblCommonSharesOutstanding ) {
              countNonZeroCommonShares++;
              if ( m_dblMinPrice < fundamentals.dbl52WkLo ) {
                countMinimumPrice++;
              }
            }

            // TODO: pass on to download
            //   filter: > $5, > 0 outstanding

            m_pAcquireFundamentals_burial = std::move( iterAcquire->second.pAcquireFundamentals );
            m_mapAcquire.erase( iterAcquire ); // needs to come before the lookup

            GetFundamentals();
          }
        );

      iterAcquire->second.pAcquireFundamentals->Start();
      m_ixInstrument++;

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


int main( int argc, char* argv[] ) {

  config::Choices choices;
  if ( config::Load( "rdaf/download.cfg", choices ) ) {

    for ( const config::vName_t::value_type& vt: choices.m_vExchange ) {
      setExchanges.emplace( vt );
    }

    for ( const config::vName_t::value_type& vt: choices.m_vSecurityType ) {
      setSecurityTypes.emplace( vt );
    }

    Symbols symbols( choices.m_dblMinPrice );

  }

  //using pAcquireFundamentals_t = std::shared_ptr<ou::tf::AcquireFundamentals>;
  //std::shared_ptr<ou::tf::AcquireFundamentals> m_pAcquireFundamentals_live;

  struct InProgress {
    //vSymbols_iter iterSymbols;
    //pAcquireFundamentals_t pAcquireFundamentals;
  };


  return 0;
}

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

const size_t nInTransit = 40;

setNames_t setExchanges;
setNames_t setSecurityTypes;

}

class Symbols {
public:
  Symbols() {
    m_piqfeed = ou::tf::iqfeed::IQFeedProvider::Factory();

    m_piqfeed->OnConnected.Add( MakeDelegate( this, &Symbols::HandleConnected ) );
    m_piqfeed->Connect();

    future = promise.get_future();
    future.wait();

    std::cout << "symbol list done " << count << std::endl;

  }
protected:
private:

  using pIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;
  pIQFeed_t m_piqfeed;

  std::promise<int> promise;
  std::future<int> future;

  size_t count {};

  void HandleConnected( int ) {
    std::cout << "connected" << std::endl;
    m_piqfeed->SymbolList(
      setExchanges, setSecurityTypes,
      [this](const std::string& sSymbol){
        std::cout << sSymbol << std::endl;
        count++;
      },
      [this](){
        promise.set_value( 0 );
      }
    );
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

    Symbols symbols;

  }

  //using pAcquireFundamentals_t = std::shared_ptr<ou::tf::AcquireFundamentals>;
  //std::shared_ptr<ou::tf::AcquireFundamentals> m_pAcquireFundamentals_live;

  struct InProgress {
    //vSymbols_iter iterSymbols;
    //pAcquireFundamentals_t pAcquireFundamentals;
  };


  return 0;
}

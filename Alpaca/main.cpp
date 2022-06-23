//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP SSL client, asynchronous
//
//------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <functional>

#include <boost/asio/strand.hpp>

#include <TFTrading/DBWrapper.h>
#include <TFTrading/OrderManager.h>
#include <TFTrading/PortfolioManager.h>
#include <TFTrading/ProviderManager.h>
#include <TFTrading/InstrumentManager.h>

#include <TFIQFeed/Provider.h>

#include <TFAlpaca/Provider.hpp>

#include "Config.hpp"

int main( int argc, char** argv )
{
    // Check command line arguments.
//    if(argc != 4 && argc != 5)
//    {
//        std::cerr <<
//            "Usage: http-client-async-ssl <host> <port> <target> [<HTTP version: 1.0 or 1.1(default)>]\n" <<
//            "Example:\n" <<
//            "    http-client-async-ssl www.example.com 443 /\n" <<
//            "    http-client-async-ssl www.example.com 443 / 1.0\n";
//        return EXIT_FAILURE;
//    }
    //auto const host = argv[1];
    //auto const port = argv[2];
    //auto const target = argv[3];

    // 'get' to obtain crypto asset list
    //const std::string sTarget( "/v2/assets?status=active&asset_class=crypto" );

    // 1. load choices
    config::Choices choices;
    config::Load( "alpaca.cfg", choices );

    // 2. construct manager & register
    using pProviderAlpaca_t = ou::tf::alpaca::Provider::pProvider_t;
    pProviderAlpaca_t pProviderAlpaca = ou::tf::alpaca::Provider::Factory();
    pProviderAlpaca->SetName( "alpaca" );  // may already be set to this
    ou::tf::ProviderManager::GlobalInstance().Register( pProviderAlpaca );

    pProviderAlpaca->Set( choices.m_sAlpacaDomain, choices.m_sAlpacaKey, choices.m_sAlpacaSecret );
    pProviderAlpaca->Connect();

    using pProviderIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;
    pProviderIQFeed_t pProviderIQFeed = ou::tf::iqfeed::IQFeedProvider::Factory();
    pProviderIQFeed->SetName( "iqfeed" ); // needs to match name in database
    ou::tf::ProviderManager::GlobalInstance().Register( pProviderIQFeed );
    pProviderIQFeed->Connect();

    // 3. database can then use the registered provider
    static const std::string sDbName( "alpaca.db" );
    std::unique_ptr<ou::tf::db> m_pdb = std::make_unique<ou::tf::db>( sDbName );


    sleep( 4 );

    const std::string sSymbol( "GLD" );
    const std::string sPortfolio( "USD" );

    using pInstrument_t = ou::tf::Instrument::pInstrument_t;
    ou::tf::Instrument::pInstrument_t pInstrument;

    ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
    if ( im.Exists( sSymbol ) ) {
      pInstrument = im.Get( sSymbol );
    }
    else {
      pInstrument = std::make_shared<ou::tf::Instrument>( sSymbol, ou::tf::InstrumentType::Stock, "alpaca" );
      ou::tf::InstrumentManager::GlobalInstance().Register( pInstrument );
    }

    ou::tf::Position::pPosition_t pPosition;
    {
      ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
      if ( pm.PositionExists( sPortfolio, sSymbol ) ) {
        pPosition = pm.GetPosition( sPortfolio, sSymbol );
      }
      else {
        pPosition = pm.ConstructPosition(
          sPortfolio, sSymbol, "manual",
          "alpaca", "iqfeed", pProviderAlpaca, pProviderIQFeed,
          pInstrument
        );
        // TODO: will need to confirm names are ok, iqfeed might need to be changed in provider
      }
    }

    ou::tf::Order::pOrder_t pOrder;
    {
      ou::tf::OrderManager& om( ou::tf::OrderManager::GlobalInstance() );
      om.CheckOrderId( 32 ); // put this into state file

      pOrder = pPosition->ConstructOrder(
        ou::tf::OrderType::Market,
        ou::tf::OrderSide::Buy,
        100
      );

      //om.PlaceOrder( pProvider.get(), pOrder );
    }


    sleep( 20 );

    pProviderAlpaca->Disconnect();
    pProviderIQFeed->Disconnect();

    pOrder.reset();
    pPosition.reset();
    pInstrument.reset();
    pProviderAlpaca.reset();
    pProviderIQFeed.reset();

    m_pdb.reset();

    return EXIT_SUCCESS;
}

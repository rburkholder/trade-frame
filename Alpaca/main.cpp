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
 * File:    main.cpp
 * Author:  raymond@burkholder.net
 * Project: Alpaca
 * Created: June 4, 2022 14:48:37
 */

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

    ou::tf::OrderManager& om( ou::tf::OrderManager::GlobalInstance() );
    ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
    ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
    ou::tf::ProviderManager& providers( ou::tf::ProviderManager::GlobalInstance() );

    // 1. load choices
    config::Choices choices;
    config::Load( "alpaca.cfg", choices );

    // 2. construct manager & register
    using pProviderAlpaca_t = ou::tf::alpaca::Provider::pProvider_t;
    pProviderAlpaca_t pProviderAlpaca = ou::tf::alpaca::Provider::Factory();
    pProviderAlpaca->SetName( "alpaca" );  // may already be set to this
    providers.Register( pProviderAlpaca );

    pProviderAlpaca->Set( choices.m_sAlpacaDomain, choices.m_sAlpacaKey, choices.m_sAlpacaSecret );
    pProviderAlpaca->Connect();

    using pProviderIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;
    pProviderIQFeed_t pProviderIQFeed = ou::tf::iqfeed::IQFeedProvider::Factory();
    pProviderIQFeed->SetName( "iqfeed" ); // needs to match name in database
    providers.Register( pProviderIQFeed );
    pProviderIQFeed->Connect();

    // 3. database can then use the registered provider
    static const std::string sDbName( "alpaca.db" );
    std::unique_ptr<ou::tf::db> m_pdb = std::make_unique<ou::tf::db>( sDbName );

    sleep( 10 ); // TODO: improve by responding to provider events instead

    std::cout << "construct symbol: " << std::endl;

    const std::string sSymbol( "GLD" );
    const std::string sPortfolio( "USD" );

    using pInstrument_t = ou::tf::Instrument::pInstrument_t;
    pInstrument_t pInstrument;

    if ( im.Exists( sSymbol ) ) {
      pInstrument = im.Get( sSymbol );
    }
    else {
      pInstrument = std::make_shared<ou::tf::Instrument>( sSymbol, ou::tf::InstrumentType::Stock, "alpaca" );
      im.Register( pInstrument );
    }

    std::cout << "construct position: " << std::endl;

    ou::tf::Position::pPosition_t pPosition;
    {
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

    std::cout << "construct / transmit limit buy: " << std::endl;

    ou::tf::Order::pOrder_t pOrder;

    pOrder = pPosition->ConstructOrder(
      ou::tf::OrderType::Limit,
      ou::tf::OrderSide::Buy,
      100,
      150.0
    );
    om.PlaceOrder( pProviderAlpaca.get(), pOrder );
    sleep( 4 );
    om.CancelOrder( pOrder->GetOrderId() );

    //std::cout << "construct / transmit limit sell: " << std::endl;

    //pOrder = pPosition->ConstructOrder(
    //  ou::tf::OrderType::Limit,
    //  ou::tf::OrderSide::Sell,
    //  100,
    //  150.0
    //);
    //om.PlaceOrder( pProviderAlpaca.get(), pOrder );

    std::cout << "wait for completion" << std::endl;

    sleep( 10 );

    std::cout << "disconnection: " << std::endl;

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


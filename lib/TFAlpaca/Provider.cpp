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
 * File:    Provider.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFAlpaca
 * Created: June 5, 2022 16:04
 */

#include <boost/json.hpp>

#include <boost/asio/strand.hpp>

#include "one_shot.hpp"
#include "root_certificates.hpp"

#include "Asset.hpp"
#include "Order.hpp"
#include "Provider.hpp"

namespace json = boost::json;           // from <boost/json.hpp>

namespace ou {
namespace tf {
namespace alpaca {

Provider::Provider( const std::string& sHost, const std::string& sKey, const std::string& sSecret )
: ProviderInterface<Provider,Asset>()
, m_ssl_context( ssl::context::tlsv12_client )
, m_sHost( sHost )
, m_sPort( "443" )
, m_sAlpacaKeyId( sKey )
, m_sAlpacaSecret( sSecret )
{
  m_sName = "Alpaca";
  m_nID = keytypes::EProviderAlpaca;
  m_pProvidesBrokerInterface = true;

  if ( 0 == GetThreadCount() ) {
    SetThreadCount( 1 ); // need at least one thread for websocket processing
  }

  // This holds the root certificate used for verification
  // NOTE: this needs to be fixed, based upon comments in the include header
  load_root_certificates( m_ssl_context );

  // Verify the remote server's certificate
  m_ssl_context.set_verify_mode( ssl::verify_peer );

}

Provider::~Provider() {
}

void Provider::Connect() {
  auto os = std::make_shared<ou::tf::alpaca::session::one_shot>(
    asio::make_strand( m_srvc ),
    m_ssl_context
    );
  os->get(
    m_sHost, m_sPort,
    "/v2/assets",
    m_sAlpacaKeyId, m_sAlpacaSecret,
    [this]( bool bStatus, const std::string& body ){
      if ( bStatus ) {
        // decode the body

        json::error_code jec;
        json::value jv = json::parse( body, jec );
        if ( jec.failed() ) {
          std::cout << "failed to parse /v2/assets" << std::endl;
        }
        else {
          // Write the message to standard out
          //std::cout << res_ << std::endl;

          std::cout << body << std::endl;


          //alpaca::Asset asset( json::value_to<alpaca::Asset>( jv ) ); // single asset
          //Asset::vMessage_t vMessage = json::value_to<Asset::vMessage_t>( jv );
          Asset::vMessage_t vMessage;
          Asset::Decode( body, vMessage );

          for ( const Asset::vMessage_t::value_type& vt: vMessage ) {
            std::cout
              //<< vt.id << ","
              << vt.class_ << ","
              << vt.exchange << ","
              << vt.symbol << ","
              << "trade=" << vt.tradable << ","
              << "short=" << vt.shortable << ","
              << "margin=" << vt.marginable
              << std::endl;
          }

          std::cout << "found " << vMessage.size() << " assets" << std::endl;

        }
      }
    }
  );
}

Provider::pSymbol_t Provider::NewCSymbol( pInstrument_t pInstrument ) {
  pSymbol_t pSymbol( new Asset( pInstrument->GetInstrumentName( ID() ), pInstrument ) );
  inherited_t::AddCSymbol( pSymbol );
  return pSymbol;
}

void Provider::PlaceOrder( pOrder_t pOrder ) {
  inherited_t::PlaceOrder( pOrder ); // any underlying initialization

  const ou::tf::Order& order(*pOrder);
  const ou::tf::Order::TableRowDef& trd( order.GetRow() );
  json::object request;
  request[ "symbol" ] = trd.idInstrument;
  request[ "quantity" ] = trd.nOrderQuantity;
  switch ( trd.eOrderSide ) {
    case OrderSide::Buy:
      request[ "side" ] = "buy";
      break;
    case OrderSide::Sell:
      request[ "side" ] = "sell";
      break;
    default:
      assert( false );
      break;
  }
  switch ( trd.eOrderType ) {
    case OrderType::Market:
      request[ "type" ] = "market";
      break;
    case OrderType::Limit:
      request[ "type" ] = "limit";
      request[ "limit_price"] = trd.dblPrice1;
      break;
    case OrderType::Stop:
      request[ "type" ] = "stop";
      request[ "stop_price" ] = trd.dblPrice1;
      break;
    case OrderType::StopLimit:
      request[ "type" ] = "stop_limit";
      request[ "limit_price" ] = trd.dblPrice1;
      request[ "stop_price"] = trd.dblPrice2;
      break;
    default:
      assert( false );
      break;
  }
  request[ "time_in_force" ] = "day";
  request[ "client_order_id" ] = trd.idOrder;
  request[ "order_class" ] = "simple";

  auto os = std::make_shared<ou::tf::alpaca::session::one_shot>(
    asio::make_strand( m_srvc ),
    m_ssl_context
  );
  os->post(
    m_sHost, m_sPort,
    m_sAlpacaKeyId, m_sAlpacaSecret,
    "/v2/orders", json::serialize( request ),
    []( bool bResult, const std::string& s ) {
      if ( bResult ) {
        std::cout << "place order error: " << s << std::endl;
      }
      else {
        std::cout << "place order result: " << s << std::endl;
      }

    }
  );
}

void Provider::CancelOrder( pOrder_t pOrder ) {
  inherited_t::CancelOrder( pOrder );
}


} // namespace alpaca
} // namespace tf
} // namespace ou

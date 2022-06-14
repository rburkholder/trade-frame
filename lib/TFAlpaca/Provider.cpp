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

#include <boost/lexical_cast.hpp>

#include <boost/asio/strand.hpp>

#include "one_shot.hpp"
#include "web_socket.hpp"
#include "root_certificates.hpp"

#include "Asset.hpp"
#include "Order.hpp"
#include "Position.hpp"
#include "Provider.hpp"

namespace json = boost::json;

namespace ou {
namespace tf {
namespace alpaca {

namespace {
  template<class T>
  void extract( json::object const& obj, T& t, json::string_view key ) {
    t = json::value_to<T>( obj.at( key ) );
  }
} // namespace anonymous


Provider::Provider( const std::string& sHost, const std::string& sKey, const std::string& sSecret )
: ProviderInterface<Provider,Asset>()
, m_state( EState::start )
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
  if ( EState::start != m_state ) {
    Disconnect();
  }
}

void Provider::Connect() {

  if ( EState::start == m_state ) {

    m_state = EState::connect;

    inherited_t::Connect();

    Assets();
    Positions();
    OrderUpdates();
  }

  // TODO: indicate connected with m_bConnected = true;, OnConnected( 0 );
}

void Provider::Disconnect() {
  m_state = EState::start;
  m_pOrderUpdates->trade_updates( false ); // may need some state refinement for calling this
  m_pOrderUpdates->disconnect();
}

void Provider::Assets() {
  // The session is constructed with a strand to
  // ensure that handlers do not execute concurrently.
  auto osSymbols = std::make_shared<ou::tf::alpaca::session::one_shot>(
    asio::make_strand( m_srvc ),
    m_ssl_context
    );
  osSymbols->get(
    m_sHost, m_sPort,
    m_sAlpacaKeyId, m_sAlpacaSecret,
    "/v2/assets",
    [this]( bool bStatus, const std::string& message ){
      if ( bStatus ) {

        json::error_code jec;
        json::value jv = json::parse( message, jec );
        if ( jec.failed() ) {
          std::cout << "failed to parse /v2/assets" << std::endl;
        }
        else {
          //std::cout << message << std::endl;
          //alpaca::Asset asset( json::value_to<alpaca::Asset>( jv ) ); // single asset
          size_t nIdMisMatch {};
          Asset::vMessage_t vMessage;
          Asset::Decode( message, vMessage );
          for ( const Asset::vMessage_t::value_type& vt: vMessage ) {
            mapAssetId_t::const_iterator iter = m_mapAssetId.find( vt.symbol );
            if ( m_mapAssetId.end() != iter ) {
              const AssetMatch& am( iter->second );
              if ( ( am.sClass != vt.class_ ) && ( am.sExchange != vt.exchange ) ) {
                std::cout
                  << "asset exists: "
                  << vt.symbol
                  << "," << iter->second.sClass << "," << vt.class_
                  << "," << iter->second.sExchange << "," << vt.exchange
                  << "," << iter->second.sId << "," << vt.id
                  //<< "trade=" << vt.tradable << ","
                  //<< "short=" << vt.shortable << ","
                  //<< "margin=" << vt.marginable
                  << std::endl;
              }
              ++nIdMisMatch;
            }
            else {
              m_mapAssetId.emplace( vt.symbol, AssetMatch( vt.id, vt.class_, vt.exchange ) );
            }
          }

          std::cout
            << "found " << vMessage.size() << " assets, "
            << nIdMisMatch << " duplicated"
            << std::endl;

        }
      }
      else {
        std::cout << "os Symbol List Retrieval problems: " << message << std::endl;
      }
    }
  );
}

void Provider::Positions() {
  auto osPositions = std::make_shared<ou::tf::alpaca::session::one_shot>(
    asio::make_strand( m_srvc ),
    m_ssl_context
    );
  osPositions->get(
    m_sHost, m_sPort,
    m_sAlpacaKeyId, m_sAlpacaSecret,
    "/v2/positions",
    [this]( bool bStatus, const std::string& message ){
      if ( bStatus ) {
        //std::cout << "positions: " << message << std::endl;
        json::error_code jec;
        json::value jv = json::parse( message, jec );
        if ( jec.failed() ) {
          std::cout << "failed to parse /v2/positions" << std::endl;
        }
        else {
          position::vCurrent_t vPositions;
          position::Decode( message, vPositions );
          for ( const position::vCurrent_t::value_type& position: vPositions ) {
            std::cout
              << "position " << position.symbol
              << " " << position.qty << " " << position.side
              << ", market value=" << position.market_value
              << ", current price=" << position.current_price
              << ", unrealized pl=" << position.unrealized_pl
              << std::endl;
          }
        }
      }
      else {
        std::cout << "os Position List Retrieval problems: " << message << std::endl;
      }
    }
  );
}

void Provider::OrderUpdates() {
  m_pOrderUpdates = std::make_shared<ou::tf::alpaca::session::web_socket>(
    m_srvc, m_ssl_context
  );
  m_pOrderUpdates->connect(
    m_sHost, m_sPort,
    m_sAlpacaKeyId, m_sAlpacaSecret,
    [this] (bool ){
      m_pOrderUpdates->trade_updates( true );
    },
    [this]( std::string&& sMessage){
      std::cout << "order update message: " << sMessage << std::endl;
      json::error_code jec;
      json::value jv = json::parse( sMessage, jec );
      if ( jec.failed() ) {
        std::cout << "failed to parse web_socket stream" << std::endl;
      }
      else {

        // TODO: encase in try/catch

        struct Stream {
          std::string sType;
          json::object object;
        } stream;

        json::object const& obj = jv.as_object();
        extract( obj, stream.sType, "stream" );
        extract( obj, stream.object, "data" );

        // todo use kvm or spirit to parse
        bool bFound( false );

        if ( "authorization" == stream.sType ) {
          bFound = true;
          std::cout << "authorization: " << stream.object << std::endl;
          // {"stream":"authorization","data":{"action":"authenticate","status":"authorized"}}

          struct Auth {
            std::string sAction;
            std::string sStatus;
          } auth;

          extract( stream.object, auth.sAction, "action" );
          assert( "authenticate" == auth.sAction );
          extract( stream.object, auth.sStatus, "status" );
          assert( "authorized" == auth.sStatus );

          m_state = EState::authorized;
        }

        if ( "listening" == stream.sType ) {
          bFound = true;
          std::cout << "listening status: " << stream.object << std::endl;
          // {"stream":"listening","data":{"streams":["trade_updates"]}}

          json::array objStreams;
          extract( stream.object, objStreams, "streams" );
          //assert( "[\"trade_updates\"]" == json::to( objStreams ) );

          m_state = EState::listening;
        }

        if ( "trade_updates" == stream.sType ) {
          bFound = true;
          std::cout << "trade update: " << stream.object << std::endl;
          // {"stream":"trade_updates","data":{"event":"new",
          // {"stream":"trade_updates","data":{"event":"fill",
          OrderUpdate( stream.object );
        }
        if ( !bFound ) {
          std::cout << "unknown order update message: " << sMessage << std::endl;
        }
      }
    }
  );
}

void Provider::OrderUpdate( const json::object& obj ) {

  struct Update {
    std::string event;
    std::string execution_id;
    json::value order;
    std::string position_qty;
    std::string price;
    std::string qty;
    std::string timestamp;
  } update;

  extract( obj, update.event, "event" );
  extract( obj, update.execution_id, "execution_id" );
  extract( obj, update.order, "order" );
  extract( obj, update.position_qty, "position_qty" );
  extract( obj, update.price, "price" );
  extract( obj, update.qty, "qty" );
  extract( obj, update.timestamp, "timestamp" );

  order::Status status;
  order::Decode( update.order, status );

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
  //request[ "symbol" ] = "675f7911-9aca-418a-acd5-07cfacb9d32b"; //trd.idInstrument; GLD
  request[ "symbol" ] = trd.idInstrument;
  request[ "qty" ] = boost::lexical_cast<std::string>( trd.nOrderQuantity );
  request[ "notional" ] = nullptr;
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
      //request[ "limit_price" ] = nullptr;
      //request[ "stop_price" ] = nullptr;
      //request[ "trail_price" ] = nullptr;
      //request[ "trail_percent" ] = nullptr;
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
  //request[ "client_order_id" ] = "1";
  request[ "order_class" ] = "simple";

  auto os = std::make_shared<ou::tf::alpaca::session::one_shot>(
    asio::make_strand( m_srvc ),
    m_ssl_context
  );
  std::cout << "order '" << json::serialize( request ) << "'" << std::endl;
  os->post(
    m_sHost, m_sPort,
    m_sAlpacaKeyId, m_sAlpacaSecret,
    "/v2/orders", json::serialize( request ),
    []( bool bResult, const std::string& s ) {
      if ( bResult ) {
        std::cout << "place order result: " << s << std::endl;
      }
      else {
        std::cout << "place order error: " << s << std::endl;
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

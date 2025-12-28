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

#include <stdexcept>

#include <boost/json.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/asio/strand.hpp>

#include <TFTrading/OrderManager.h>

#include "one_shot.hpp"
#include "web_socket.hpp"
// this needs to be factored out properly
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


Provider::Provider()
: ProviderInterface<Provider,Asset>()
, m_kwmEvent( EEvent::unknown, 20 )
, m_state( EState::start )
, m_ssl_context( ssl::context::tlsv12_client )
{
  m_sName = "alpaca"; // this needs to match provider used in the database
  m_nID = keytypes::EProviderAlpaca;
  m_bProvidesBrokerInterface = true;

  m_kwmEvent.AddPattern( "new", EEvent::new_ );
  m_kwmEvent.AddPattern( "fill", EEvent::fill );
  m_kwmEvent.AddPattern( "partial_fill", EEvent::partial_fill );
  m_kwmEvent.AddPattern( "canceled", EEvent::canceled );
  m_kwmEvent.AddPattern( "expired", EEvent::expired );
  m_kwmEvent.AddPattern( "done_for_day", EEvent::done_for_day );
  m_kwmEvent.AddPattern( "replaced", EEvent::replaced );
  m_kwmEvent.AddPattern( "rejected", EEvent::rejected );
  m_kwmEvent.AddPattern( "pending_new", EEvent::pending_new );
  m_kwmEvent.AddPattern( "stopped", EEvent::stopped );
  m_kwmEvent.AddPattern( "pending_cancel", EEvent::pending_cancel );
  m_kwmEvent.AddPattern( "pending_replace", EEvent::pending_replace );
  m_kwmEvent.AddPattern( "calculated", EEvent::calculated );
  m_kwmEvent.AddPattern( "suspended", EEvent::suspended );
  m_kwmEvent.AddPattern( "order_replace_rejected", EEvent::order_replace_rejected );
  m_kwmEvent.AddPattern( "order_cancel_rejected", EEvent::order_cancel_rejected );

  if ( 0 == GetThreadCount() ) {
    SetThreadCount( 1 ); // need at least one thread for websocket processing
  }

  // This holds the root certificate used for verification
  // NOTE: this needs to be fixed, based upon comments in the include header
  ou::load_root_certificates( m_ssl_context );

  // Verify the remote server's certificate
  m_ssl_context.set_verify_mode( ssl::verify_peer );

}

Provider::~Provider() {
  if ( EState::start != m_state ) {
    Disconnect();
  }
  m_pTradeUpdates.reset();
  m_mapAssetId.clear();
  m_umapOrderLookup.clear();
}

void Provider::Set( const std::string& sHost, const std::string& sKey, const std::string& sSecret ) {
  m_sHost = sHost;
  m_sPort = "443";
  m_sAlpacaKeyId = sKey;
  m_sAlpacaSecret = sSecret;
}

void Provider::Connect() {

  if ( !m_bConnected ) {

    ProviderInterfaceBase::OnConnecting( 0 );

    assert( 0 < m_sHost.size() );
    assert( 0 < m_sPort.size() );
    assert( 0 < m_sAlpacaKeyId.size() );
    assert( 0 < m_sAlpacaSecret.size() );

    if ( EState::start == m_state ) {

      m_state = EState::connect;

      inherited_t::Connect();

      LastOrderId();
      Assets();
      Positions();
      TradeUpdates();
    }

  }
}

void Provider::Disconnect() {
  if ( m_bConnected ) {
    ProviderInterfaceBase::OnDisconnecting( 0 );
    m_state = EState::start;
    m_pTradeUpdates->trade_updates( false ); // may need some state refinement for calling this
    m_pTradeUpdates->disconnect();
    m_bConnected = false;
    ProviderInterfaceBase::OnDisconnected( 0 );
  }
}

void Provider::Assets() {
  // The session is constructed with a strand to
  // ensure that handlers do not execute concurrently.

  m_mapAssetId.clear();
  m_setExchange.clear();
  m_setClass.clear();

  auto os = std::make_shared<ou::tf::alpaca::session::one_shot>(
    asio::make_strand( m_srvc ),
    m_ssl_context
    );
  os->get(
    m_sHost, m_sPort,
    m_sAlpacaKeyId, m_sAlpacaSecret,
    "/v2/assets",
    [this]( bool bStatus, const std::string& message ){
      if ( bStatus ) {

        boost::system::error_code jec;
        json::value jv = json::parse( message, jec );
        if ( jec.failed() ) {
          BOOST_LOG_TRIVIAL(error) << "provder/alpaca failed to parse /v2/assets";
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
                BOOST_LOG_TRIVIAL(warning)
                  << "provider/alpaca asset exists: "
                  << vt.symbol
                  << "," << iter->second.sClass << "," << vt.class_
                  << "," << iter->second.sExchange << "," << vt.exchange
                  << "," << iter->second.sId << "," << vt.id
                  //<< "trade=" << vt.tradable << ","
                  //<< "short=" << vt.shortable << ","
                  //<< "margin=" << vt.marginable
                  ;
              }
              ++nIdMisMatch;
            }
            else {
              m_mapAssetId.emplace( vt.symbol, AssetMatch( vt.id, vt.class_, vt.exchange ) );
              if ( m_setExchange.end() == m_setExchange.find( vt.exchange ) ) {
                m_setExchange.emplace( vt.exchange );
              }
              if ( m_setClass.end() == m_setClass.find( vt.class_ ) ) {
                m_setClass.emplace( vt.class_ );
              }
            }
          }

          std::cout
            << "alpaca found " << vMessage.size() << " assets, "
            << nIdMisMatch << " duplicated"
            << std::endl;

          std::cout << "exchanges: ";
          bool bComma( false );
          for ( const setExchange_t::value_type& vt: m_setExchange ) {
            if ( bComma ) std::cout << ',';
            else bComma = true;
            std::cout << vt;
          }
          std::cout << std::endl;

          std::cout << "classes: ";
          bComma = false;
          for ( const setClass_t::value_type& vt: m_setClass ) {
            if ( bComma ) std::cout << ',';
            else bComma = true;
            std::cout << vt;
          }
          std::cout << std::endl;

        }
      }
      else {
        BOOST_LOG_TRIVIAL(error) << "provider/alpaca Symbol List Retrieval problems: " << message;
      }
    }
  );
}

void Provider::Positions() {
  auto os = std::make_shared<ou::tf::alpaca::session::one_shot>(
    asio::make_strand( m_srvc ),
    m_ssl_context
    );
  os->get(
    m_sHost, m_sPort,
    m_sAlpacaKeyId, m_sAlpacaSecret,
    "/v2/positions",
    [this]( bool bStatus, const std::string& message ){
      if ( bStatus ) {
        //std::cout << "positions: " << message << std::endl;
        boost::system::error_code jec;
        json::value jv = json::parse( message, jec );
        if ( jec.failed() ) {
          BOOST_LOG_TRIVIAL(error) << "provider/alpaca failed to parse /v2/positions";
        }
        else {
          position::vCurrent_t vPositions;
          position::Decode( message, vPositions );
          for ( const position::vCurrent_t::value_type& position: vPositions ) {
            std::cout
              << "alpaca position " << position.symbol
              << " " << position.qty << " " << position.side
              << ", market value=" << position.market_value
              << ", current price=" << position.current_price
              << ", unrealized pl=" << position.unrealized_pl
              << std::endl;
          }
        }
      }
      else {
        BOOST_LOG_TRIVIAL(error) << "provider/alpaca Position List Retrieval problems: " << message;
      }
    }
  );
}

void Provider::TradeUpdates() {
  m_pTradeUpdates = std::make_shared<ou::tf::alpaca::session::web_socket>(
    m_srvc, m_ssl_context
  );
  m_pTradeUpdates->connect(
    m_sHost, m_sPort,
    m_sAlpacaKeyId, m_sAlpacaSecret,
    [this] (bool ){ // fConnected_t
      m_pTradeUpdates->trade_updates( true );
      m_bConnected = true;
      ProviderInterfaceBase::OnConnected( 0 );
    },
    [this]( std::string&& sMessage){ // fMessage_t
      //std::cout << "order update message: " << sMessage << std::endl;
      boost::system::error_code jec;
      json::value jv = json::parse( sMessage, jec );
      if ( jec.failed() ) {
        BOOST_LOG_TRIVIAL(error) << "provider/alpaca failed to parse web_socket stream: " << sMessage;
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
          BOOST_LOG_TRIVIAL(info) << "authorization: " << stream.object;
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
          BOOST_LOG_TRIVIAL(error) << "listening status: " << stream.object << std::endl;
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
          TradeUpdate( stream.object );
        }
        if ( !bFound ) {
          BOOST_LOG_TRIVIAL(warning) << "provider/alpaca unknown order update message: " << sMessage << std::endl;
        }
      }
    }
  );
}

void Provider::TradeUpdate( const json::object& obj ) {

  struct Update {
    std::string id;
    std::string event;
    std::string execution_id;
    std::string timestamp;

    std::string position_qty;
    std::string price;
    std::string qty;
  } update;

  //json::value order;
  order::Status status;

  extract( obj, update.event, "event" );
  extract( obj, update.timestamp, "timestamp" );

  EEvent event = m_kwmEvent.FindMatch( update.event );
  switch ( event ) {
    case EEvent::new_:
      {
        //extract( obj, update.id, "id" );
        json::object order;
        extract( obj, order, "order" );
        //order::Decode( order, status ); // is the full decode necessary, just extract other desired fields?
        std::string id;
        extract( order, id, "id" );
        //std::string sIdOrder;
        //extract( order, sIdOrder, "client_order_id" );
        //try {
          ou::tf::Order::idOrder_t idOrder;
          //idOrder = boost::lexical_cast<ou::tf::Order::idOrder_t>( status.client_order_id );

          //umapOrderLookup_t::iterator iter = m_umapOrderLookup.find( id );
          //assert( m_umapOrderLookup.end() != iter );
          //OrderManager::GlobalInstance().UpdateReference( iter->second, id );
        //}
        //catch ( boost::bad_lexical_cast& e ) {
        //  std::cout << "alpaca provider: can not decode order " << status.client_order_id << std::endl;
        //}
      }
      break;
    case EEvent::partial_fill:
      {
        json::value order;

        extract( obj, update.qty, "qty" );
        extract( obj, update.price, "price" );
        extract( obj, update.execution_id, "execution_id" );

        extract( obj, order, "order" );
        order::Decode( order, status );
        OrderSide::EOrderSide side( OrderSide::Unknown );
        if ( "sell" == status.side ) side = OrderSide::Sell;
        if ( "buy"  == status.side ) side = OrderSide::Buy;
        auto price = boost::lexical_cast<double>( update.price );
        auto volume = boost::lexical_cast<ou::tf::Price::volume_t>( update.qty );
        ou::tf::Execution exec(
          price,
          volume,
          side,
          std::string( "alpaca" ),
          update.execution_id
        );
        //ou::tf::Order::idOrder_t idOrder;
        //idOrder = boost::lexical_cast<ou::tf::Order::idOrder_t>( status.client_order_id );
        umapOrderLookup_t::iterator iter = m_umapOrderLookup.find( status.id );
        if ( m_umapOrderLookup.end() != iter ) { // there may be unknown manual orders
          OrderManager::GlobalInstance().ReportExecution( iter->second->GetOrderId(), exec );
        }
      }
      break;
    case EEvent::fill:
      try {
        json::value order; // TODO: use object instead like with new_?

        extract( obj, update.qty, "qty" );
        extract( obj, update.price, "price" );
        extract( obj, update.execution_id, "execution_id" );

        extract( obj, order, "order" );
        order::Decode( order, status );
        OrderSide::EOrderSide side( OrderSide::Unknown );
        if ( "sell" == status.side ) side = OrderSide::Sell;
        if ( "buy"  == status.side ) side = OrderSide::Buy;
        auto price = boost::lexical_cast<double>( update.price );
        auto volume = boost::lexical_cast<ou::tf::Price::volume_t>( update.qty );
        ou::tf::Execution exec(
          price,
          volume,
          side,
          std::string( "alpaca" ),
          update.execution_id
        );
        //ou::tf::Order::idOrder_t idOrder;
        //idOrder = boost::lexical_cast<ou::tf::Order::idOrder_t>( status.client_order_id );
        umapOrderLookup_t::iterator iter = m_umapOrderLookup.find( status.id );
        if ( m_umapOrderLookup.end() != iter ) { // there may be unknown manual orders
          OrderManager::GlobalInstance().ReportExecution( iter->second->GetOrderId(), exec );
        }
      }
      catch(...) {
        BOOST_LOG_TRIVIAL(error) << "provider/alpaca EEVent::fill broke";
      }
      break;
    case EEvent::canceled:
      {
        json::value order; // TODO: use object instead?
        extract( obj, order, "order" );
        order::Decode( order, status );
        //ou::tf::Order::idOrder_t idOrder;
        //idOrder = boost::lexical_cast<ou::tf::Order::idOrder_t>( status.client_order_id );
        umapOrderLookup_t::iterator iter = m_umapOrderLookup.find( status.id );
        if ( m_umapOrderLookup.end() != iter ) { // there may be unknown manual orders
          OrderManager::GlobalInstance().ReportCancellation( iter->second->GetOrderId() );
        }
      }
      break;
    case EEvent::expired:
      //break;
    case EEvent::done_for_day:
      //break;
    case EEvent::replaced:
      //break;
    case EEvent::rejected:
      //break;
    case EEvent::pending_new:
      //break;
    case EEvent::stopped:
      //break;
    case EEvent::pending_cancel:
      //break;
    case EEvent::pending_replace:
      //break;
    case EEvent::calculated:
      //break;
    case EEvent::suspended:
      //break;
    case EEvent::order_replace_rejected:
      //break;
    case EEvent::order_cancel_rejected:
      //break;
    case EEvent::unknown:
      assert( false ); // fix as they occur
      break;
  }

}

Provider::pSymbol_t Provider::NewCSymbol( pInstrument_t pInstrument ) {
  pSymbol_t pSymbol( new Asset( pInstrument->GetInstrumentName( ID() ), pInstrument ) );
  inherited_t::AddCSymbol( pSymbol );
  return pSymbol;
}

void Provider::LastOrderId() {
  json::object request;
  request[ "status" ] = "all";
  //request[ "limit" ] = 10;
  //request[ "direction" ] = "desc";
  std::cout
    << "order id request: "
    << json::serialize( request )
    << std::endl;

  auto os = std::make_shared<ou::tf::alpaca::session::one_shot>(
    asio::make_strand( m_srvc ),
    m_ssl_context
    );
  os->get(
    m_sHost, m_sPort,
    m_sAlpacaKeyId, m_sAlpacaSecret,
    "/v2/orders?status=all&limit=1&direction=desc",
    [this]( bool bStatus, const std::string& message ){
      if ( bStatus ) {
        //std::cout << "order list for id: " << message << std::endl;
        order::vOrderId_t vOrderId;
        order::Decode( message, vOrderId );
        if ( 1 < vOrderId.size() ) {
          BOOST_LOG_TRIVIAL(error) << "provider/alpaca order id decode has more than 1 entry";
        }
        else {
          if ( 1 == vOrderId.size() ) {
            try {
              // as we can't maintain uniqueness of our own order id, don't bother trying to decode what comes back
              //auto idOrder = boost::lexical_cast<ou::tf::OrderManager::idOrder_t>( vOrderId[ 0 ].client_order_id );
              //ou::tf::OrderManager& om( ou::tf::OrderManager::GlobalInstance() );
              //om.CheckOrderId( idOrder ); // put this into state file
              //std::cout << "OrderManager assigned idOrder: " << idOrder << std::endl;
              std::cout << "OrderManager last client_order_id: " << vOrderId[ 0 ].client_order_id << std::endl;
            }
            catch ( boost::bad_lexical_cast& e ) {
              BOOST_LOG_TRIVIAL(error) << "provider/alpaca LastOrderId: can not decode order " << vOrderId[ 0 ].client_order_id;
            }
          }
        }
      }
      else {
        BOOST_LOG_TRIVIAL(error) << "provider/alpaca Order List Retrieval problems: " << message;
      }
    }
  );
}

void Provider::PlaceOrder( pOrder_t pOrder ) {

  const ou::tf::Order& order(*pOrder);
  const ou::tf::Order::TableRowDef& trd( order.GetRow() );
  json::object request;
  //request[ "client_order_id" ] = boost::lexical_cast<std::string>( trd.idOrder ); // don't send this
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
      {
        boost::format format( "%0.2f" ); // TODO: refactor this
        format % ( std::round( trd.dblPrice1 / 0.01 ) * 0.01 );
        request[ "limit_price"] = format.str();
      }
      break;
    case OrderType::Stop:
      request[ "type" ] = "stop";
      {
        boost::format format( "%0.2f" ); // TODO: refactor this
        format % ( std::round( trd.dblPrice1 / 0.01 ) * 0.01 );
        request[ "stop_price" ] = format.str();
      }
      break;
    case OrderType::StopLimit:
      request[ "type" ] = "stop_limit";
      {
        boost::format format( "%0.2f" ); // TODO: refactor this
        format % ( std::round( trd.dblPrice1 / 0.01 ) * 0.01 );
        request[ "limit_price" ] = format.str();
        format % ( std::round( trd.dblPrice2 / 0.01 ) * 0.01 );
        request[ "stop_price"]   = format.str();
      }
      break;
    default:
      assert( false );
      break;
  }
  request[ "time_in_force" ] = "day";
  request[ "order_class" ] = "simple";

  inherited_t::PlaceOrder( pOrder ); // any underlying initialization

  auto os = std::make_shared<ou::tf::alpaca::session::one_shot>(
    asio::make_strand( m_srvc ),
    m_ssl_context
  );
  std::cout << "provider/alpaca order '" << json::serialize( request ) << "'" << std::endl;
  os->post(
    m_sHost, m_sPort,
    m_sAlpacaKeyId, m_sAlpacaSecret,
    "/v2/orders", json::serialize( request ),
    [this,pOrder]( bool bResult, const std::string& s ) {
      if ( bResult ) {

        // std::cout << "place order result: " << s << std::endl;

        boost::system::error_code jec;
        json::value jv = json::parse( s, jec );
        if ( jec.failed() ) {
          BOOST_LOG_TRIVIAL(error) << "provider/alpaca - failed to parse order result: " << s;
        }
        else {
          try {

            json::object const& obj = jv.as_object();

            if ( obj.contains( "code" ) ) { // has an error

              struct ResultDecoded {
                uint64_t code;
                std::string message;
              } result;

              extract( obj, result.code, "code" );
              extract( obj, result.message, "message" );

              switch ( result.code ) {
                case 40010001: // client_order_id must be unique
                case 40310000: //
                case 42210000: // sub-penny problems on an equity, need to perform rounding
                  BOOST_LOG_TRIVIAL(warning) << "provider/alpaca - order not placed: (" << result.code << ") " << result.message;
                  // TODO: need to cancel the order locally
                  break;
                  //assert( false );
                default:
                  BOOST_LOG_TRIVIAL(error) << "provider/alpaca - place order unhandled error: " << s;
                  //assert( false );  // abort to catch other possibilities
                  break;
              }
            }
            else { // has order status
              order::Status status;
              extract( obj, status.id, "id" );
              extract( obj, status.status, "status" );
              m_umapOrderLookup.emplace( status.id, pOrder );  // use this when trade lifecycle processing
              idOrder_t idOrder( pOrder->GetOrderId() );
              std::cout << "order id " << idOrder << " has reference " << status.id << std::endl;
              //umapOrderLookup_t::iterator iter = m_umapOrderLookup.find( id );
              //assert( m_umapOrderLookup.end() != iter );
              OrderManager::GlobalInstance().UpdateReference( idOrder, status.id );
            }

          }
          catch ( std::out_of_range& error ) {
            BOOST_LOG_TRIVIAL(error) << "provider/alpaca place order exception: " << s;
            assert( false );
          }
        }
      }
      else {
        BOOST_LOG_TRIVIAL(error) << "provider/alpaca - place order one_shot error: " << s;
      }
    }
  );
}

void Provider::CancelOrder( pOrder_t pOrder ) {

  const ou::tf::Order& order( *pOrder ); // need to check the reference exists
  assert( 0 < order.GetRow().sReference.size() );

  auto os = std::make_shared<ou::tf::alpaca::session::one_shot>(
    asio::make_strand( m_srvc ),
    m_ssl_context
  );

  os->delete_(
    m_sHost, m_sPort,
    m_sAlpacaKeyId, m_sAlpacaSecret,
    std::string( "/v2/orders/" + order.GetRow().sReference ),
    //"/v2/orders/", // TODO: need broker supplied order id
    []( bool bResult, const std::string& s ) { // TODO: decode the result here, probably won't show up in the stream?
      if ( bResult ) { // any processing required on either true/false side?
        BOOST_LOG_TRIVIAL(info) << "provider/alpaca cancel order result: " << s;
      }
      else {
        BOOST_LOG_TRIVIAL(error) << "provider/alpaca cancel order error: " << s;
      }
    }
  );
  inherited_t::CancelOrder( pOrder );
}

} // namespace alpaca
} // namespace tf
} // namespace ou

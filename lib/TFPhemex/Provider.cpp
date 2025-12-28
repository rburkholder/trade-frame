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
 * Project: lib/TFPhemex
 * Created: July 24, 2022 12:19
 */

#include <ctime>

#include <cryptopp/base64.h>

#include <openssl/sha.h>
#include <openssl/hmac.h>

#include <boost/json.hpp>
#include <boost/asio/strand.hpp>
#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>

#include <TFTrading/OrderManager.h>

#include "one_shot.hpp"
#include "web_socket.hpp"
#include "root_certificates.hpp" // this needs to be factored out properly

#include "Provider.hpp"
#include "GatewayTrades.hpp"

namespace json = boost::json;

namespace ou {
namespace tf {
namespace phemex {

Provider::Provider()
: ProviderInterface<Provider,Symbol>()
, m_ssl_context( ssl::context::tlsv12_client )
{
  m_sName = "phemex"; // this needs to match provider used in the database
  m_nID = keytypes::EProviderPhemex;
//  m_bProvidesQuotes = true; // need to derive from order table
  m_bProvidesTrades = true;
//m_bProvidesBrokerInterface = true;

  if ( 0 == GetThreadCount() ) { // affects m_srvc
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
}

void Provider::Set(
  const std::string& sDomainAPI, const std::string& sDomainWS,
  const std::string& sKey, const std::string& sSecret
) {

  m_sDomainAPI = sDomainAPI;
  m_sDomainWS = sDomainWS;
  m_sPort = "443";
  m_sKeyId = sKey;

  assert( 0 < sSecret.size() );

  // https://www.cryptopp.com/wiki/Base64URLDecoder -- is there a libssl function to do this
  CryptoPP::Base64Decoder decoder;
  decoder.Put( (CryptoPP::byte const*)sSecret.data(), sSecret.size() );
  decoder.MessageEnd();

  auto size = decoder.MaxRetrievable();

  m_sDecodedSecret.resize( size );
  decoder.Get( (CryptoPP::byte*)m_sDecodedSecret.data(), m_sDecodedSecret.size() );
}

Provider::pSymbol_t Provider::NewCSymbol( pInstrument_t pInstrument ) {
  // TODO: perform validation: name exists in the product table
  //   place product table data into symbol for reference by client
  pSymbol_t pSymbol;
  try {
    pSymbol = boost::make_shared<Symbol>( pInstrument->GetInstrumentName( ID() ), pInstrument );
    inherited_t::AddCSymbol( pSymbol );
  }
  catch ( const std::runtime_error& e ) {
    std::cerr << "phemex/NewCSymbol error: " << e.what() << std::endl;
    assert( false );
  }
  return pSymbol;
}

void Provider::Connect() {

  if ( !m_bConnected ) {

    ProviderInterfaceBase::OnConnecting( 0 );

    assert( 0 < m_sDomainAPI.size() );
    assert( 0 < m_sDomainWS.size() );
    assert( 0 < m_sPort.size() );
    assert( 0 < m_sKeyId.size() );

    if ( EState::start == m_state ) {

      m_state = EState::connect;

      inherited_t::Connect();

      //GetProducts();
      DataGateWayUp();
      //LastOrderId();
      //Positions();
    }

  }
}

void Provider::Disconnect() {
  if ( m_bConnected ) {
    ProviderInterfaceBase::OnDisconnecting( 0 );
    if ( m_pDataGateWay ) {
      m_pDataGateWay->disconnect();
      m_pDataGateWay.reset();
    }
  }
}

void Provider::GetProducts() {

  time_t seconds = std::time( 0 );
  seconds += 60; // expiry for phemex processing

  static const std::string sRequestPath( "/public/products" );
  //static const std::string sRequestQuery( "" );
  //static const std::string sRequestBody( "" );

  const std::string sExpiry( boost::lexical_cast<std::string>( seconds ) );
  const std::string sForSignature( sRequestPath + sExpiry );

  // Calculate HMAC
  //CryptoPP::HMAC<CryptoPP::SHA256> hmac(reinterpret_cast<CryptoPP::byte const*>(m_sDecodedSecret.data()), m_sDecodedSecret.size());

  //std::string calculated_hmac;
  //auto sink = std::make_unique<CryptoPP::StringSink>( calculated_hmac );

  //auto filter = std::make_unique<CryptoPP::HashFilter>( hmac, sink.get() );
  //sink.release();

  //CryptoPP::StringSource( reinterpret_cast<CryptoPP::byte const*>( sQuery.data()), sQuery.size(), true, filter.get() ); // StringSource
  //filter.release();

  // use the ssl version as the cryptopp seems to use lots of heap stuff with make_unique

  std::array<unsigned char, EVP_MAX_MD_SIZE> hash;
  unsigned int hashLen;

  HMAC(
    EVP_sha256(),
    m_sDecodedSecret.data(),
    static_cast<int>(m_sDecodedSecret.size()),
    reinterpret_cast<unsigned char const*>( sForSignature.data() ),
    static_cast<int>( sForSignature.size() ),
    hash.data(),
    &hashLen
  );

  std::string sHash( reinterpret_cast<char const*>(hash.data()), hashLen );

  auto osProducts = std::make_shared<ou::tf::phemex::session::one_shot>(
    asio::make_strand( m_srvc ),
    m_ssl_context
    );
  osProducts->get(
    m_sDomainAPI, m_sPort,
    m_sKeyId, sHash, sExpiry,
    sRequestPath,
    [this]( bool bStatus, const std::string& message ){
      if ( bStatus ) {
        try {
          boost::system::error_code jec;
          json::value jv = json::parse( message, jec );
          if ( jec.failed() ) {
            BOOST_LOG_TRIVIAL(error) << "provider/phemex failed to parse " << sRequestPath;
            std::cout << "products: " << message << std::endl;
          }
          else {
            json::object const& obj = jv.as_object();
            int code;
            code = json::value_to<int>( obj.at( "code" ) );
            if ( 0 != code ) {
              std::string msg;
              msg = json::value_to<std::string>( obj.at( "msg" ) );
              BOOST_LOG_TRIVIAL(error) << "provider/phemex " << sRequestPath << " error: " << msg;
            }
            else {
              json::object data = json::value_to<json::object>( obj.at( "data" ) );

              m_ratioScale = json::value_to<unsigned int>( data.at( "ratioScale" ) );

              json::array currencies = json::value_to<json::array>( data.at( "currencies" ) );
              Decode( currencies, m_vCurrency );

              json::array products   = json::value_to<json::array>( data.at( "products" ) );
              Decode( products, m_vProduct );

              json::array riskLimits = json::value_to<json::array>( data.at( "riskLimits" ) );
              Decode( riskLimits, m_vriskLimits );

              json::array leverages  = json::value_to<json::array>( data.at( "leverages" ) );
              Decode( leverages, m_vLeverages );

              std::string md5Checksum = json::value_to<std::string>( data.at( "md5Checksum" ) );

              BOOST_LOG_TRIVIAL(error)
                << "provider/phemex"
                << " ratioScale=" << m_ratioScale
                << " currencies=" << m_vCurrency.size()
                << " products=" << m_vProduct.size()
                << " riskLimits=" << m_vriskLimits.size()
                << " leverages=" << m_vLeverages.size()
                ;
            }
          }
        }
        catch (...) {
          BOOST_LOG_TRIVIAL(error) << "provider/phemex " << sRequestPath << " exception";
        }
      }
      else {
        BOOST_LOG_TRIVIAL(error) << "provider/phemex " << sRequestPath << " failure: " << message;
      }
    });
}

void Provider::DataGateWayUp() {
  m_pDataGateWay = std::make_shared<ou::tf::phemex::session::web_socket>(
    m_srvc, m_ssl_context
  );
  m_pDataGateWay->connect(
    m_sDomainWS, m_sPort,
    [this] (bool ){ // fConnected_t
      m_bConnected = true;
      ProviderInterfaceBase::OnConnected( 0 );
    },
    [this](){ // fDisconnected_t
      m_bConnected = false;
      m_state = EState::start;
      //m_pTradeUpdates->trade_updates( false ); // may need some state refinement for calling this
      //m_pTradeUpdates->disconnect();
      ProviderInterfaceBase::OnDisconnected( 0 );
    },
    [this]( std::string&& sMessage){ // fMessage_t
      boost::system::error_code jec;
      json::value jv = json::parse( sMessage, jec );
      if ( jec.failed() ) {
        BOOST_LOG_TRIVIAL(error) << "provider/phemex failed to parse web_socket stream: " << sMessage;
      }
      else { // TODO: use spirit here at some point
        bool bMessageProcessed( false );
        json::object const& obj = jv.as_object();
        if ( auto p = obj.if_contains( "error" ) ) {
          int id = json::value_to<int>( obj.at( "id" ) );
          switch ( id ) {
            case (int)session::web_socket::EMessageId::HeartBeat:
              // todo: signal back into web_socket for timeout reset
              break;
            case (int)session::web_socket::EMessageId::StartTradeWatch:
              if ( !obj.at( "error" ).is_null() ) {
                json::object error = json::value_to<json::object>( obj.at( "error" ) );
                BOOST_LOG_TRIVIAL(error)
                  << "provider/phemex gw start watch: " << sMessage;
              }
              break;
            case (int)session::web_socket::EMessageId::StopTradeWatch:
              if ( !obj.at( "error" ).is_null() ) {
                json::object error = json::value_to<json::object>( obj.at( "error" ) );
                BOOST_LOG_TRIVIAL(error)
                  << "provider/phemex gw stop watch: " << sMessage;
              }
              break;
            default:
              BOOST_LOG_TRIVIAL(error) << "provider/phemex gw error: " << sMessage;
              break;
          }
          bMessageProcessed = true;
        }
        else {
          if ( auto p = obj.if_contains( "trades" ) ) {
            uint64_t sequence = json::value_to<uint64_t>( obj.at( "sequence" ) );
            std::string sSymbol = json::value_to<std::string>( obj.at( "symbol" ) );
            json::array trades = json::value_to<json::array>( obj.at( "trades" ) );
            std::string sType = json::value_to<std::string>( obj.at( "type" ) );

            mapSymbols_t::iterator iterSymbol = m_mapSymbols.find( sSymbol );
            if ( m_mapSymbols.end() == iterSymbol ) {
              BOOST_LOG_TRIVIAL(error) << "provider/phemex DataGateway can not find symbol " << sSymbol;
            }
            else {
              if ( "snapshot" == sType ) {}
              if ( "incremental" == sType ) {
                gateway::trades::v_trade_t v_trade;
                try {
                  for ( const auto& item: trades ) {

                    json::array trade_items = json::value_to<json::array>( item );
                    json::array::const_iterator citer = trade_items.begin();

                    v_trade.push_back( gateway::trades::trade() );
                    auto& back( v_trade.back() );
                    back.time_stamp = json::value_to<uint64_t>( *citer ); citer++;
                    back.side = json::value_to<std::string>( *citer ); citer++;
                    back.price = json::value_to<uint64_t>( *citer ); citer++;
                    back.quantity = json::value_to<uint64_t>( *citer );
                  }
                }
                catch (...) {
                  BOOST_LOG_TRIVIAL(error) << "provider/phemex DataGateWay error";
                }

                uint64_t value1 {}, value2 {};
                for ( gateway::trades::v_trade_t::const_reverse_iterator iter = v_trade.rbegin(); v_trade.rend() != iter; iter++ ) {
                  value2 = iter->time_stamp;
                  if ( value2 < value1 ) {
                    BOOST_LOG_TRIVIAL(error)
                      << "phemex::DataGateWay trades not in expected sequence: "
                      << value1 << "," << value2
                      ;
                  }
                  value1 = value2;
                  iterSymbol->second->HandleTrade( *iter );
                }
              }
            }
            bMessageProcessed = true;
          }
        }
        if ( !bMessageProcessed ) {
          BOOST_LOG_TRIVIAL(info) << "gateway received: " << sMessage;
        }
      }
    });
}

//void Provider::StartQuoteWatch( pSymbol_t pSymbol ) {
  // no quotes to watch for now, will need to pull from order book
//}

//void Provider::StopQuoteWatch( pSymbol_t pSymbol ) {
  // no quotes to watch for now, will need to pull from order book
//}

void Provider::StartTradeWatch( pSymbol_t pSymbol ) {
  // does inherited handle watch count?
  if ( m_pDataGateWay ) {
    m_pDataGateWay->StartTradeWatch( pSymbol->GetInstrument()->GetInstrumentName() );
  }
}

void Provider::StopTradeWatch( pSymbol_t pSymbol ) {
  // does inherited handle watch count?
  if ( m_pDataGateWay ) {
    m_pDataGateWay->StopTradeWatch( pSymbol->GetInstrument()->GetInstrumentName() );
  }
}

void Provider::PlaceOrder( pOrder_t pOrder ) {
  const ou::tf::Order& order( *pOrder );
  const ou::tf::Order::TableRowDef& trd( order.GetRow() );
}

void Provider::CancelOrder( pOrder_t pOrder ) {
}


} // namespace phemex
} // namespace tf
} // namespace ou

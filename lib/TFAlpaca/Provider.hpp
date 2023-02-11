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
 * File:    Provider.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFAlpaca
 * Created: June 5, 2022 16:04
 */

#pragma once

#include <map>
#include <set>
#include <memory>
#include <unordered_map>

#include <boost/asio/ssl.hpp>

#include <OUCommon/KeyWordMatch.h>

#include <TFTrading/ProviderInterface.h>

#include "Asset.hpp"

namespace asio = boost::asio; // from <boost/asio.hpp>
namespace ssl  = asio::ssl;   // from <boost/asio/ssl.hpp>

namespace boost {
namespace json {
  class object;
}
}

namespace ou {
namespace tf {
namespace alpaca {

namespace session {
  class web_socket;
} // namespace session

class Provider:
  public ProviderInterface<Provider, Asset>
{
public:

  using pProvider_t = std::shared_ptr<Provider>;
  using inherited_t = ProviderInterface<Provider,Asset>;
  using idSymbol_t = inherited_t::idSymbol_t ;
  using pSymbol_t = inherited_t::pSymbol_t;
  using pInstrument_t = inherited_t::pInstrument_t;
  using idOrder_t = ou::tf::Order::idOrder_t;
  using pOrder_t = ou::tf::Order::pOrder_t;

  Provider(); // for auto construction by ProviderManager
  virtual ~Provider();

  static pProvider_t Factory() {
    return std::make_shared<Provider>();
  }

  static pProvider_t Cast( inherited_t::pProvider_t pProvider ) {
    return std::dynamic_pointer_cast<Provider>( pProvider );
  }

  void Set( const std::string& sHost, const std::string& sKey, const std::string& sSecret );

  // do these need to be virtual?  use crtp?
  virtual void Connect();
  virtual void Disconnect();

protected:

  pSymbol_t NewCSymbol( pInstrument_t pInstrument );  // used by Add/Remove x handlers in base class

  // From ProviderInterface Execution Section
  virtual void PlaceOrder( pOrder_t );
  virtual void CancelOrder( pOrder_t );

private:

  enum EState { start, connect, authorized, listening, error } m_state;

  // https://alpaca.markets/deprecated/docs/api-documentation/api-v2/streaming/
  enum EEvent { new_, fill, partial_fill, canceled, expired, done_for_day
              , replaced, rejected, pending_new, stopped, pending_cancel
              , pending_replace, calculated, suspended
              , order_replace_rejected, order_cancel_rejected
              , unknown };

  ou::KeyWordMatch<EEvent> m_kwmEvent;

  ssl::context m_ssl_context;

  std::string m_sHost;
  std::string m_sPort;
  std::string m_sAlpacaKeyId;
  std::string m_sAlpacaSecret;

  using pTradeUpdates_t = std::shared_ptr<ou::tf::alpaca::session::web_socket>;
  pTradeUpdates_t m_pTradeUpdates;

  struct AssetMatch {
    std::string sId;
    std::string sClass;
    std::string sExchange;
    AssetMatch() {};
    AssetMatch( const std::string& sId_, const std::string& sClass_, const std::string& sExchange_ )
    : sId( sId_ ), sClass( sClass_ ), sExchange( sExchange_ ) {}
  };

  using mapAssetId_t = std::map<std::string,AssetMatch>; // normal name
  mapAssetId_t m_mapAssetId;

  using setExchange_t = std::set<std::string>;
  setExchange_t m_setExchange;

  using setClass_t = std::set<std::string>;
  setClass_t m_setClass;

  // TODO: remove lookup on fill or cancel
  using umapOrderLookup_t = std::unordered_map<std::string,pOrder_t>;
  umapOrderLookup_t m_umapOrderLookup;

  void Assets();
  void LastOrderId();
  void Positions();
  void TradeUpdates();

  void TradeUpdate( const boost::json::object& obj );

};

} // namespace alpaca
} // namespace tf
} // namespace ou

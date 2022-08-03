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
 * Project: lib/TFPhemex
 * Created: July 24, 2022 12:19
 */

#pragma once

#include <boost/asio/ssl.hpp>

#include <TFTrading/ProviderInterface.h>

#include "Symbol.hpp"
#include "Products.hpp"


// The default Rest API base endpoint is: https://api.phemex.com.
// The High rate limit Rest API base endpoint is: https://vapi.phemex.com.
// Or for the testnet is: https://testnet-api.phemex.com

// The WebSocket API url is: wss://phemex.com/ws.
// The High rate limit WebSocket API url is: wss://vapi.phemex.com/ws.
// Or for the testnet is: wss://testnet.phemex.com/ws

namespace asio = boost::asio; // from <boost/asio.hpp>
namespace ssl  = asio::ssl;   // from <boost/asio/ssl.hpp>

namespace boost {
namespace json {
  class object;
}
}

namespace ou {
namespace tf {
namespace phemex {

namespace session {
  class web_socket;
} // namespace session

class Provider:
  public ProviderInterface<Provider, Symbol>
{
public:

  using pProvider_t = std::shared_ptr<Provider>;
  using inherited_t = ProviderInterface<Provider,Symbol>;
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

  void Set(
    const std::string& sDomainAPI, const std::string& sDomainWS,
    const std::string& sKey, const std::string& sSecret
    );

  // do these need to be virtual?  use crtp?
  virtual void Connect();
  virtual void Disconnect();

protected:

  // overridden from ProviderInterface, called when application adds/removes watches
  //virtual void StartQuoteWatch( pSymbol_t pSymbol );
  //virtual void  StopQuoteWatch( pSymbol_t pSymbol );

  virtual void StartTradeWatch( pSymbol_t pSymbol );
  virtual void  StopTradeWatch( pSymbol_t pSymbol );

  pSymbol_t NewCSymbol( pInstrument_t pInstrument );  // used by Add/Remove x handlers in base class

private:

  enum EState { start, connect, authorized, listening, error } m_state;

  ssl::context m_ssl_context;

  std::string m_sDomainAPI;
  std::string m_sDomainWS;
  std::string m_sPort;
  std::string m_sKeyId;
  std::string m_sDecodedSecret;

  unsigned int m_ratioScale;

  using pDataGateWay_t = std::shared_ptr<ou::tf::phemex::session::web_socket>;
  pDataGateWay_t m_pDataGateWay;

  products::vCurrency_t m_vCurrency;
  products::vProduct_t m_vProduct;
  products::vriskLimits_t m_vriskLimits;
  products::vLeverages_t m_vLeverages;

  bool m_bSendHeartBeat;

  void GetProducts();
  void DataGateWayUp();

};

} // namespace phemex
} // namespace tf
} // namespace ou

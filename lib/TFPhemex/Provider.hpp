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

namespace asio  = boost::asio; // from <boost/asio.hpp>
namespace ssl   = asio::ssl;   // from <boost/asio/ssl.hpp>

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

  void Set( const std::string& sHost, const std::string& sKey, const std::string& sSecret );

protected:

  pSymbol_t NewCSymbol( pInstrument_t pInstrument );  // used by Add/Remove x handlers in base class

private:

  enum EState { start, connect, authorized, listening, error } m_state;

  ssl::context m_ssl_context;

  std::string m_sHost;
  std::string m_sPort;
  std::string m_sKeyId;
  std::string m_sSecret;

};

} // namespace phemex
} // namespace tf
} // namespace ou

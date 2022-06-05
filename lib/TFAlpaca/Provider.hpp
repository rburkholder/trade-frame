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

#include <TFTrading/ProviderInterface.h>

#include "Asset.hpp"

namespace ou {
namespace tf {
namespace alpaca {

class Provider:
  public ProviderInterface<Provider, Asset>
{
public:

  using pProvider_t = boost::shared_ptr<Provider>;
  using inherited_t = ProviderInterface<Provider,Asset>;
  using idSymbol_t = inherited_t::idSymbol_t ;
  using pSymbol_t = inherited_t::pSymbol_t;
  using pInstrument_t = inherited_t::pInstrument_t;

  Provider();
  virtual ~Provider();

  static pProvider_t Factory() {
    return boost::make_shared<Provider>();
  }

  // do these need to be virtual?  use crtp?
  virtual void Connect();
  virtual void Disconnect() {};

protected:

  pSymbol_t NewCSymbol( pInstrument_t pInstrument );  // used by Add/Remove x handlers in base class

private:
};

} // namespace alpaca
} // namespace tf
} // namespace ou

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
 * File:    Asset.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFAlpaca
 * Created: June 5, 2022 16:08
 */

#pragma once

#include <TFTrading/Symbol.h>

namespace ou {
namespace tf {
namespace alpaca {

class Provider;

class Asset
: public ou::tf::Symbol<Asset>
{
  friend class Provider;
public:

  using inherited_t = ou::tf::Symbol<Asset>;
  using pInstrument_t = inherited_t::pInstrument_t;

  // https://alpaca.markets/docs/api-references/trading-api/assets/
  struct Message {
    std::string id;
    std::string class_;
    std::string exchange;
    std::string symbol;
    std::string status;
    bool        tradable;
    bool        marginable;
    bool        shortable;
    bool        easy_to_borrow;
    bool        fractionable;
  };

  Asset( const std::string &sName, pInstrument_t pInstrument );
  virtual ~Asset();

  using vMessage_t = std::vector<Message>;
  static void Decode( const std::string&, vMessage_t& );

protected:
private:
};

} // namespace alpaca
} // namespace tf
} // namespace ou

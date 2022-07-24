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
 * File:    Symbol.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFPhemex
 * Created: July 24, 2022 12:59:37
 */

#pragma once

#include <TFTrading/Symbol.h>

namespace ou {
namespace tf {
namespace phemex {

class Provider;

class Symbol
: public ou::tf::Symbol<Symbol>
{
  friend class Provider;
public:

  using inherited_t = ou::tf::Symbol<Symbol>;
  using pInstrument_t = inherited_t::pInstrument_t;

  Symbol( const std::string &sName, pInstrument_t pInstrument );
  virtual ~Symbol();

protected:
private:
};

} // namespace phemex
} // namespace tf
} // namespace ou

/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// started 2013-05-05

// put the stuff into a database table at some point in time.

// sources:
// http://www.currency-iso.org/en/home/tables/table-a1.html

// get common currency lists from IB or IQFeed.

#pragma once

#include <string>
#include <map>

#include <boost/range.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/adaptor/map.hpp>

namespace ou { // One Unified
namespace tables { // tables
namespace CurrencyCode { // currency code

  typedef std::string idCurrency_t;

  bool IsValid( const idCurrency_t& );

namespace detail {

  typedef std::map<std::string,std::string> mapCurrencies_t;
  const mapCurrencies_t& getMap( void );

} // namespace detail


template<class F>
void ScanCurrencyCodes( F f ) {
  boost::for_each( detail::getMap() | boost::adaptors::map_keys, f );
}

} // namespace currency code
} // namespace tables
} // namespace ou

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

#include "stdafx.h"

#include <iostream>

#include <boost/tuple/tuple.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include "CurrencyCode.h"

namespace ou { // One Unified
namespace tables { // Currency
namespace CurrencyCode {
namespace detail {

#include "IsoCurrency.txt"

  struct initializer_t {
    mapCurrencies_t m_mapCurrencies;  // currency code, currency name
    initializer_t( void );
  };

  const mapCurrencies_t& getMap( void ) {
    static initializer_t initializer;
    return initializer.m_mapCurrencies;
  }

  // private initialization in structure, and static the structure
  // http://stackoverflow.com/questions/1197106/static-constructors-in-c-need-to-initialize-private-static-objects


initializer_t::initializer_t( void ) {
  for ( detail::vCurrencyCodes_t::iterator iter = detail::vCurrencyCodes.begin(); 
        iter != detail::vCurrencyCodes.end(); iter++ ) {
    m_mapCurrencies[ iter->get<2>() ] = iter->get<1>();  // 0 based offset into 5 element tuple
  }
  for ( mapCurrencies_t::iterator iter = m_mapCurrencies.begin(); iter != m_mapCurrencies.end(); iter++ ) {
    std::cout << iter->first << ":" << iter->second << std::endl;
  }
}

} // namespace detail

bool IsValid( const idCurrency_t& idCurrency ) {
  return ( detail::getMap().end() != detail::getMap().find( idCurrency ) );
}

} // namespace CurrencyCode
} // namespace tables
} // namespace ou

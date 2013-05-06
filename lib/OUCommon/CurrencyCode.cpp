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

#include <boost/tuple/tuple.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include <boost/property_tree/xml_parser.hpp>

#include "CurrencyCode.h"

namespace detail {
#include "IsoCurrency.cpp"
} // namespace detail

namespace ou { // One Unified

CurrencyCode::CurrencyCode(void) {
  for ( detail::vCurrencyCodes_t::iterator iter = detail::vCurrencyCodes.begin(); 
        iter != detail::vCurrencyCodes.end(); iter++ ) {
    m_mapCurrencies[ iter->get<2>() ] = iter->get<1>();
  }
  for ( m_mapCurrencies_t::iterator iter = m_mapCurrencies.begin(); iter != m_mapCurrencies.end(); iter++ ) {
    std::cout << iter->first << ":" << iter->second << std::endl;
  }
}

CurrencyCode::~CurrencyCode(void) {
}





} // namespace ou

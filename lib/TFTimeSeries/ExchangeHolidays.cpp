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

#include "StdAfx.h"

#include <boost/assign.hpp>
#include <boost/assign/std/set.hpp>

#include "ExchangeHolidays.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace holidays {
namespace exchange {

using namespace boost::assign;

setDates_t setUSDates = list_of
  ( boost::gregorian::date( 2013, 11, 28 ) ) // Thanksgiving
  ( boost::gregorian::date( 2013, 12, 25 ) ) // Christmas
  ( boost::gregorian::date( 2014,  1,  1 ) ) // New Years
  ( boost::gregorian::date( 2014,  1, 20 ) ) // Martin Luther
  ( boost::gregorian::date( 2014,  2, 17 ) ) // Presidents  Day
  ( boost::gregorian::date( 2014,  4, 18 ) ) // Good Friday
  ( boost::gregorian::date( 2014,  5, 26 ) ) // Memorial
  ( boost::gregorian::date( 2014,  7,  4 ) ) // Independence
  ( boost::gregorian::date( 2014,  9,  1 ) ) // Labor
  ( boost::gregorian::date( 2014, 11, 28 ) ) // Thanksgiving
  ( boost::gregorian::date( 2014, 12, 25 ) ) // Christmas
  ;


} // namespace exchange
} // namespace holidays
} // namespace tf
} // namespace ou


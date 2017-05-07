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

#include "stdafx.h"

#include <boost/assign.hpp>
#include <boost/assign/std/set.hpp>

#include "ExchangeHolidays.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace holidays {
namespace exchange {

using namespace boost::assign;

setDates_t setUSDates = list_of
//  ( boost::gregorian::date( 2013, 11, 28 ) ) // Thanksgiving
//  ( boost::gregorian::date( 2013, 12, 25 ) ) // Christmas
	
//  ( boost::gregorian::date( 2014,  1,  1 ) ) // New Years
//  ( boost::gregorian::date( 2014,  1, 20 ) ) // Martin Luther
//  ( boost::gregorian::date( 2014,  2, 17 ) ) // Presidents  Day
//  ( boost::gregorian::date( 2014,  4, 18 ) ) // Good Friday
//  ( boost::gregorian::date( 2014,  5, 26 ) ) // Memorial
//  ( boost::gregorian::date( 2014,  7,  4 ) ) // Independence
//  ( boost::gregorian::date( 2014,  9,  1 ) ) // Labor
//  ( boost::gregorian::date( 2014, 11, 28 ) ) // Thanksgiving
//  ( boost::gregorian::date( 2014, 12, 25 ) ) // Christmas
	
  ( boost::gregorian::date( 2015, 11, 26 ) ) // Thanksgiving, 13:00 est closing on 27
  ( boost::gregorian::date( 2015, 12, 25 ) ) // Christmas, 13:00 est closing on 24
	
  ( boost::gregorian::date( 2016,  1,  1 ) ) // New Years
  ( boost::gregorian::date( 2016,  1, 18 ) ) // Martin Luther
  ( boost::gregorian::date( 2016,  2, 15 ) ) // Presidents  Day
  ( boost::gregorian::date( 2016,  3, 25 ) ) // Good Friday
  ( boost::gregorian::date( 2016,  5, 30 ) ) // Memorial
  ( boost::gregorian::date( 2016,  7,  4 ) ) // Independence
  ( boost::gregorian::date( 2016,  9,  5 ) ) // Labor
  ( boost::gregorian::date( 2016, 11, 24 ) ) // Thanksgiving, 13:00 est closing on 25
  ( boost::gregorian::date( 2016, 12, 26 ) ) // Christmas
	
  ( boost::gregorian::date( 2017,  1,  1 ) ) // New Years
  ( boost::gregorian::date( 2017,  1, 16 ) ) // Martin Luther
  ( boost::gregorian::date( 2017,  2, 20 ) ) // Presidents  Day
  ( boost::gregorian::date( 2017,  4, 14 ) ) // Good Friday
  ( boost::gregorian::date( 2017,  5, 29 ) ) // Memorial
  ( boost::gregorian::date( 2017,  7,  4 ) ) // Independence
  ( boost::gregorian::date( 2017,  9,  4 ) ) // Labor
  ( boost::gregorian::date( 2017, 11, 23 ) ) // Thanksgiving, 13:00 est closing on 25
  ( boost::gregorian::date( 2017, 12, 25 ) ) // Christmas

	
  ( boost::gregorian::date( 2018,  1,  1 ) ) // New Years
  ( boost::gregorian::date( 2018,  1, 15 ) ) // Martin Luther
  ( boost::gregorian::date( 2018,  2, 19 ) ) // Presidents  Day
  ( boost::gregorian::date( 2018,  3, 30 ) ) // Good Friday
  ( boost::gregorian::date( 2018,  5, 28 ) ) // Memorial
  ( boost::gregorian::date( 2018,  7,  4 ) ) // Independence
  ( boost::gregorian::date( 2018,  9,  3 ) ) // Labor
  ( boost::gregorian::date( 2018, 11, 22 ) ) // Thanksgiving, 13:00 est closing on 25
  ( boost::gregorian::date( 2018, 12, 25 ) ) // Christmas
  ;


} // namespace exchange
} // namespace holidays
} // namespace tf
} // namespace ou


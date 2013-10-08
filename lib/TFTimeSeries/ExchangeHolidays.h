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

// Started 2013/10/08

#pragma once

#include <set>

#include "boost/date_time/gregorian/gregorian_types.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace holidays {
namespace exchange {

typedef std::set<boost::gregorian::date> setDates_t;
extern setDates_t setUSDates;

} // namespace exchange
} // namespace holidays
} // namespace tf
} // namespace ou

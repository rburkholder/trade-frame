/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#pragma once

// http://www.theoptionsguide.com/expiration-date.aspx
// http://www.cboe.com/TradTool/strikepricecode.aspx

#include "boost/date_time/gregorian/gregorian.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace options { // options

boost::gregorian::date Next3rdFriday( boost::gregorian::date date );

} // namespace options
} // namespace tf
} // namespace ou


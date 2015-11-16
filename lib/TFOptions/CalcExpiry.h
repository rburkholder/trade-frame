/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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
 
#pragma once

// http://www.theoptionsguide.com/expiration-date.aspx
// http://www.cboe.com/TradTool/strikepricecode.aspx

#include <boost/date_time/gregorian/gregorian.hpp>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options
  
boost::gregorian::date CurrentFrontMonthExpiry( boost::gregorian::date date );
boost::gregorian::date Next3rdFriday( boost::gregorian::date date );
boost::gregorian::date FuturesExpiry( boost::gregorian::date date );
boost::gregorian::date FuturesOptionExpiry( boost::gregorian::date date );

} // namespace option
} // namespace tf
} // namespace ou


/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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

// started 2015/11/15

#pragma once

#include <string>

#include <TFTrading/TradingEnumerations.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
  
// months are 1 .. 12
const std::string BuildOptionName( const std::string& sUnderlying, boost::uint16_t year, boost::uint16_t month, boost::uint16_t day, double strike, ou::tf::OptionSide::enumOptionSide side );
const std::string BuildFuturesName( const std::string& sUnderlying, boost::uint16_t year, boost::uint16_t month );
const std::string BuildFuturesOptionName( const std::string& sUnderlying, boost::uint16_t year, boost::uint16_t month, double strike, ou::tf::OptionSide::enumOptionSide side );
const std::string BuildFuturesOptionName( const std::string& sUnderlying, boost::uint16_t year, boost::uint16_t month, boost::uint16_t day, double strike, ou::tf::OptionSide::enumOptionSide side );

} // namespace iqfeed
} // namespace tf
} // namespace ou

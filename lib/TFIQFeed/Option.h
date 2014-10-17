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

#pragma once

// started 2013/07/13
// Calculate an option symbol, given basic information

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include <TFTrading/TradingEnumerations.h>
#include <TFTrading/Instrument.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

typedef ou::tf::Instrument::pInstrument_t pInstrument_t;

void ComposeOptionName( 
  /* OUT */ std::string& sCall, std::string& sPut, 
  /* IN  */ const std::string& sUnderlying, ou::tf::OptionSide::enumOptionSide option, ptime dtExpiry, double dblStrike );

void SetAlternateName( const pInstrument_t& pInstrument );

} // namespace iqfeed
} // namespace TradeFrame
} // namespace ou


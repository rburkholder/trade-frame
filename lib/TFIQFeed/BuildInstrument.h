/************************************************************************
 * Copyright(c) 2014, One Unified. All rights reserved.                 *
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

// started 2014/10/06
// create an instrument given a ou::tf::iqfeed::MarketSymbol::TableRowDef

#include <TFTrading/Instrument.h>

#include <TFIQFeed/Fundamentals.h>

#include "MarketSymbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

using pInstrument_t = ou::tf::Instrument::pInstrument_t;
using trd_t = ou::tf::iqfeed::MarketSymbol::TableRowDef;
using date_t = boost::gregorian::date;

// day is supplied because IQFeed Market Symbol File doesn't have the day in for futures and futures options
pInstrument_t BuildInstrument( const std::string& sGenericName, const trd_t& trd );
// deprecate (uses assumed defaults)
pInstrument_t BuildInstrument( const std::string& sGenericName, const trd_t& trd, date_t );
// preferred (uses better fundamentals):
pInstrument_t BuildInstrument( const trd_t& trd, const Fundamentals& );
// best (uses fundamentals only):
pInstrument_t BuildInstrument( const Fundamentals& );

} // namespace iqfeed
} // namespace TradeFrame
} // namespace ou

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

#include "MarketSymbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
typedef ou::tf::iqfeed::MarketSymbol::TableRowDef trd_t;

pInstrument_t BuildInstrument( const trd_t& trd );  // equities and futures
pInstrument_t BuildInstrument( const trd_t& trd, pInstrument_t pUnderlying ); // options and futuresoptions

} // namespace iqfeed
} // namespace TradeFrame
} // namespace ou


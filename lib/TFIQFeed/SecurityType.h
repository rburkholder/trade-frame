/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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

/*
 * File:    SecurityType.h
 * Author:  raymond@burkholder.net
 * Project: TFIQFeed
 * Created: Novemeber 2, 2021, 18:14
 */

#ifndef IQFEED_SECURITY_TYPE_H
#define IQFEED_SECURITY_TYPE_H

#include <cstdint>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

enum class ESecurityType: uint32_t {
  Unknown = 0, Bonds, Calc, Equity, FOption, Forex, Forward, Future, ICSpread,
    IEOption, Index, MktRpt, MktStats, Money, Mutual, PrecMtl, Spot, Spread, StratSpread, Swaps, Treasuries,
    _Count
};

} // namespace iqfeed
} // namespace tf
} // namespace ou

#endif
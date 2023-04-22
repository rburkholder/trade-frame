/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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

/*
 * File:    OptionDelegates.hpp
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created: 2023/04/22 14:14:28
 */

#pragma once

#include <OUCommon/FastDelegate.h>

#include <TFTimeSeries/DatedDatum.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

struct Delegates {
  std::string sSymbolName;
  fastdelegate::FastDelegate<void(const ou::tf::Quote&)> fQuote;
  fastdelegate::FastDelegate<void(const ou::tf::Trade&)> fTrade;
  fastdelegate::FastDelegate<void(const ou::tf::Greek&)> fGreek;
};

} // namespace option
} // namespace tf
} // namespace ou
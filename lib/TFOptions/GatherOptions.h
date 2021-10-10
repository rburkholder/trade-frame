/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    GatherOptions.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on October 7, 2021, 21:17
 */

#pragma once

#include <functional>

#include <TFOptions/Option.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

  using pOption_t = Option::pOption_t;
  using fOption_t = std::function<void(pOption_t)>; // incrementally obtain built options
  using fGatherOptions_t = std::function<void(const std::string&, fOption_t&&)>; // request by IQFeed Symbol Name

} // namespace option
} // namespace tf
} // namespace ou

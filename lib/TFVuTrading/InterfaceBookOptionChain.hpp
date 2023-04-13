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
 * File:    InterfaceBookOptionChain.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading
 * Created: 2023/04/12 23:21:16
 */

#pragma once

namespace ou { // One Unified
namespace tf { // TradeFrame

// acquire interface, use interface, dismiss interface

struct InterfaceBookOptionChain {
// Panel Interface
//   add date/strike
//   event: page dismissed (date)
//   event: page selected (date)
// Data Interface
//   update (date, strike, call/put, trade/quote/greek)
//   clear (date,strike)
// Order Interface
//   ComboOrder: fill order legs from leg list

// internal:
//   buy/sell call/put leg into leg list
};

} // namespace tf
} // namespace ou
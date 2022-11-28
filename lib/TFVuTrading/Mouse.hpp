/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    Mouse.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading library
 * Created: 2022/11/27 19:14:13
 */

#pragma once

namespace ou { // One Unified
namespace tf { // TradeFrame

struct Mouse {
  enum class EButton: int {
    Left = -1,
    Middle = 0,
    Right = 1
  };
};

} // namespace tf
} // namespace ou

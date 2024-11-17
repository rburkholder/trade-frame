/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    CubicRegression.hpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: November 15, 2024 22:58:52
 */

#pragma once

#include <array>

namespace Regression {
namespace Cubic {

  static const std::size_t nRows = 10;
  static const std::size_t nCoef =  4;

  using rInput_t = std::array<double,nRows>;
  using rOutput_t = std::array<double,nCoef>;

  void Calc( const rInput_t& X, const rInput_t& Y, rOutput_t& b );
}
}

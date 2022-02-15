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
 * File:    Config.h
 * Author:  raymond@burkholder.net
 * Project: AutoTrade
 * Created: February 14, 2022 10:29
 */

#include <string>

namespace config {

struct Options {

  std::string sSymbol;

  int nPeriodWidth;  // units:  seconds

  // nPeriodWidth * nPeriods => moving average time range
  // common: 10/21/50, or 8/13/21

  // shortest SMA/EMA
  int nMA1Periods;

  // shortest SMA/EMA
  int nMA2Periods;

  // longest SMA/EMA
  int nMA3Periods;
};

bool Load( const std::string& sFileName, Options& );

} // namespace config
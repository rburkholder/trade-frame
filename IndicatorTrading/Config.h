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
 * Project: IndicatorTrading
 * Created: February 8, 2022 00:16
 */

#include <string>

namespace config {

struct Options {

  std::string sSymbol;

  int nPeriodWidth;  // units:  seconds

  // shortest EMA
  int nMA1Periods;

  // shortest EMA
  int nMA2Periods;

  // longest EMA
  int nMA3Periods;

  // Interactive Brokers api instance
  int nIbInstance;

  int nStochastic1Periods;
  int nStochastic2Periods;
  int nStochastic3Periods;

  Options()
  : nPeriodWidth( 10 ), nMA1Periods( 8 ), nMA2Periods( 13 ), nMA3Periods( 21 )
  , nIbInstance( 2 )
  {}
};

bool Load( const std::string& sFileName, Options& );

} // namespace config
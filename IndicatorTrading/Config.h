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

  std::string sSymbol; // this probably will be the general symbol, not a specific instrument

  // Interactive Brokers api instance
  int ib_client_id;

  size_t nThreads; // iqfeed multiple symbols

  size_t nL2Levels;

  int nPeriodWidth;  // units:  seconds

  // shortest to longest
  int nMA1Periods;
  int nMA2Periods;
  int nMA3Periods;

  int nStochastic1Periods;
  int nStochastic2Periods;
  int nStochastic3Periods;

  Options()
  : ib_client_id( 2 ), nThreads( 1 )
  , nPeriodWidth( 10 ), nMA1Periods( 8 ), nMA2Periods( 13 ), nMA3Periods( 21 )
  {}
};

bool Load( const std::string& sFileName, Options& );

} // namespace config
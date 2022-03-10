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
 * Project: rdaf/at
 * Created: March 7, 2022 14:35
 */

#include <string>

namespace config {

struct Options {

  std::string sSymbol;

  int nPeriodWidth;  // units:  seconds

  // nPeriodWidth * nPeriods => moving average time range
  // common: 10/21/50, or 8/13/21

  // shortest EMA
  int nMA1Periods;

  // shortest EMA
  int nMA2Periods;

  // longest EMA
  int nMA3Periods;

  // group directory for simulator - obtain from Hdf5Chart
  std::string sGroupDirectory;

  // force a simulation run
  bool bSimStart;

  // Interactive Brokers api instance
  int nIbInstance;

  Options()
  : nPeriodWidth( 10 ), nMA1Periods( 8 ), nMA2Periods( 13 ), nMA3Periods( 21 )
  , bSimStart( false )
  , nIbInstance( 1 )
  {}
};

bool Load( const std::string& sFileName, Options& );

} // namespace config
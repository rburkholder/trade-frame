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
 * File:    Config.h
 * Author:  raymond@burkholder.net
 * Project: DepthOfMarket
 * Created: October 24, 2021 14:46
 */

 #include <string>

namespace config {

struct Options {

  std::string sSymbolName;

  // Interactive Brokers api instance
  int ib_client_id;

  std::string sDepthType;

  unsigned int nBlockSize; // delta per click

  int nPeriodWidth;  // units:  seconds

  // shortest to longest
  int nMA1Periods;
  int nMA2Periods;
  int nMA3Periods;

  int nStochastic1Periods;
  int nStochastic2Periods;
  int nStochastic3Periods;

  Options(): nBlockSize {} {}

};

bool Load( Options& );

} // namespace config
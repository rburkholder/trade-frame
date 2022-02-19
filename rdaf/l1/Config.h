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
 * Project: rdaf/l1
 * Created: February 6, 2022 14:22
 */

#include <string>

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace config {

struct Options {
  std::string sSymbol;

  int nTimeBins;
  boost::posix_time::ptime dtTimeUpper;
  boost::posix_time::ptime dtTimeLower;

  int nPriceBins;
  double dblPriceUpper;
  double dblPriceLower;

  int nVolumeSideBins;
  double dblVolumeSideUpper;
  double dblVolumeSideLower;

  int nVolumeTotalBins;
  double dblVolumeTotalUpper;
  double dblVolumeTotalLower;
};

bool Load( const std::string& sFileName, Options& );

} // namespace config
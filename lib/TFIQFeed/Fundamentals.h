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

// started: 2021/09/06

#pragma once

#include <string>

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <TFTrading/TradingEnumerations.h>

#include "SecurityType.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

struct Fundamentals {
  std::string sSymbolName;
  std::string sCompanyName;
  std::string sExchangeRoot; // usable for IB?
  std::string sOptionRoots;
  std::string sExchange;
  int nPrecision;
  int nContractSize;
  int nAverageVolume;
  int nOpenInterest; // from Update message, not set currently
  ESecurityType eSecurityType;
  ou::tf::OptionSide::EOptionSide eOptionSide;
  double dblHistoricalVolatility;
  double dblStrikePrice;
  double dblPriceEarnings;
  double dbl52WkHi;
  double dbl52WkLo;
  double dblDividendAmount;
  double dblDividendRate;
  double dblDividendYield;
  double dblTickSize;
  boost::gregorian::date dateExDividend;
  boost::gregorian::date dateExpiration;
  boost::posix_time::time_duration timeSessionOpen; // futures, futures options
  boost::posix_time::time_duration timeSessionClose; // futures, futures options
  Fundamentals()
  : nAverageVolume {}, nPrecision {}, nOpenInterest {},
    eSecurityType( ESecurityType::Unknown ),
    dblHistoricalVolatility {}, dblStrikePrice {}, dblPriceEarnings {},
    dbl52WkHi {}, dbl52WkLo {},
    dblDividendAmount {}, dblDividendRate {}, dblDividendYield {},
    dblTickSize {},
    dateExDividend( boost::posix_time::not_a_date_time ),
    dateExpiration( boost::posix_time::not_a_date_time )
    {}
};

} // namespace iqfeed
} // namespace tf
} // namespace ou

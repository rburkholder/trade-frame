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
 * File:    Config.hpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: March 09, 2024 19:58:27
 */

#pragma once

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace config {

struct Choices {

  std::string m_sSymbolName;
  std::string m_sExchange;

  int m_nIbInstance; // Interactive Brokers api instance

  std::string m_sStartTime;
  boost::posix_time::time_duration m_tdStartTime;

  std::string m_sStopTime;
  boost::posix_time::time_duration m_tdStopTime;

  std::string m_sMaxTradeLifeTime; // minutes
  boost::posix_time::time_duration m_tdMaxTradeLifeTime;

  unsigned int m_nPipProfit;
  unsigned int m_nPipStopLoss;
  unsigned int m_nPipTrailingStop;
  unsigned int m_nLotSize;

  unsigned int m_nBarSeconds;

};

bool Load( const std::string& sFileName, Choices& );

} // namespace config

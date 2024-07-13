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
 * File:    Common.hpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: June 1, 2024 14:06:36
 */

#pragma once

#include <string>
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>

// is base currency the first or second part of a pair, eg, usd.cad
enum class EBase { Unknown, First, Second };

namespace config { // matches Config.hpp

struct Strategy {

  std::string m_sMaxTradeLifeTime; // minutes
  boost::posix_time::time_duration m_tdMaxTradeLifeTime;

  unsigned int m_nPipProfit;
  unsigned int m_nPipStopLoss;
  unsigned int m_nPipTrailingStop;

  unsigned int m_nBarSeconds;

  using vSmootherSeconds_t = std::vector<unsigned int>;
  vSmootherSeconds_t m_vSmootherSeconds;

};

}
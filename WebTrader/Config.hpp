/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * Project: WebTrader
 * Created: 2025/04/05 21:25:03
 */

#pragma once

#include <string>
#include <vector>

namespace config {

struct Choices {

  using vWatchList_t = std::vector<std::string>;
  vWatchList_t m_vWatchList;

  // Interactive Brokers api instance
  int ib_client_id;
  unsigned int ib_client_port;

  // simple auth
  std::string m_sUIUserName;
  std::string m_sUIPassWord;

  using vCandidateFutures_t = std::vector<std::string>;
  vCandidateFutures_t m_vCandidateFutures;
};

bool Load( const std::string& sFileName, Choices& );

} // namespace config

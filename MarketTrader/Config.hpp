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
 * Project: MarketTrader
 * Created: 2024/12/26 22:02:51
 */

#pragma once

#include <string>
#include <cstdint>

namespace config {

struct Telegram {
  std::string sToken;
  uint64_t idChat;
};

struct Values {

  int ib_client_port;
  int ib_client_id;

  std::string sDirConfig;
  std::string sDirLib; // used in lua apps, eg "lib/lua/extract.lua", how to use?
  std::string sDirLog; // unused for now
  std::string sDirScript;

  Telegram telegram;

};

bool Load( const std::string& sFileName, Values& );

} // namespace config
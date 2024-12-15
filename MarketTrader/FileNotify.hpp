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
 * File:    FileNotify.hpp
 * Author:  raymond@burkholder.net
 * Project: TradeFrame/MarketTrader
 * Created: 2024/12/15 12:12:40
 * Based on:Apparition
 */

#pragma once

#include <map>
#include <string>
#include <thread>
#include <functional>
#include <unordered_map>

namespace ou {

class FileNotify {
public:

  enum class EType { unknown_, delete_, create_, modify_, move_from_, move_to_ };
  using fNotify_t = std::function<void( EType, const std::string& )>;

  FileNotify();
  ~FileNotify();

  void AddWatch( const std::string& sPath, fNotify_t&& );
  void DelWatch( const std::string& sPath );

protected:
private:

  int m_fdINotify;

  using mapPathWatch_t = std::unordered_map<std::string, int>;
  mapPathWatch_t m_mapPathWatch;

  using mapWatchNotify_t = std::map<int, fNotify_t>;
  mapWatchNotify_t m_mapWatchNotify;

  bool m_bActive;
  std::thread m_threadINotify;

  void Close();
};

} // namespace ou


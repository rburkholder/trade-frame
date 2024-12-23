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
 * File:    Sol.hpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/12/22 14:16:40
 */

#pragma once

#include <set>
#include <string>
#include <filesystem>
#include <unordered_map>

#include <sol/sol.hpp>

#include "FileNotify.hpp"

class Sol {
public:

  Sol();
  ~Sol();

  void AddPath( const std::string& sPath );
  void DelPath( const std::string& sPath );

protected:
private:

  // set of paths running file notifications
  using setPath_t = std::set<std::string>;
  setPath_t m_setPath;

  // assign a state per script
  // note: may use same script for multiple states if multiple symbols use same script?
  //   key may be object & path, or just object
  //   maybe use CRTP for class specific instance for a series of interfaces
  using mapScript_t = std::unordered_map<std::string, sol::state>;
  mapScript_t m_mapScript;

  ou::FileNotify m_fn;

  void InitialLoad( const std::string& sPath );  // initial directory load
  bool TestExtension( const std::filesystem::path& path );

  mapScript_t::iterator Load( const std::string& sPath );

  void Load( const std::filesystem::path& );
  void Modify( const std::filesystem::path& );
  void Delete( const std::filesystem::path& );

  void Attach( mapScript_t::iterator );
  void Detach( mapScript_t::iterator );

};

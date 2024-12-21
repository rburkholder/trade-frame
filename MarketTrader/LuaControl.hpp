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
 * File:    LuaControl.hpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/12/15 11:58:12
 */

#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>

#include "Lua.hpp"
#include "FileNotify.hpp"

namespace lua {

class Control {
public:

  Control();
  ~Control();

  void AddPath( const std::string& sPath, ou::FileNotify::fNotify_t&& );
  void DelPath( const std::string& sPath );

protected:
private:

  using mapScript_t = std::unordered_map<std::string, Lua>;
  mapScript_t m_mapScript;

  ou::FileNotify m_fn;

  void Load( const std::string& sPath );
  bool TestExtension( const std::filesystem::path& path );
  mapScript_t::iterator  Parse( const std::string& );

  void Load( const std::filesystem::path& );
  void Modify( const std::filesystem::path& );
  void Delete( const std::filesystem::path& );

  void Attach( mapScript_t::iterator );
  void Detach( mapScript_t::iterator );


};

} // namespace lua

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

#include <sol/state.hpp>

#include "FileNotify.hpp"

// TODO: convert to CRTP?
// TODO: need to handle multiple scripts
// TODO: need to handle multiple instances of same script:
//         watch function, one instance per symbol
//         initiated through telegram command to start watch on a symbol
//       load library of scripts, quiesce until invoked by telegram command
//       each script loads telegram with command set
//       telegram mode by symbol?
//       telegram can invoke by iqfeed symbol or generic symbol
//  scripts:
//    watch a symbol, alert on triggers
//    trade on triggers, initiate options on triggers
//    provide symbol list
//    generate chart
//    LuaStateTie renamed as quote/trade watch, can be generically used across scripts

class Sol {
public:

  Sol();
  virtual ~Sol(); // inherited by LuaMarketTie

  void AddPath( const std::string& sPath );
  void DelPath( const std::string& sPath );

protected:

  virtual void Initialize( sol::state& );

private:

  // set of paths running path notifications
  using setPath_t = std::set<std::string>;
  setPath_t m_setPath;

  // assign a state per script
  // note: may use same script for multiple states if multiple symbols use same script?
  //   key may be object & path, or just object
  //   maybe use CRTP for class specific instance for a series of interfaces
  // use a thread tied to asio message passing?
  using mapScript_t = std::unordered_map<std::string, sol::state>;
  mapScript_t m_mapScript;

  ou::FileNotify m_fn;

  void InitialLoad( const std::string& sPath );  // initial directory load
  bool TestExtension( const std::filesystem::path& path );

  mapScript_t::iterator Load( const std::string& sPath ); // sol::state construction

  void Load( const std::filesystem::path& );
  void Modify( const std::filesystem::path& );
  void Delete( const std::filesystem::path& );

  void Attach( mapScript_t::iterator );
  void Detach( mapScript_t::iterator );

};

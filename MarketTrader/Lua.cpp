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
 * File:    Lua.cpp
 * Author:  raymond@burkholder.net
 * Project: TradeFrame/MarketTrader
 * Created: 2024/12/15 18:46:28
 */

#include <cassert>

extern "C" {
#include <luajit-2.1/lua.h>
#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>
}

#include "Lua.hpp"

namespace lua {

Lua::Lua() {

  // http://lua-users.org/wiki/SimpleLuaApiExample

  m_pLua = luaL_newstate();
  assert( m_pLua );

  luaL_openlibs( m_pLua ); /* Load Lua libraries */ // TODO: skip if reduced time/space footprint desirable

}

Lua::~Lua() {
  if ( m_pLua ) {
    lua_close( m_pLua );
    m_pLua = nullptr;
  }
}

} // namespace lua

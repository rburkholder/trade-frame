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
 * File:    Lua.hpp
 * Author:  raymond@burkholder.net
 * Project: TradeFrame/MarketTrader
 * Created: 2024/12/15 18:46:28
 */

#pragma once

class lua_State;

namespace lua {

class Lua {
public:

  Lua();
  Lua( Lua&& rhs ): m_pLua( rhs.m_pLua ) { rhs.m_pLua = nullptr; }
  virtual ~Lua();

  lua_State* operator()() { return m_pLua; } // temporary transitionary

protected:
  lua_State* m_pLua;
private:
};

} // namespace lua

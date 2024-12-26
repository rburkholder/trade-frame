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
 * File:    LuaInterface.cpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/12/25 14:28:13
 */

#include <boost/log/trivial.hpp>

#include "LuaInterface.hpp"

LuaInterface::LuaInterface()
{}

LuaInterface::~LuaInterface() {
  if ( 0 < m_sScriptPath.size() ) {
    m_sol.DelPath( m_sScriptPath );
  }
}

void LuaInterface::SetPath( const std::string& sScriptPath ) {
  m_sol.AddPath( sScriptPath );
}

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

#include "LuaInterface.hpp"

LuaInterface::LuaInterface(  const std::string& sScripts, pProvider_t pExec, pProvider_t pData )
: m_engineInstrument( pExec, pData )
, m_sPathScripts( sScripts )
{
  m_sol.AddPath( m_sPathScripts );
  assert( pExec->Connected() );
  assert( pData->Connected() );
}

LuaInterface::~LuaInterface() {
  m_sol.DelPath( m_sPathScripts );
}
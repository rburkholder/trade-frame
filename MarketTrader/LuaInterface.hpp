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
 * File:    LuaInterface.hpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/12/25 14:28:13
 */

#pragma once

#include "Sol.hpp"

#include "InstrumentEngine.hpp"

class LuaInterface {
public:

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;

  LuaInterface( const std::string& sScripts, pProvider_t pExec, pProvider_t pData );
  ~LuaInterface();

protected:
private:

  const std::string m_sPathScripts;

  ou::tf::engine::Instrument m_engineInstrument;

  Sol m_sol;

};
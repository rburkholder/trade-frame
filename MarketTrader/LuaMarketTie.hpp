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
 * File:    LuaMarketTie.hpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/12/26 11:30:07
 */

#pragma once

#include "Sol.hpp"
#include "InstrumentEngine.hpp"

class LuaMarketTie: public Sol {
public:

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;

  LuaMarketTie( pProvider_t pExec, pProvider_t pData );
  virtual ~LuaMarketTie();

protected:

  virtual void Initialize( sol::state& ) override;

private:

  ou::tf::engine::Instrument m_engineInstrument; // this needs to be factored out to be used by multiple scripts

  void Watch( const std::string& sName );

};
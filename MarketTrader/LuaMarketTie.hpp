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

#include <memory>
#include <string_view>

#include "Sol.hpp"
#include "InstrumentEngine.hpp"
#include "TFTrading/Watch.h"

class LuaStateTie {
public:

  using fHandleTrade_t = std::function<void(double,std::int64_t)>;

  LuaStateTie() = delete;
  LuaStateTie( ou::tf::engine::Instrument&, fHandleTrade_t&& );
  ~LuaStateTie();

  void Watch( const std::string_view& );

protected:
private:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  fHandleTrade_t m_ffHandleTrade;

  ou::tf::engine::Instrument& m_engineInstrument;
  pWatch_t m_pWatch;

  void HandleOnTrade( const ou::tf::Trade& );
};

// ======

class LuaMarketTie: public Sol {
public:

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;

  LuaMarketTie( pProvider_t pExec, pProvider_t pData );
  virtual ~LuaMarketTie();

protected:

  virtual void Initialize( sol::state& ) override;

private:

  ou::tf::engine::Instrument m_engineInstrument; // this needs to be factored out to be used by multiple scripts

};
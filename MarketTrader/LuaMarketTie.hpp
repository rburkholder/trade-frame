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

#include <functional>
#include <string_view>

#include <TFTrading/Watch.h>

#include "Sol.hpp"
#include "InstrumentEngine.hpp"

class LuaStateTie {
public:

  using fHandleTrade_t = std::function<void(double,std::int64_t)>;

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using fConstructedWatch_t = std::function<void( pWatch_t )>;
  using fConstructWatch_t = std::function<void( const std::string_view&, fConstructedWatch_t&& )>;

  LuaStateTie() = delete;
  LuaStateTie( fConstructWatch_t&&, fHandleTrade_t&& );
  ~LuaStateTie();

  void Watch( const std::string_view& );

protected:
private:

  fHandleTrade_t m_ffHandleTrade;
  fConstructWatch_t m_fConstructWatch;

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

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  using mapRequestedInstrument_t = std::unordered_map<std::string,pInstrument_t>; // eg, @ES#
  mapRequestedInstrument_t m_mapRequestedInstrument;

  using mapActualInstrument_t = std::unordered_map<std::string,pInstrument_t>; // eg, specific ES Future
  mapActualInstrument_t m_mapActualInstrument;

  using mapWatch_t = std::unordered_map<uint64_t,pWatch_t>; // ib contract as lookup
  mapWatch_t m_mapWatch;

  ou::tf::engine::Instrument m_engineInstrument; // this needs to be factored out to be used by multiple scripts

};
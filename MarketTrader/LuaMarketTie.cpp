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
 * File:    LuaMarketTie.cpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/12/26 11:30:07
 */

#include <boost/log/trivial.hpp>

#include "LuaMarketTie.hpp"

LuaMarketTie::LuaMarketTie( pProvider_t pExec, pProvider_t pData )
: m_engineInstrument( pExec, pData )
{
  assert( pExec->Connected() );
  assert( pData->Connected() );
}

LuaMarketTie::~LuaMarketTie() {
}

void LuaMarketTie::Watch( const std::string& sName ) { // track which script is sending this, one objec per script?
  m_engineInstrument.Compose(
    "SPY",
    []( ou::tf::Instrument::pInstrument_t p, bool bConstructed ){
      BOOST_LOG_TRIVIAL(trace) << p->GetInstrumentName() << ',' << p->GetContract();
    } );
}

void LuaMarketTie::Initialize( sol::state& sol ) {

}


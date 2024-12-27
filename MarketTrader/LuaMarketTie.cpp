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

LuaStateTie::LuaStateTie( ou::tf::engine::Instrument& engine, fHandleTrade_t&& f )
: m_engineInstrument( engine )
, m_ffHandleTrade( std::move( f ) )
{
  BOOST_LOG_TRIVIAL(trace) << "LuaStateTie::LuaStateTie()";
}

LuaStateTie::~LuaStateTie() {
  BOOST_LOG_TRIVIAL(trace) << "LuaStateTie::~LuaStateTie()";
  if ( m_pWatch ) {
    m_pWatch->StopWatch();
    m_pWatch->OnTrade.Remove( MakeDelegate( this, &LuaStateTie::HandleOnTrade ) );
    m_pWatch.reset();
  }
}

void LuaStateTie::Watch(  const std::string_view& sIQFeedSymbolName ) {
  BOOST_LOG_TRIVIAL(trace) << "LuaStateTie::Watch " << sIQFeedSymbolName;
  m_engineInstrument.Compose(
    std::string( sIQFeedSymbolName ),
    [ this ]( ou::tf::Instrument::pInstrument_t p, bool bConstructed ){
      BOOST_LOG_TRIVIAL(trace)
        << "LuaStateTie::Watch "
        << p->GetInstrumentName() << ','
        << p->GetContract() << ','
        << bConstructed
        ;
      m_pWatch = m_engineInstrument.MakeWatch( p );
      m_pWatch->OnTrade.Add( MakeDelegate( this, &LuaStateTie::HandleOnTrade ) );
      m_pWatch->StartWatch();
    } );
}

void LuaStateTie::HandleOnTrade( const ou::tf::Trade& trade ) {
  //BOOST_LOG_TRIVIAL(trace) << trade.DateTime() << ',' << trade.Volume() << '@' << trade.Price();
  m_ffHandleTrade( trade.Price(), trade.Volume() );
}

// ======

LuaMarketTie::LuaMarketTie( pProvider_t pExec, pProvider_t pData )
: m_engineInstrument( pExec, pData )
{
  assert( pExec->Connected() );
  assert( pData->Connected() );
}

LuaMarketTie::~LuaMarketTie() {
}

void LuaMarketTie::Initialize( sol::state& sol ) {
  auto factory = sol::factories(
    [this,&sol]() {
      using pLuaStateTie_t = std::unique_ptr<LuaStateTie>;
      sol::function handle_trade = sol[ "handle_trade" ];
      LuaStateTie::fHandleTrade_t f = handle_trade;
      pLuaStateTie_t pTie = std::make_unique<LuaStateTie>( m_engineInstrument, std::move( f ) );
      return pTie;
    } );
  sol::usertype<LuaStateTie> lua_state_tie = sol.new_usertype<LuaStateTie>( "tie", factory );
  lua_state_tie[ "watch" ] = &LuaStateTie::Watch;

}


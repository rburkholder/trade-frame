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

LuaStateTie::LuaStateTie( fConstructWatch_t&& fConstructWatch, fHandleTrade_t&& fHandleTrade )
: m_fConstructWatch( std::move( fConstructWatch ) )
, m_ffHandleTrade( std::move( fHandleTrade ) )
{
  BOOST_LOG_TRIVIAL(trace) << "LuaStateTie::LuaStateTie()";
  assert( m_fConstructWatch );
  assert( m_ffHandleTrade );
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
  assert( !m_pWatch );
  m_fConstructWatch(
    std::string( sIQFeedSymbolName ),
    [ this ]( pWatch_t pWatch ){
      BOOST_LOG_TRIVIAL(trace)
        << "LuaStateTie::Watch "
        << pWatch->GetInstrumentName() << ','
        << pWatch->GetInstrument()->GetContract()
        ;
      m_pWatch = std::move( pWatch );
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
      sol::function handle_trade = sol[ "handle_trade" ];
      LuaStateTie::fConstructWatch_t fConstructWatch =
        [this]( const std::string_view& svIQFSymbolName, LuaStateTie::fConstructedWatch_t fConstructedWatch ){
          const std::string sIQFSymbolName( svIQFSymbolName );

          mapRequestedInstrument_t::iterator iterRequest = m_mapRequestedInstrument.find( sIQFSymbolName );
          if ( m_mapRequestedInstrument.end() == iterRequest ) {
            m_engineInstrument.Compose(
              sIQFSymbolName,
              [this,sIQFSymbolName,f_=std::move( fConstructedWatch )]( pInstrument_t pInstrument, bool bConstructed ){
                assert( bConstructed ); // what does this do?
                m_mapRequestedInstrument.emplace( sIQFSymbolName, pInstrument );
                m_mapActualInstrument.emplace( pInstrument->GetInstrumentName(), pInstrument );
                pWatch_t pWatch = m_engineInstrument.MakeWatch( pInstrument );
                m_mapWatch.emplace( pInstrument->GetContract(), pWatch );
                f_( pWatch );
              } );
          }
          else {
            uint64_t contract = iterRequest->second->GetContract();
            mapWatch_t::iterator iterWatch = m_mapWatch.find( contract );
            assert( m_mapWatch.end() != iterWatch );
            fConstructedWatch( iterWatch->second );
          }
        };

      LuaStateTie::fHandleTrade_t fHandleTrade = handle_trade;

      using pLuaStateTie_t = std::unique_ptr<LuaStateTie>;
      pLuaStateTie_t pTie = std::make_unique<LuaStateTie>( std::move( fConstructWatch ), std::move( fHandleTrade ) );
      return pTie;
    } );
  sol::usertype<LuaStateTie> lua_state_tie = sol.new_usertype<LuaStateTie>( "tie", factory );
  lua_state_tie[ "watch" ] = &LuaStateTie::Watch;

}


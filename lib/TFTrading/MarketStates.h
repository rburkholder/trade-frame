/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#pragma once

#include <boost/mpl/list.hpp>

#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/transition.hpp>

#include <TFTimeSeries/DatedDatum.h>

namespace sc = boost::statechart;
namespace mpl = boost::mpl;

namespace rtd { // real time data processing

// Events
struct EvInitialize: sc::event<EvInitialize> {};
struct EvQuote: sc::event<EvQuote> {
  EvQuote( const ou::tf::CQuote& quote ): sc::event<EvQuote>(), m_quote( quote ) {};
private:
  const ou::tf::CQuote& m_quote;
};
struct EvTrade: sc::event<EvTrade> {
  EvTrade( const ou::tf::CTrade& trade ): sc::event<EvTrade>(), m_trade( trade ) {};
private:
  const ou::tf::CTrade& m_trade;
};
struct EvScheduled: sc::event<EvScheduled> {};

// States & Machine

struct StateInitialization;  // initialize for market data, intial state for the machine

struct MachineMarketData: 
  sc::state_machine<MachineMarketData, StateInitialization> 
{
};

struct StateInitialization: sc::simple_state<StateInitialization, MachineMarketData> {
  typedef sc::custom_reaction<EvInitialize> reactions;
  sc::result react( const EvInitialize& event ) { return transit<StatePreMarket>(); };  //return transit<StateMarketOpen();  // 
};

struct StatePreMarket: sc::simple_state<StatePreMarket, MachineMarketData> {
  typedef mpl::list<
    sc::custom_reaction<EvQuote>,
    sc::custom_reaction<EvTrade>
  > reactions;
  sc::result react( const EvQuote& ) { return discard_event(); };
  sc::result react( const EvTrade& ) { return discard_event(); };
};

struct StateMarketOpen: sc::simple_state<StateMarketOpen, MachineMarketData> {
  typedef mpl::list<
    sc::custom_reaction<EvQuote>,
    sc::custom_reaction<EvTrade>
  > reactions;
  sc::result react( const EvQuote& ) { return discard_event(); };
  sc::result react( const EvTrade& ) { return discard_event(); };
};

struct StatePreTrading: sc::simple_state<StatePreTrading, MachineMarketData> { // collect some info before initiating trades
  typedef mpl::list<
    sc::custom_reaction<EvQuote>,
    sc::custom_reaction<EvTrade>
  > reactions;
  sc::result react( const EvQuote& ) { return discard_event(); };
  sc::result react( const EvTrade& ) { return discard_event(); };
};

struct StateTrading: sc::simple_state<StateTrading, MachineMarketData> {
  typedef mpl::list<
    sc::custom_reaction<EvQuote>,
    sc::custom_reaction<EvTrade>
  > reactions;
  sc::result react( const EvQuote& ) { return discard_event(); };
  sc::result react( const EvTrade& ) { return discard_event(); };
};

struct StateCancelOrders: sc::simple_state<StateCancelOrders, MachineMarketData> {
  typedef mpl::list<
    sc::custom_reaction<EvQuote>,
    sc::custom_reaction<EvTrade>
  > reactions;
  sc::result react( const EvQuote& ) { return discard_event(); };
  sc::result react( const EvTrade& ) { return discard_event(); };
};

struct StateCancelOrdersIdle: sc::simple_state<StateCancelOrdersIdle, MachineMarketData> {
  typedef mpl::list<
    sc::custom_reaction<EvQuote>,
    sc::custom_reaction<EvTrade>
  > reactions;
  sc::result react( const EvQuote& ) { return discard_event(); };
  sc::result react( const EvTrade& ) { return discard_event(); };
};

struct StateClosePositions: sc::simple_state<StateClosePositions, MachineMarketData> {
  typedef mpl::list<
    sc::custom_reaction<EvQuote>,
    sc::custom_reaction<EvTrade>
  > reactions;
  sc::result react( const EvQuote& ) { return discard_event(); };
  sc::result react( const EvTrade& ) { return discard_event(); };
};

struct StateClosePositionsIdle: sc::simple_state<StateClosePositionsIdle, MachineMarketData> {
  typedef mpl::list<
    sc::custom_reaction<EvQuote>,
    sc::custom_reaction<EvTrade>
  > reactions;
  sc::result react( const EvQuote& ) { return discard_event(); };
  sc::result react( const EvTrade& ) { return discard_event(); };
};

struct StateAfterMarket: sc::simple_state<StateAfterMarket, MachineMarketData> {
  typedef mpl::list<
    sc::custom_reaction<EvQuote>,
    sc::custom_reaction<EvTrade>
  > reactions;
  sc::result react( const EvQuote& ) { return discard_event(); };
  sc::result react( const EvTrade& ) { return discard_event(); };
};

struct StateMarketClosed: sc::simple_state<StateMarketClosed, MachineMarketData> {
  typedef mpl::list<
    sc::custom_reaction<EvQuote>,
    sc::custom_reaction<EvTrade>
  > reactions;
  sc::result react( const EvQuote& ) { return discard_event(); };
  sc::result react( const EvTrade& ) { return discard_event(); };
};

}; // namespace rtd

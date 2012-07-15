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

namespace ou { // One Unified
namespace tf { // TradeFrame

// Events
struct EvInitialize: sc::event<EvInitialize> {};
struct EvQuote: sc::event<EvQuote> {
  EvQuote( const ou::tf::Quote& quote ): sc::event<EvQuote>(), m_quote( quote ) {};
  const ou::tf::Quote& Quote( void ) const { return m_quote; };
private:
  const ou::tf::Quote& m_quote;
};
struct EvTrade: sc::event<EvTrade> {
  EvTrade( const ou::tf::Trade& trade ): sc::event<EvTrade>(), m_trade( trade ) {};
  const ou::tf::Trade& Trade( void ) const { return m_trade; };
private:
  const ou::tf::Trade& m_trade;
};
struct EvScheduled: sc::event<EvScheduled> {};

// Machine

template<typename T, typename I>  // I = StateInitialization
struct MachineMarketStates: 
  sc::state_machine<MachineMarketStates<T,I>, I> 
{
  T data;
};

// States

template<typename S, typename O, typename P> // S = CRTP State, O = Outer State, P = StatePreMarket
struct StateInitialization: sc::simple_state<StateInitialization<S,O,P>, O> {
  typedef sc::custom_reaction<EvInitialize> reactions;
  sc::result react( const EvInitialize& event ) { return static_cast<S*>( this )->Handle( event ); }; 
protected:
  sc::result Handle( const EvInitialize& event ) { return transit<P>(); }; 
};

template<typename O, typename S, typename InnerInitial=mpl::list<> > // O = Outer State, S = State
struct StateBase: sc::simple_state<StateBase<O,S,InnerInitial>, O, InnerInitial > {

  typedef mpl::list<
    sc::custom_reaction<EvQuote>,
    sc::custom_reaction<EvTrade>,
    sc::custom_reaction<EvScheduled>
  > reactions;


//protected:
  sc::result Handle( const EvQuote& ) { return discard_event(); }
  sc::result Handle( const EvTrade& ) { return discard_event(); };
  sc::result Handle( const EvScheduled& ) { return discard_event(); };

  template<typename E>
  sc::result react( const E& event ) { return static_cast<S*>( this )->Handle( event ); };
  //sc::result react( const E& event ) { return Handle( event ); };

private:
};

} // namespace tf
} // namespace ou


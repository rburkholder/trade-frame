/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    StateMachine_Order.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: November 8, 2025 15:11:44
 */

// based upon https://www.boost.org/doc/libs/1_89_0/libs/msm/doc/HTML/examples/SimpleWithFunctors2.cpp

#include <boost/log/trivial.hpp>

// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
// functors
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
// for And_ operator
#include <boost/msm/front/euml/operator.hpp>
// for func_state and func_state_machine
#include <boost/msm/front/euml/state_grammar.hpp>

//#include "StateMachine_Order.hpp"

namespace msm = boost::msm;
namespace mpl = boost::mpl;
namespace front = msm::front;

namespace {

// events:

struct evtBuild {};  // build order
struct evtSubmit {}; // submit order
struct evtCancel {};  // action to cancel
struct evtExecute {};  // wait for events from exchange
struct evtFill {};  // exchange full or partial fill
struct evtCancelled {}; // exchange cancellation
struct evtDone {}; // reporting / clean up

struct OrderCycle: public msm::front::state_machine_def<OrderCycle> {

  // define states
  struct NoOrder: public msm::front::state<> {};
  struct Build: public msm::front::state<> {};
  struct Submit: public msm::front::state<> {};
  struct Execution: public msm::front::state<> {}; // fallthrough from Submit
  struct Cancel: public msm::front::state<> {};
  struct Fill: public msm::front::state<> {};
  struct Cancelled: public msm::front::state<> {};
  struct Done: public msm::front::state<> {};

  // define initial state
  using initial_state = NoOrder;

  // actions

  struct build_order {
    template <typename EVT, typename FSM, typename SourceState, typename TargetState>
    void operator()( EVT const&, FSM&, SourceState&, TargetState& ) {
    }
  };

  struct submit_order {
    template <typename EVT, typename FSM, typename SourceState, typename TargetState>
    void operator()( EVT const&, FSM&, SourceState&, TargetState& ) {
    }
  };

  struct cancel_order {
    template <typename EVT, typename FSM, typename SourceState, typename TargetState>
    void operator()( EVT const&, FSM&, SourceState&, TargetState& ) {
    }
  };

  struct order_execution {
    template <typename EVT, typename FSM, typename SourceState, typename TargetState>
    void operator()( EVT const&, FSM&, SourceState&, TargetState& ) {
    }
  };

  struct order_fill {
    template <typename EVT, typename FSM, typename SourceState, typename TargetState>
    void operator()( EVT const&, FSM&, SourceState&, TargetState& ) {
    }
  };

  struct order_cancel {
    template <typename EVT, typename FSM, typename SourceState, typename TargetState>
    void operator()( EVT const&, FSM&, SourceState&, TargetState& ) {
    }
  };

  struct order_done {
    template <typename EVT, typename FSM, typename SourceState, typename TargetState>
    void operator()( EVT const&, FSM&, SourceState&, TargetState& ) {
    }
  };

  // guard conditions

  struct fill_status_full {
    template <typename EVT, typename FSM, typename SourceState, typename TargetState>
    bool operator()( EVT const&, FSM&, SourceState&, TargetState& ) {
      const bool filled( true ); // temporary, check for partial fills
      return filled;  //
    }
  };

  struct fill_status_parial {
    template <typename EVT, typename FSM, typename SourceState, typename TargetState>
    bool operator()( EVT const&, FSM&, SourceState&, TargetState& ) {
      const bool partial( false ); // temporary, check for partial fills
      return partial;  //
    }
  };

  using oc = OrderCycle;

  struct transition_table: mpl::vector<
    //            Start       Event         Next       Action           Guard
    //         +-----------+-------------+----------+----------------+--------------+
    front::Row < NoOrder   , evtBuild    , Build    , build_order    , front::none >,
    front::Row < Build     , evtSubmit   , Submit   , submit_order   , front::none >,
    front::Row < Submit    , evtExecute  , Execution, order_execution, front::none >,
    front::Row < Execution , evtFill     , Fill     , order_fill     , front::none >,
    front::Row < Execution , evtCancelled, Cancelled, order_cancel   , front::none >,
    front::Row < Fill      , evtExecute  , Execution, front::none    , front::none >,
    front::Row < Fill      , evtDone     , Done     , order_done     , front::none >,
    front::Row < Cancelled , evtDone     , Done     , order_done     , front::none >
  > {};

  // replaces default no-transition reponse
  template <typename EVT, typename FSM>
  void no_transition( EVT const & e, FSM&, int state ) {
    BOOST_LOG_TRIVIAL(debug)
      << "no transition from state " << state
      << " on event " << typeid(e).name()
      << std::endl;
  }

}; // struct SMOrder

using BackEnd = msm::back::state_machine<OrderCycle>;

// diagnostic
static char const* const state_names[] = { "NoOrder", "Build", "Submit", "Execution", "Cancel", "Fill", "Cancelled", "Done" };
void pstate( BackEnd const& be ) {
  BOOST_LOG_TRIVIAL(debug) << " -> " << state_names[ be.current_state()[0]]  << std::endl;
}

}
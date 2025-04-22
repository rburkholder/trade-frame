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
 * File:    TradeState.cpp
 * Author:  raymond@burkholder.net
 * Project: TFBitsNPieces
 * Created: November 5, 2024 21:37
 */

#include <boost/describe.hpp>

#include <boost/log/trivial.hpp>

#include "TradeState.hpp"

ETradeState::E ETradeState::Set( E state ) {

  const E oldState( m_state );
  m_state = state;

  BOOST_LOG_TRIVIAL(trace)
    << "state "
    << boost::describe::enum_to_string( oldState, "unknown" )
    << " > "
    << boost::describe::enum_to_string( m_state, "unknown" )
    ;

  return oldState;
}

ETradeState::E ETradeState::Set( E state, const char* func, const int line ) {

  const E oldState( m_state );
  m_state = state;

  BOOST_LOG_TRIVIAL(trace)
    << "state "
    << boost::describe::enum_to_string( oldState, "unknown" )
    << " > "
    << boost::describe::enum_to_string( m_state, "unknown" )
    << ", " << func
    << ", " << line
    ;

  return oldState;
}

ETradeState::E ETradeState::Set( E state, const std::string& name, const char* func, const int line ) {

  const E oldState( m_state );
  m_state = state;

  BOOST_LOG_TRIVIAL(trace)
    << name
    << " state "
    << boost::describe::enum_to_string( oldState, "unknown" )
    << " > "
    << boost::describe::enum_to_string( m_state, "unknown" )
    << ", " << func
    << ", " << line
    ;

  return oldState;
}

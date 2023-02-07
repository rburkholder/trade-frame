/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    State.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: February 6, 2023 18:34:54
 */

#include <algorithm>

#include "State.hpp"

State::State(
  const double bid, const double ask
, const double ma0, const double ma1, const double ma2, const double ma3
, const double slope0, const double slope1, const double slope2, const double slope3
)
: m_slope0( slope0 ), m_slope1( slope1 ), m_slope2( slope2 ), m_slope3( slope3 )
{

  rMAOrder_t::iterator iter( m_rMAOrder.begin() );

  Insert( iter, MAOrder( EValue::bid, bid ) );
  Insert( iter, MAOrder( EValue::ask, ask ) );
  Insert( iter, MAOrder( EValue::ma0, ma0 ) );
  Insert( iter, MAOrder( EValue::ma1, ma1 ) );
  Insert( iter, MAOrder( EValue::ma2, ma2 ) );
  Insert( iter, MAOrder( EValue::ma3, ma3 ) );

  std::sort( m_rMAOrder.begin(), m_rMAOrder.end() );

  m_stop = m_rMAOrder.rbegin()->value - m_rMAOrder.begin()->value;

  // TODO:
  //   ratio of mid two related to min/max
  //   slope > 0, slope < 0

}

bool State::operator==( const State& rhs ) {
  bool bResult( true );
  rMAOrder_t::const_iterator iterLhs = m_rMAOrder.begin();
  rMAOrder_t::const_iterator iterRhs = rhs.m_rMAOrder.begin();
  for (
    ;
    iterLhs != m_rMAOrder.end();
    iterLhs++, iterRhs++
  ) {
    bResult &= ( *iterLhs == *iterRhs );
  }
  return bResult;
}

bool State::EnterLong() const {
  //return (
    //   ( EValue::bid == rEValue[ 4 ] )
  //     ( EValue::ma0 == rEValue[ 3 ] )
  //  && ( EValue::ma1 == rEValue[ 2 ] )
  //  && ( EValue::ma2 == rEValue[ 1 ] )
  //  && ( EValue::ma3 == rEValue[ 0 ] )
  //);
  return (
    ( 0.0 < m_slope0 ) && ( 0.0 < m_slope1 ) && ( 0.0 < m_slope2 ) && ( 0.0 < m_slope3 )
  );
}

bool State::EnterShort() const {
  //return (
      //   ( EValue::ask == rEValue[ 1 ] )
  //      ( EValue::ma0 == rEValue[ 0 ] )
  //   && ( EValue::ma1 == rEValue[ 1 ] )
  //   && ( EValue::ma2 == rEValue[ 2 ] )
  //   && ( EValue::ma3 == rEValue[ 3 ] )
  //);
  return (
    ( 0.0 > m_slope0 ) && ( 0.0 > m_slope1 ) && ( 0.0 > m_slope2 ) && ( 0.0 > m_slope3 )
  );
}

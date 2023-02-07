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

#include "State.hpp"

State::State(
  const double bid, const double ask
, const double ma0, const double ma1, const double ma2, const double ma3
, const double slope0, const double slope1, const double slope2, const double slope3
)
: m_slope0( slope0 ), m_slope1( slope1 ), m_slope2( slope2 ), m_slope3( slope3 )
{
  mapRelative_t mapRelative;

  Insert( mapRelative, EValue::bid, bid );
  Insert( mapRelative, EValue::ask, ask );
  Insert( mapRelative, EValue::ma0, ma0 );
  Insert( mapRelative, EValue::ma1, ma1 );
  Insert( mapRelative, EValue::ma2, ma2 );
  Insert( mapRelative, EValue::ma3, ma3 );

  m_stop = mapRelative.rbegin()->first - mapRelative.begin()->first;

  // TODO:
  //   ratio of mid two related to min/max
  //   slope > 0, slope < 0

  //rEValue_t::iterator iterEValue( rEValue.begin() );
  //for ( const mapRelative_t::value_type& vt_map: mapRelative ) {
  //  for ( const vRelative_t::value_type vt_vector: vt_map.second ) {
  //    *iterEValue = vt_vector;
  //    iterEValue++;
  //  }
  //}

}

void State::Insert( mapRelative_t& map, EValue e, const double value ) {
  mapRelative_t::iterator iter = map.find( value );
  if ( map.end() == iter ) {
    auto pair =  map.emplace( mapRelative_t::value_type( value, std::move( vRelative_t() ) ));
    assert( pair.second );
    iter = pair.first;
  }
  iter->second.push_back( e );
}

bool State::operator==( const State& rhs ) {
  bool bResult( true );
  rEValue_t::const_iterator iterLhs = m_rEValue.begin();
  rEValue_t::const_iterator iterRhs = rhs.m_rEValue.begin();
  for (
    ;
    iterLhs != m_rEValue.end();
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

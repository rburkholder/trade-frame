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
 * File:    State.hpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: February 6, 2023 18:34:54
 */

#pragma once

#include <map>
#include <array>
#include <vector>
#include <cassert>

struct State {

  enum class EValue { bid, ask, ma0, ma1, ma2, ma3 };

  using vRelative_t = std::vector<EValue>;
  using mapRelative_t = std::map<double,vRelative_t>;

  using rEValue_t = std::array<EValue,6>;
  rEValue_t rEValue;

  double slope0 {};
  double slope1 {};
  double slope2 {};
  double slope3 {};

  double stop {};

  void Insert( mapRelative_t& map, EValue e, const double value ) {
    mapRelative_t::iterator iter = map.find( value );
    if ( map.end() == iter ) {
      auto pair =  map.emplace( mapRelative_t::value_type( value, std::move( vRelative_t() ) ));
      assert( pair.second );
      iter = pair.first;
    }
    iter->second.push_back( e );
  }

  State(
    const double bid, const double ask
  , const double ma0, const double ma1, const double ma2, const double ma3
  , const double slope0_, const double slope1_, const double slope2_, const double slope3_
  )
  : slope0( slope0_ ), slope1( slope1_ ), slope2( slope2_ ), slope3( slope3_ )
  {
    mapRelative_t mapRelative;

    Insert( mapRelative, EValue::bid, bid );
    Insert( mapRelative, EValue::ask, ask );
    Insert( mapRelative, EValue::ma0, ma0 );
    Insert( mapRelative, EValue::ma1, ma1 );
    Insert( mapRelative, EValue::ma2, ma2 );
    Insert( mapRelative, EValue::ma3, ma3 );

    stop = mapRelative.rbegin()->first - mapRelative.begin()->first;

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

  bool operator==( const State& rhs ) {
    bool bResult( true );
    rEValue_t::const_iterator iterLhs = rEValue.begin();
    rEValue_t::const_iterator iterRhs = rhs.rEValue.begin();
    for (
      ;
      iterLhs != rEValue.end();
      iterLhs++, iterRhs++
    ) {
      bResult &= ( *iterLhs == *iterRhs );
    }
    return bResult;
  }

  double Stop() const { return stop; }

  bool EnterLong() const {
    //return (
      //   ( EValue::bid == rEValue[ 4 ] )
    //     ( EValue::ma0 == rEValue[ 3 ] )
    //  && ( EValue::ma1 == rEValue[ 2 ] )
    //  && ( EValue::ma2 == rEValue[ 1 ] )
    //  && ( EValue::ma3 == rEValue[ 0 ] )
    //);
    return (
      ( 0.0 < slope0 ) && ( 0.0 < slope1 ) && ( 0.0 < slope2 ) && ( 0.0 < slope3 )
    );
  }

  bool EnterShort() const {
    //return (
        //   ( EValue::ask == rEValue[ 1 ] )
    //      ( EValue::ma0 == rEValue[ 0 ] )
    //   && ( EValue::ma1 == rEValue[ 1 ] )
    //   && ( EValue::ma2 == rEValue[ 2 ] )
    //   && ( EValue::ma3 == rEValue[ 3 ] )
    //);
    return (
      ( 0.0 > slope0 ) && ( 0.0 > slope1 ) && ( 0.0 > slope2 ) && ( 0.0 > slope3 )
    );
  }
}; // struct State


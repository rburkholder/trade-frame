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
 * File:    CrossingEvent.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: November 11, 2025 12:48:33
 */

// incorporate hysterisis?

#include <cassert>

#include "CrossingEvent.hpp"

namespace ou {
namespace tf {

CrossingEvent::CrossingEvent( double level_, Direction direction_, fTrigger_t&& f )
: state( touched )
, limit {} // problem if level is 0.0?
, level( level_ ), direction( direction_ ), fTrigger( std::move( f ) )
{
  assert( fTrigger );
}

void CrossingEvent::Test( double value ) {
  switch ( direction ) {
    case dn_up: // crossing upwards from low
      switch ( state ) {
        case above:
          if ( level == value ) {
            state = touched;
            limit = value;
          }
          else {
            if ( level > value ) {
              state = below;
              limit = value;
            }
          }
          break;
        case touched:
          if ( level < value ) {
            fTrigger();
            state = above;
            //limit = 0.0;
          }
          else {
            if ( level > value ) {
              state = below;
              limit = value;
            }
          }
          break;
        case below:
          if ( level < value ) {
            fTrigger();
            state = above;
            //limit = 0.0;
          }
          else {
            if ( limit > value ) limit = value;
          }
          break;
      }
      break;
    case up_dn: // crossing downards from high
      switch ( state ) {
        case above:
          if ( level > value ) {
            fTrigger();
            state = below;
            //limit = 0.0;
          }
          else {
            if ( limit < value ) limit = value;
          }
          break;
        case touched:
          if ( level > value ) {
            fTrigger();
            state = below;
            //limit = 0.0;
          }
          else {
            if ( level < value ) {
              state = above;
              limit = value;
            }
          }
          break;
        case below:
          if ( level == value ) {
            state = touched;
            limit = value;
          }
          else {
            if ( level < value ) {
              state = above;
              limit = value;
            }
          }
          break;
      }
      break;
  }
}

} // namespace tf
} // namespace ou

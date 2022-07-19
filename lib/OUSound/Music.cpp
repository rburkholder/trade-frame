/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    Music.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/OUSound
 * Created: July 18, 2022 13:54:58
 */

#include <cmath>
#include <cassert>

#include "Music.hpp"

namespace ou { // One Unified
namespace music {

// https://en.wikipedia.org/wiki/Musical_note
// A above middle C is A4 = 440 Hz
// typical octave range A0 to C8
// midi notes are 0 ( C-1 ) to 127 ( G9)
// midi: f = 2^((p-69)/12) x 440Hz

float Frequency( int octave, PitchClass pc ) {
  assert( -1 <= octave );
  assert(  8 >= octave );
  static const int base_octave = 4;
  int note = 12 * ( octave - base_octave ) + (int)pc;
  return std::pow( 2.0f, (float)note / 12.0f ) * 440.0f;
}


} // namespace music
} // namespace ou

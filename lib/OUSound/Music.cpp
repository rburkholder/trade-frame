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
#include "PortAudio.hpp"

namespace ou { // One Unified
namespace music {

// https://en.wikipedia.org/wiki/Musical_note
// A above middle C is A4 = 440 Hz
// typical octave range A0 to C8
// midi notes are 0 ( C-1 ) to 127 ( G9)
// midi: f = 2^((p-69)/12) x 440Hz

Note Frequency( int octave, PitchClass pc ) {
  assert( -1 <= octave );
  assert(  8 >= octave );
  static const int base_octave = 4;
  int note = 12 * ( octave - base_octave ) + (int)pc; // relative to A above middle C
  return (Note)std::pow( 2.0f, (float)note / 12.0f ) * 440.0f;
}

Chords::Chords( PortAudio& pa )
: m_pa( pa )
, m_progression { {
    { Frequency( 4, PitchClass::C ), Frequency( 4, PitchClass::E  ), Frequency( 4, PitchClass::G ), 0.0f },
    { Frequency( 4, PitchClass::E ), Frequency( 4, PitchClass::Gs ), Frequency( 5, PitchClass::B ), 0.0f },
    { Frequency( 4, PitchClass::D ), Frequency( 4, PitchClass::Fs ), Frequency( 5, PitchClass::A ), 0.0f },
    { Frequency( 4, PitchClass::F ), Frequency( 4, PitchClass::A  ), Frequency( 5, PitchClass::C ), 0.0f }
  } }
, m_envelope( 0.060f, 0.050f, 0.200f, 0.7f, 0.070f ) // fractional seconds
{
}

Chords::~Chords() {}

void Chords::LoadChord() {
  double rate = m_pa.SampleRate();
  m_vGenerator.clear();
  Chord::const_iterator iterNote = m_iterChord->cbegin();
  for ( Note note: *m_iterChord ) {
    if ( 0.0 != note ) {
      m_vGenerator.emplace_back( Generator( note, rate, m_envelope ) );
    }
  }
  m_iterChord++;
}

void Chords::Play() {

  m_iterChord = m_progression.cbegin();
  LoadChord();

  m_pa.Stream(
    [this](unsigned long count, float* frames) {
      bool bContinue( true );
      if ( m_progression.end() != m_iterChord ) {
        bool bChordDone( true );
        for ( const Generator& gen: m_vGenerator ) {
          bChordDone &= gen.Done();
        }
        if ( bChordDone ) {
          if ( m_progression.end() != m_iterChord ) {
            LoadChord();
          }
          else {
            bContinue = false;
          }
        }
      }
      for ( unsigned long ix = 0; ix < count; ix++ ) {
        float sample {};
        for ( Generator& gen: m_vGenerator ) {
          float result = gen.Sample();
          assert( ( -1.1 <= result ) && ( 1.1 >= result ) );
          sample += result;
        }
        auto size = m_vGenerator.size();
        assert( ( ( size * -1.1 ) <= sample ) && ( ( size * 1.1 ) >= sample ) );
        sample = sample / size;
        assert( ( -1.1 <= sample ) && ( 1.1 >= sample ) );
        *(frames++) = sample;
        *(frames++) = sample;
      }
      return bContinue;
    } );

  m_pa.Start();
}

} // namespace music
} // namespace ou

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

//#include <boost/log/trivial.hpp>

#include "Music.hpp"
#include "PortAudio.hpp"

namespace {
using PC = ou::music::PitchClass;
ou::music::Progression progressionUp = {
{
    { Frequency( 4, PC::C ), Frequency( 4, PC::E  ), Frequency( 4, PC::G ), 0.0f },
    { Frequency( 4, PC::E ), Frequency( 4, PC::Gs ), Frequency( 5, PC::B ), 0.0f },
    { Frequency( 4, PC::D ), Frequency( 4, PC::Fs ), Frequency( 5, PC::A ), 0.0f },
    { Frequency( 4, PC::F ), Frequency( 4, PC::A  ), Frequency( 5, PC::C ), 0.0f }
  }
};

ou::music::Progression progressionDn = {
{
    { Frequency( 4, PC::F ), Frequency( 4, PC::A  ), Frequency( 5, PC::C ), 0.0f },
    { Frequency( 4, PC::D ), Frequency( 4, PC::Fs ), Frequency( 5, PC::A ), 0.0f },
    { Frequency( 4, PC::E ), Frequency( 4, PC::Gs ), Frequency( 5, PC::B ), 0.0f },
    { Frequency( 4, PC::C ), Frequency( 4, PC::E  ), Frequency( 4, PC::G ), 0.0f }
  }
};
} // namespace anonymous

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
, m_envelope( 0.060f, 0.050f, 0.200f, 0.7f, 0.070f ) // fractional seconds
{
}

Chords::~Chords() {}

void Chords::LoadChord() {
  //BOOST_LOG_TRIVIAL(debug) << "Chords::LoadChord";
  double rate = m_pa.SampleRate();
  m_vGenerator.clear();
  for ( Note note: *m_iterChord ) {
    if ( 0.0 != note ) {
      m_vGenerator.emplace_back( Generator( note, rate, m_envelope ) );
    }
  }
  assert( 0 < m_vGenerator.size() );
}

void Chords::Play( EProgression progression ) {

  if ( !m_pa.Stopped() ) m_pa.Stop();

  //m_start = std::chrono::high_resolution_clock::now();

  // TODO: assert progression has > 0 entries
  switch ( progression ) {
    case EProgression::Up:
      m_iterChord = progressionUp.cbegin();
      m_iterChord_End = progressionUp.cend();
      break;
    case EProgression::Down:
      m_iterChord = progressionDn.cbegin();
      m_iterChord_End = progressionDn.cend();
      break;
  }

  LoadChord();

  m_pa.Stream(
    [this](unsigned long count, float* frames) {
      // http://files.portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a8a60fb2a5ec9cbade3f54a9c978e2710
      // The callback must always fill the entire output buffer irrespective of its return value.

      //auto start = std::chrono::high_resolution_clock::now();

      for ( unsigned long ix = 0; ix < count; ix++ ) {
        float sample {};
        float result {};
        for ( Generator& gen: m_vGenerator ) {
          result = gen.Sample();
          assert( ( -1.1 <= result ) && ( 1.1 >= result ) );
          sample += result;
        }
        float size = m_vGenerator.size();
        assert( ( ( size * -1.1 ) <= sample ) && ( ( size * 1.1 ) >= sample ) );
        sample = sample / size;
        assert( ( -1.1 <= sample ) && ( 1.1 >= sample ) );
        *(frames++) = sample;
        *(frames++) = sample;
      }

      bool bContinue( true );
      bool bChordDone( true );

      for ( const Generator& gen: m_vGenerator ) {
        bChordDone &= gen.Done();
      }
      if ( bChordDone ) {
        //assert( m_progression.end() != m_iterChord );

        m_iterChord++;
        if ( m_iterChord_End == m_iterChord ) {
          //BOOST_LOG_TRIVIAL(debug) << "Chords::Play done";
          bContinue = false;
        }
        else {
          LoadChord();
        }
      }

      //auto stop = std::chrono::high_resolution_clock::now();
      //auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      //auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(stop - m_start);

      //m_start = std::chrono::high_resolution_clock::now();

      //BOOST_LOG_TRIVIAL(debug)
      //  << "Chords play data, " << count << ","
      //  << duration1.count() << ","
      //  << duration2.count() << " microseconds";

      return bContinue;
    } );

  m_pa.Start();
}

} // namespace music
} // namespace ou

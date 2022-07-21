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
 * File:    Music.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/OUSound
 * Created: July 18, 2022 13:54:58
 */

#include <array>
#include <vector>

#include "WaveGen.hpp"

namespace ou { // One Unified
class PortAudio;
namespace music {

enum class PitchClass { // https://en.wikipedia.org/wiki/Pitch_class
  C = -9, Cs = -8, Df = -8, D = -7, Ds = -6, Ef = -6, E = -5,
  F = -4, Fs = -3, Gf = -3, G = -2, Gs = -1, Af = -1, A = 0, As = 1, Bf = 1, B = 2
};

using Note = float;
using Chord = std::array<Note,4>;
using Progression = std::array<Chord,4>;

Note Frequency( int octave, PitchClass );

class Chords {
public:

  Chords( PortAudio& );
  ~Chords();

  void Play();

protected:
private:

  const Progression m_progression;
  PortAudio& m_pa;

  ADSR::Envelope_seconds m_envelope; // default envelope

  using vGenerator_t = std::vector<Generator>;
  vGenerator_t m_vGenerator;

  Progression::const_iterator m_iterChord;

  void LoadChord();

};

} // namespace music
} // namespace ou

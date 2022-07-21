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
 * File:    WaveGen.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/OUSound
 * Created: July 16, 2022 22:45:26
 */

#include <cstdint>

namespace ou { // One Unified

// == Sine ==

class Sine {
public:
  Sine() = delete;
  Sine( double frequency, double sampling_rate );
  Sine( const Sine& );
  Sine( Sine&& ) = delete;
  float Sample();
protected:
private:

  float m_b1;

  float m_y0;
  float m_y1;
  float m_y2;

};

// == ADSR ==

class ADSR {
public:

  using samples_t = uint32_t;
  using seconds_t = float;

  struct Envelope_samples {
    samples_t attack;
    samples_t decay;
    samples_t sustain;
    float level;  // 0.0 - 1.0 for now, later, should be in db
    samples_t release;
    Envelope_samples(): attack {}, decay {}, sustain {}, level {}, release {} {}
    Envelope_samples( samples_t attack_, samples_t decay_, samples_t sustain_, float level_, samples_t release_)
    : attack( attack_ ), decay( decay_ ), sustain( sustain_ ), level( level_ ), release( release_ )
    {}
  };

  struct Envelope_seconds {
    seconds_t attack;
    seconds_t decay;
    seconds_t sustain;
    float level;  // 0.0 - 1.0 for now, later, should be in db
    seconds_t release;
    Envelope_seconds(): attack {}, decay {}, sustain {}, level {}, release {} {}
    Envelope_seconds( seconds_t attack_, seconds_t decay_, seconds_t sustain_, float level_, seconds_t release_)
    : attack( attack_ ), decay( decay_ ), sustain( sustain_ ), level( level_ ), release( release_ )
    {}
  };

  ADSR( const Envelope_samples&, Sine& );

  float Sample();
  bool Done() const;
  const Envelope_samples& Envelope() const;

protected:
private:

  enum State { Init, Attack, Decay, Sustain, Release, Zero };

  State m_state;

  Envelope_samples m_envelope;
  Sine& m_sine;

  samples_t m_counter;
  float m_interval;
  float m_multiplier;

  void InitAttack();
  void InitDecay();
  void InitSustain();
  void InitRelease();

};

// == Generator ==

class Generator {
public:
  Generator( double frequency, double sampling_rate, const ADSR::Envelope_seconds& );
  Generator( Generator&& );
  ~Generator();
  float Sample() { return adsr.Sample(); }
  bool Done() const { return adsr.Done(); }
protected:
private:
  Sine sine;
  ADSR adsr;
};

} // namespace ou

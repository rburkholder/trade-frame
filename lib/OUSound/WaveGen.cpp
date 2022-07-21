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
 * File:    WaveGen.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/OUSound
 * Created: July 17, 2022 20:25:27
 */

#include <cmath>
#include <cassert>
#include <stdexcept>

#include "WaveGen.hpp"

// https://www.musicdsp.org/en/latest/Synthesis/9-fast-sine-wave-calculation.html

namespace ou { // One Unified

// == Sine ==

Sine::Sine( double frequency, double sample_rate ) {
  double initial_phase = 0.0; // radians
  double w = frequency * 2.0 * M_PI / sample_rate;
  m_b1 = 2.0 * std::cos( w );
  m_y1 = std::sin( initial_phase - w );
  m_y2 = std::sin( initial_phase - 2.0 * w );
}

Sine::Sine( const Sine& rhs )
: m_b1( rhs.m_b1 ), m_y0( rhs.m_y0 ), m_y1( rhs.m_y1 ), m_y2( rhs.m_y2 )
{}

float Sine::Sample() {

  float result;
  result = m_y0 = m_b1 * m_y1 - m_y2;
  m_y2 = m_y1;
  m_y1 = m_y0;

  assert( ( -1.1 <= result ) && ( 1.1 >= result ) );
  return result;
}

// == ADSR ==

ADSR::ADSR( const Envelope_samples& envelope, Sine& sine )
: m_envelope( envelope ), m_sine( sine )
, m_state( State::Init )
, m_counter {}
, m_interval {}, m_multiplier {}
{
  assert( 0.0 <= envelope.level );
  assert( 1.0 >= envelope.level );
  InitAttack();
}

const ADSR::Envelope_samples& ADSR::Envelope() const {
  return m_envelope;
}

float ADSR::Sample() {
  float result {};
  switch (m_state ) {
    case State::Init:
      assert( false );
      break;
    case State::Attack:
      result = m_sine.Sample() * m_multiplier;
      m_multiplier += m_interval;
      m_counter--;
      if ( 0 == m_counter ) {
        InitDecay();
      }
      break;
    case State::Decay:
      result = m_sine.Sample() * m_multiplier;
      m_multiplier -= m_interval;
      m_counter--;
      if ( 0 == m_counter ) {
        InitSustain();
      }
      break;
    case State::Sustain:
      result = m_sine.Sample() * m_multiplier;
      //m_multiplier += m_interval;
      m_counter--;
      if ( 0 == m_counter ) {
        InitRelease();
      }
      break;
    case State::Release:
      result = m_sine.Sample() * m_multiplier;
      m_multiplier -= m_interval;
      m_counter--;
      if ( 0 == m_counter ) {
        m_state = State::Zero;
      }
      break;
    case State::Zero:
      result = m_sine.Sample() * 0.0; // maintain phase for other ops
      break;
  }
  assert( ( -1.1 <= result ) && ( 1.1 >= result ) );
  return result;
}

bool ADSR::Done() const {
  return ( State::Zero == m_state );
}

void ADSR::InitAttack() {
  if ( 0 < m_envelope.attack ) {
    m_state = State::Attack;
    m_counter = m_envelope.attack;
    m_interval = 1.0f / (float)m_envelope.attack;
    m_multiplier = 0.0;
  }
  else {
    InitDecay();
  }
}

void ADSR::InitDecay() {
  if ( 0 < m_envelope.decay ) {
    m_state = State::Decay;
    m_counter = m_envelope.decay;
    m_interval = ( 1.0f - m_envelope.level ) / (float)m_envelope.decay;
    m_multiplier = 1.0;
  }
  else {
    InitSustain();
  }
}

void ADSR::InitSustain() {
  if ( 0 < m_envelope.sustain ) {
    m_state = State::Sustain;
    m_counter = m_envelope.sustain;
    m_interval = 0.0;
    m_multiplier = m_envelope.level;
  }
  else {
    m_state = State::Zero;
    throw std::runtime_error( "invalid envelope" );
  }
}

void ADSR::InitRelease() {
  if ( 0 < m_envelope.release ) {
    m_state = State::Release;
    m_counter = m_envelope.release;
    m_interval = m_envelope.level / (float)m_envelope.release;
    m_multiplier = m_envelope.level;
  }
  else {
    m_state = State::Zero;
  }
}

// == Generator ==

Generator::Generator( double frequency, double sampling_rate, const ADSR::Envelope_seconds& envelope )
: sine( frequency, sampling_rate )
, adsr(
    ADSR::Envelope_samples(
      (ADSR::samples_t)( sampling_rate * envelope.attack )
    , (ADSR::samples_t)( sampling_rate * envelope.decay )
    , (ADSR::samples_t)( sampling_rate * envelope.sustain )
    ,                                    envelope.level
    , (ADSR::samples_t)( sampling_rate * envelope.release )
  )
  , sine
  )
{}

Generator::Generator( Generator&& rhs )
: sine( rhs.sine )
, adsr( rhs.adsr.Envelope(), sine )
{}

Generator::~Generator() {}

} // namespace ou

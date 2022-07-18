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

#include "WaveGen.hpp"

// https://www.musicdsp.org/en/latest/Synthesis/9-fast-sine-wave-calculation.html

namespace ou { // One Unified

Sine::Sine( double frequency, double sample_rate ) {
  double initial_phase = 0.0;
  double w = frequency * 2.0 * M_PI / sample_rate;
  m_b1 = 2.0 * std::cos( w );
  m_y1 = std::sin( initial_phase - w );
  m_y2 = std::sin( initial_phase - 2.0 * w );
}

float Sine::Sample() {

  float result = m_y0 = m_b1 * m_y1 - m_y2;
  m_y2 = m_y1;
  m_y1 = m_y0;

  return result;
}

} // namespace ou

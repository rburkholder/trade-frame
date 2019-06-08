/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:    PivotCrossing.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on June 8, 2019, 2:09 PM
 */

// TFIndicators/Pivots.h has R3 and S3

#include <cassert>

#include "PivotCrossing.h"

namespace {
using State = PivotCrossing::EState;
using Crossing = PivotCrossing::ECrossing;
const Crossing rCrossing[ (unsigned int)State::_size ][ (unsigned int)State::_size ]
  = { //     to:   unknown           gtR2            R1R2            PVR1            PVS1            S1S2            ltS2
/*from*/ { Crossing::none, Crossing::none, Crossing::none, Crossing::none, Crossing::none, Crossing::none, Crossing::none }, // unknown
         { Crossing::none, Crossing::none, Crossing::R2dn, Crossing::R1dn, Crossing::PVdn, Crossing::S1dn, Crossing::S2dn }, // gtR2
         { Crossing::none, Crossing::R2up, Crossing::none, Crossing::R1dn, Crossing::PVdn, Crossing::S1dn, Crossing::S2dn }, // R1R2
         { Crossing::none, Crossing::R2up, Crossing::R1up, Crossing::none, Crossing::PVdn, Crossing::S1dn, Crossing::S2dn }, // PVR1
         { Crossing::none, Crossing::R2up, Crossing::R1up, Crossing::PVup, Crossing::none, Crossing::S1dn, Crossing::S2dn }, // PVS1
         { Crossing::none, Crossing::R2up, Crossing::R1up, Crossing::PVup, Crossing::S1up, Crossing::none, Crossing::S2dn }, // S1S2
         { Crossing::none, Crossing::R2up, Crossing::R1up, Crossing::PVup, Crossing::S1up, Crossing::S2up, Crossing::none }  // ltS2
    };
} // namespace

PivotCrossing::PivotCrossing()
: m_state( EState::unknown ), m_crossing( ECrossing::none ),
  m_R2 {}, m_R1 {}, m_PV {}, m_S1 {}, m_S2 {}
{}

PivotCrossing::PivotCrossing( double R2, double R1, double PV, double S1, double S2 )
: m_state( EState::unknown ), m_crossing( ECrossing::none ),
  m_R2( R2 ), m_R1( R1 ), m_PV( PV ), m_S1( S1 ), m_S2( S2 )
{
  Init();
}

PivotCrossing::~PivotCrossing( ) { }

void PivotCrossing::Init() {
  assert( 0.0 < m_R2 );
  assert( 0.0 < m_R1 );
  assert( 0.0 < m_PV );
  assert( 0.0 < m_S1 );
  assert( 0.0 < m_S2 );
  assert( 0 == (unsigned int) EState::unknown );
}


void PivotCrossing::Set( double R2, double R1, double PV, double S1, double S2 ) {
  m_R2 = R2;
  m_R1 = R1;
  m_PV = PV;
  m_S1 = S1;
  m_S2 = S2;
  m_state = EState::unknown;
  m_crossing = ECrossing::none;
  Init();
}

PivotCrossing::ECrossing PivotCrossing::Update( const double price ) {

  EState state( EState::unknown );

  if ( price < m_PV ) {
    if ( price < m_S1 ) {
      if ( price < m_S2 ) {
        state = EState::ltS2;
      }
      else {
        state = EState::S1S2;
      }
    }
    else {
      state = EState::PVS1;
    }
  }
  else {
    if ( price > m_R1 ) {
      if ( price > m_R2 ) {
        state = EState::gtR2;
      }
      else {
        state = EState::R1R2;
      }
    }
    else {
      state = EState::PVR1;
    }
  }

  m_crossing = rCrossing[(unsigned int)m_state][(unsigned int)state]; // old state(m_state) -> new state(state)
  m_state = state;
  return m_crossing;
}

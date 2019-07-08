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
 * File:    SpreadValidation.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFTrading
 * Created on May 27, 2019, 8:59 AM
 */

#include "SpreadValidation.h"

namespace ou {
namespace tf {

SpreadValidation::SpreadValidation() {
}

SpreadValidation::SpreadValidation( size_t nLegs ) {
  SetLegCount( nLegs );
}

SpreadValidation::SpreadValidation( const SpreadValidation& rhs )
: m_vSpreadCandidate( rhs.m_vSpreadCandidate )
{
}

SpreadValidation::SpreadValidation( const SpreadValidation&& rhs )
: m_vSpreadCandidate( std::move( rhs.m_vSpreadCandidate ) )
{
}

SpreadValidation::~SpreadValidation() {
  m_vSpreadCandidate.clear();
}

void SpreadValidation::SetLegCount( size_t nLegs ) {
  if ( nLegs != m_vSpreadCandidate.size() ) {
    m_vSpreadCandidate.resize( nLegs );
  }
}

void SpreadValidation::SetWatch( size_t ixLeg, pWatch_t pCall ) {
  m_vSpreadCandidate[ ixLeg ].SetWatch( pCall );
}

SpreadValidation::pWatch_t SpreadValidation::GetWatch( size_t ixLeg ) {
  assert( ixLeg < m_vSpreadCandidate.size() );
  //return boost::dynamic_pointer_cast<ou::tf::option::Option>( m_scCall.GetWatch() );
  return m_vSpreadCandidate[ ixLeg ].GetWatch();
}

bool SpreadValidation::IsActive() const {
  bool bActive( true );
  for ( const SpreadCandidate& candidate: m_vSpreadCandidate ) {
    bActive = bActive && candidate.IsActive();
  }
  return bActive;
}

bool SpreadValidation::Validate( size_t nDuration ) {
  bool bValidation( true );
  for ( SpreadCandidate& candidate: m_vSpreadCandidate ) {
    bValidation &= candidate.ValidateSpread( nDuration );
  }
  return bValidation;
}

void SpreadValidation::ResetOptions() {
  for ( SpreadCandidate& candidate: m_vSpreadCandidate ) {
    candidate.Clear();
  }
}

} // namespace tf
} // namespace ou

/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include "Individual.h"

namespace ou { // One Unified
namespace gp { // genetic programming

unsigned int Individual::m_nIdGenerator( 0 );

Individual::Individual(void)
  : m_bComputed( false ), m_nCount( 1 ), m_id( m_nIdGenerator++ ),
  m_dblRawFitness( 0.0 ), m_dblRelativeFitness( 0.0 ), m_dblAdjustedFitness( 0.0 ), m_dblNormalizedFitness( 0.0 )
{
}

Individual::Individual( const Individual& rhs )
  : m_nCount( rhs.m_nCount + 1 ), m_id( rhs.m_id ), m_bComputed( rhs.m_bComputed ),
  m_dblRawFitness( rhs.m_dblRawFitness ), m_dblRelativeFitness( rhs.m_dblRelativeFitness ), 
  m_dblAdjustedFitness( rhs.m_dblAdjustedFitness ), m_dblNormalizedFitness( rhs.m_dblNormalizedFitness ),
  m_Signals( rhs.m_Signals )
{
  m_ssFormula.str( ( rhs.m_ssFormula.str() ) ); 
}

Individual::~Individual(void) {
}

const Individual& Individual::operator=( const Individual& rhs ) {
  if ( &rhs != this ) {
    m_nCount = rhs.m_nCount + 1;
    m_bComputed = rhs.m_bComputed;
    m_id = rhs.m_id;
    m_dblRawFitness = rhs.m_dblRawFitness;
    m_dblRelativeFitness = rhs.m_dblRelativeFitness;
    m_dblAdjustedFitness = rhs.m_dblAdjustedFitness;
    m_dblNormalizedFitness = rhs.m_dblNormalizedFitness;
    m_ssFormula.str( rhs.m_ssFormula.str() ), 
    m_Signals = rhs.m_Signals;
  }
  return *this;
}

void Individual::TreeToString( std::stringstream& ss ) const {
  ss << "Long=";
  m_Signals.rnLong->TreeToString( ss );
  ss << "\nShort=";
  m_Signals.rnShort->TreeToString( ss );
//  ss << "\n";
}

} // namespace gp
} // namespace ou

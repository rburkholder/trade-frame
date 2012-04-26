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

#include <vector>

#include "Individual.h"

#include "Population.h"

namespace ou { // One Unified
namespace gp { // genetic programming

Population::Population( unsigned int nPopulationSize ) 
  : m_nPopulationSize( nPopulationSize ), m_nMaxGenerations( 25 ), m_nMaxDepthOnCreation( 4 ), m_nMaxDepthOnCrossover( 17 ),
  m_probCrossover( 0.95 ), m_probReproduction( 0.10 ), m_probFunctionPointCrossover( 0.90 ), m_probTerminalPointCrossover( 0.10), 
  m_probMutation( 0.0 ), m_probPermutation( 0.0 ), m_probDecimation( 0.58 ), m_ratioElitism( 0.012 ),
  m_nTournamentSize( 2 ), 
  m_cntAboveAverage( 0 ), m_cntSignals( 4 ) // LongEnter, LongExit, ShortEnter, ShortExit
{
  m_pCurGeneration = new Individual[ nPopulationSize ];
  m_vGenerations.push_back( m_pCurGeneration );
  CreateIndividuals( nPopulationSize, m_pCurGeneration );
}

Population::~Population(void) {
  // the various generations will require careful destruction
}

void Population::CreateIndividuals( unsigned int cntIndividuals, Individual* pGeneration ) {
  // can only populate starting at beginning
  // create individuals with RootNode signals
  // keep rootnodes in a list so they can be populated with random expressions
  std::vector<Individual::Signals_t*> vSignals;
  vSignals.resize( cntIndividuals );
  for ( unsigned int ix = 0; ix < cntIndividuals; ++ix ) {
    vSignals[ix] = &( m_pCurGeneration[ ix ].m_Signals );
  }

  // implement ramped half and half, koza, 1992, page 93
}

} // namespace gp
} // namespace ou

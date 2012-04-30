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
  : m_nPopulationSize( nPopulationSize ), m_nMaxGenerations( 25 ), m_nMaxDepthOnCreation( 5 ), m_nMaxDepthOnCrossover( 17 ),
  m_probCrossover( 0.95 ), m_probReproduction( 0.10 ), m_probFunctionPointCrossover( 0.90 ), m_probTerminalPointCrossover( 0.10), 
  m_probMutation( 0.0 ), m_probPermutation( 0.0 ), m_probDecimation( 0.58 ), m_ratioElitism( 0.012 ),
  m_nTournamentSize( 2 ), 
  m_rng( std::time( 0 ) ),  // possible issue after jan 18, 2038?
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
  typedef std::vector<RootNode*> vSignals_t;
  vSignals_t vSignals;
  unsigned int n = cntIndividuals * Individual::Signals_t::cntSignals;
  vSignals.resize( n );
  unsigned int iy = 0;
  for ( unsigned int ix = 0; ix < cntIndividuals; ++ix ) {
    vSignals[iy++] = &( m_pCurGeneration[ ix ].m_Signals.rnLongEnter );
    vSignals[iy++] = &( m_pCurGeneration[ ix ].m_Signals.rnLongExit );
    vSignals[iy++] = &( m_pCurGeneration[ ix ].m_Signals.rnShortEnter );
    vSignals[iy++] = &( m_pCurGeneration[ ix ].m_Signals.rnShortExit );
  }

  // implement ramped half and half, koza, 1992, page 93
  RootNode* node;
  unsigned int nNodesPerStep = n / ( m_nMaxDepthOnCreation - 1 );
  unsigned int nNodesPerHalf = nNodesPerStep / 2;
  
  for ( unsigned int step1 = 2; step1 <=m_nMaxDepthOnCreation; ++step1 ) {
    for ( unsigned int step2 = 1; step2 <= nNodesPerHalf; ++step2 ) {
      boost::random::uniform_int_distribution<vSignals_t::size_type> dist( 0, vSignals.size() - 1 );  // closed range
      vSignals_t::size_type ix( dist( m_rng ) );
      vSignals_t::const_iterator iter = vSignals.begin() + ix;
      node = *iter;
      vSignals.erase( iter );
    }
  }
}

} // namespace gp
} // namespace ou

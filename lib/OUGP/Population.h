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

#pragma once

#include <vector>
#include <array>

#include <boost/random.hpp>
#include <boost/random/uniform_real_distribution.hpp>

#include "TreeBuilder.h"
#include "Individual.h"

namespace ou { // One Unified
namespace gp { // genetic programming

class Population {
public:
  Population( unsigned int nPopulationSize = 20 );
  ~Population(void);
protected:
private:

  typedef Individual::pRootNode_t pRootNode_t;

  // page 114, Kosa 1992
  const unsigned int m_nPopulationSize; // should be multiple of ( 2 * ( size / maxdepth - 1 ) )
  const double m_dblPopulationSize;
  const unsigned int m_nMaxGenerations;

  const unsigned int m_nMaxDepthOnCreation;
  const unsigned int m_nMaxDepthOnCrossover;

  const double m_probFunctionPointCrossover;
  const double m_probTerminalPointCrossover;

  const double m_probCrossover;
  const double m_probReproduction;

  const double m_probDecimation;
  const double m_ratioElitism;

  const unsigned int m_nTournamentSize;
  const double m_probTournamentSegregation;

  double m_probMutation;
  double m_probPermutation;

  unsigned int m_cntAboveAverage;

  typedef std::vector<Individual> vGeneration_t;
  vGeneration_t* m_pvCurGeneration;
  vGeneration_t* m_pvNxtGeneration;

  typedef std::vector<vGeneration_t*> vGenerations_t;
  vGenerations_t m_vGenerations;

  boost::random::mt19937 m_rng;
  boost::random::uniform_real_distribution<double> m_urd;

  TreeBuilder m_tb;

  void BuildIndividuals( vGeneration_t& vGeneration );
  unsigned int TournamentSelection( unsigned int cntAboveAverage );
  bool IsMatchInGeneration( const Individual&, const vGeneration_t&, vGeneration_t::size_type ixMax );
  bool MakeNewGeneration( bool bCopyValues );
  bool CrossOver( pRootNode_t& rn1, pRootNode_t& rn2 );

};

} // namespace gp
} // namespace ou

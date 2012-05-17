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

//#include <boost/assign/std/vector.hpp>
//using namespace boost::assign;

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/core/reference.hpp>
#include <boost/phoenix/stl/container.hpp>

#include "Individual.h"

#include "Population.h"

namespace ou { // One Unified
namespace gp { // genetic programming

Population::Population( unsigned int nPopulationSize ) 
  : m_nPopulationSize( nPopulationSize ), m_dblPopulationSize( nPopulationSize ),
  m_nMaxGenerations( 25 ), m_nMaxDepthOnCreation( 5 ), m_nMaxDepthOnCrossover( 17 ),
  m_probCrossover( 0.95 ), m_probReproduction( 0.10 ), m_probFunctionPointCrossover( 0.90 ), m_probTerminalPointCrossover( 0.10), 
  m_probMutation( 0.0 ), m_probPermutation( 0.0 ), m_probDecimation( 0.58 ), m_ratioElitism( 0.012 ),
  m_probTournamentSegregation( 0.35 ),
  m_nTournamentSize( 2 ), 
  m_rng( std::time( 0 ) ),  // possible issue after jan 18, 2038?
  m_urd( 0.0, 1.0 ),  // probability in [0.0, 1.0)
  m_cntAboveAverage( 0 )
{
  m_pvCurGeneration = new vGeneration_t;
  m_pvCurGeneration->resize( nPopulationSize );
  BuildIndividuals( *m_pvCurGeneration );
  m_vGenerations.push_back( m_pvCurGeneration );
}

Population::~Population(void) {
  // the various generations will require careful destruction
}

void Population::BuildIndividuals( vGeneration_t& vGeneration ) {

  using boost::phoenix::arg_names::arg1;

  // can only populate starting at beginning
  // create individuals with RootNode signals
  // keep rootnodes in a list so they can be populated with random expressions
  typedef Individual::pRootNode_t pRootNode_t;
  typedef std::vector<pRootNode_t> vSignals_t;
  vSignals_t vSignals;
  unsigned int n( vGeneration.size() * Individual::Signals_t::cntSignals );
  vSignals.resize( n );
  unsigned int iy = 0;
  for ( vSignals_t::size_type ix = 0; ix < vGeneration.size(); ++ix ) {
//    vSignals[iy++] = vGeneration[ ix ].m_Signals.rnLong;
    vGeneration[ ix ].m_Signals.EachSignal( boost::phoenix::ref( vSignals.at(iy++) ) = arg1 );
  }

  // implement ramped half and half, koza, 1992, page 93
  pRootNode_t node;
  unsigned int nNodesPerStep = n / ( m_nMaxDepthOnCreation - 1 );
  unsigned int nNodesPerHalf = nNodesPerStep / 2;

  vSignals_t::const_iterator iter;
  vSignals_t::size_type ix;
  
  for ( unsigned int step1 = 2; step1 <=m_nMaxDepthOnCreation; ++step1 ) {
    for ( unsigned int step2 = 1; step2 <= nNodesPerHalf; ++step2 ) {

      boost::random::uniform_int_distribution<vSignals_t::size_type> dist1( 0, vSignals.size() - 1 );  // closed range
      ix = dist1( m_rng );
      iter = vSignals.begin() + ix;
      node = *iter;
      vSignals.erase( iter );
      m_tb.BuildTree( *node, true, true, step1 );  // random tree

      boost::random::uniform_int_distribution<vSignals_t::size_type> dist2( 0, vSignals.size() - 1 );  // closed range
      ix = dist2( m_rng );
      iter = vSignals.begin() + ix;
      node = *iter;
      vSignals.erase( iter );
      m_tb.BuildTree( *node, false, true, step1 );  // full tree
    }
  }
  // due to rounding above, finish up any unused rootnodes
  while ( 0 != vSignals.size() ) {
    iter = vSignals.begin();  // change this to use end of vector instead
    node = *iter;
    vSignals.erase( iter );
    m_tb.BuildTree( *node, true, true, m_nMaxDepthOnCreation );  // full tree
  }
}

unsigned int Population::TournamentSelection( unsigned int cntAboveAverage ) {
  double prob1; // use above average individuals % of the time
  unsigned int prob2; // used for selecting individual
  unsigned int result = m_nPopulationSize - 1;  // default to worst individual (vector is sorted)
  boost::random::uniform_int_distribution<unsigned int> dist1( 0, cntAboveAverage - 1 );
  boost::random::uniform_int_distribution<unsigned int> dist2( cntAboveAverage, m_nPopulationSize - 1 );
  for ( unsigned int i = m_nTournamentSize; i>=1; --i ) {
    prob1 = m_urd( m_rng );
    if ( m_probTournamentSegregation > prob1 ) {
      prob2 = dist1( m_rng );
    }
    else {
      prob2 = dist2( m_rng );
    }
    result = std::min<unsigned int>( result, prob2 );
  }
  return result;
}

bool Population::IsMatchInGeneration( const Individual& individual, const vGeneration_t& generation, vGeneration_t::size_type ixMax ) {
  bool b = false;
  std::stringstream ssIndividual1;
  individual.TreeToString( ssIndividual1 );
  for ( vGenerations_t::size_type ix = 0; ix <= ixMax; ++ix ) {
    std::stringstream ssIndividual2;
    generation[ ix ].TreeToString( ssIndividual2 );
    if ( ssIndividual1.str() == ssIndividual2.str() ) {
      b = true;
      break;
    }
  }
  return b;
}

bool Population::MakeNewGeneration( bool bCopyValues ) {

  using boost::phoenix::arg_names::arg1;

  typedef Individual::pRootNode_t pRootNode_t;

  bool bMore( false );
  unsigned int cntMaxElites( (unsigned int) std::floor( m_ratioElitism * m_dblPopulationSize + 0.5 ) );

  if ( m_nMaxGenerations > m_vGenerations.size() + 1 ) { // is this a correct validation?

    bMore = true;
    m_pvNxtGeneration = new vGeneration_t;  // set no individuals

//    unsigned int ix( 0 );

    assert( 0 != m_cntAboveAverage );
    double dblPopulationSize( m_nPopulationSize );
    // decimate and repopulate minimum of ( 40% population, count below average )
    unsigned int decimation = std::min<unsigned int>( (unsigned int) std::floor( 0.40 * m_dblPopulationSize + 0.5 ), m_nPopulationSize - m_cntAboveAverage );
    // always introduce a certain minimum set of new individuals
    decimation = std::max<unsigned int>( decimation, (unsigned int) std::floor( 0.04 * m_dblPopulationSize + 0.5 ) );

    m_pvNxtGeneration->resize( decimation );
    BuildIndividuals( *m_pvNxtGeneration );
    m_vGenerations.push_back( m_pvNxtGeneration );

    // elitism
    unsigned int cntAcceptedElites( 0 );
    vGeneration_t::iterator iter( m_pvCurGeneration->begin() );
    if ( ( 0 == m_pvNxtGeneration->size() ) && ( 0 < cntMaxElites ) ) { // bring over at least one elite
      m_pvNxtGeneration->push_back( *iter );
      ++cntAcceptedElites;
    }
    while ( cntAcceptedElites < cntMaxElites ) {
      iter++;
      if ( m_pvCurGeneration->end() == iter ) break;
      if ( !IsMatchInGeneration( *iter, *m_pvNxtGeneration, m_pvNxtGeneration->size() - 1 ) ) {
        m_pvNxtGeneration->push_back( *iter );
        ++cntAcceptedElites;
      }
    }

    // reproduction and crossover
    while ( m_nPopulationSize > m_pvCurGeneration->size() ) {
      double prob1( m_urd( m_rng ) );
      if ( ( m_probReproduction > prob1 ) || ( 1 == ( m_nPopulationSize - m_pvCurGeneration->size() ) ) ) {  // cross over appends two individuals
        // reproduction
        const Individual& test( m_pvCurGeneration->at( TournamentSelection( m_cntAboveAverage ) ) );
        if ( !IsMatchInGeneration( test, *m_pvNxtGeneration, m_pvNxtGeneration->size() - 1 ) ) {
          m_pvNxtGeneration->push_back( test ); 
        }
      }
      else {
        // cross over
        bool bOk( true );

        Individual& i1( m_pvCurGeneration->at( TournamentSelection( m_cntAboveAverage ) ) );
        Individual& i2( m_pvCurGeneration->at( TournamentSelection( m_cntAboveAverage ) ) );

        pRootNode_t rnNewXOver1; // seed and grow crossovers here
        pRootNode_t rnNewXOver2;
        pRootNode_t rnOldXOver1; // obtain seed from here
        pRootNode_t rnOldXOver2;
        pRootNode_t rnNode( new RootNode() );  // holds node

        std::vector<pRootNode_t> vSrcNodes;
        i1.m_Signals.EachSignal( push_back( vSrcNodes, arg1 ) );
        i2.m_Signals.EachSignal( push_back( vSrcNodes, arg1 ) );

        Individual i3;
        Individual i4;

        std::vector<pRootNode_t> vDstNodes;

        i3.m_Signals.EachSignal( push_back( vDstNodes, arg1 ) );
        i4.m_Signals.EachSignal( push_back( vDstNodes, arg1 ) );

        // crossover all but two random pairs
        typedef std::vector<pRootNode_t>::size_type prob2_t;
        prob2_t prob2;
        for ( unsigned int k = Individual::Signals_t::cntSignals - 1; k >= 1; --k ) {
          boost::random::uniform_int_distribution<prob2_t> dist1( 0, vSrcNodes.size() - 1 );  // closed range
          prob2 = dist1( m_rng );
          rnOldXOver1 = vSrcNodes[ prob2 ];
          vSrcNodes.erase( vSrcNodes.begin() + prob2 );

          boost::random::uniform_int_distribution<prob2_t> dist2( 0, vSrcNodes.size() - 1 );  // closed range
          prob2 = dist2( m_rng );
          rnOldXOver2 = vSrcNodes[ prob2 ];
          vSrcNodes.erase( vSrcNodes.begin() + prob2 );
        }

      }
    }

  }

  return bMore;
}

} // namespace gp
} // namespace ou

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
#include <algorithm>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/core/reference.hpp>
#include <boost/phoenix/stl/container.hpp>

#include "Individual.h"

#include "Population.h"

namespace ou { // One Unified
namespace gp { // genetic programming

// assumption:  each population and individual will be sharing the same set of Node types.  Additional non default Node types should only be 
//  registered once

Population::Population( unsigned int nPopulationSize ) 
  : m_nPopulationSize( nPopulationSize ), m_dblPopulationSize( nPopulationSize ),
  m_nMaxGenerations( 25 ), m_nMaxDepthOnCreation( 6 ), m_nMaxDepthOnCrossover( 17 ),
  m_probCrossover( 0.95 ), m_probReproduction( 0.10 ), m_probFunctionPointCrossover( 0.90 ), m_probTerminalPointCrossover( 0.10), 
  m_probMutation( 0.0 ), m_probPermutation( 0.0 ), m_probDecimation( 0.58 ), m_ratioElitism( 0.012 ),
  m_probTournamentSegregation( 0.35 ),
  m_nTournamentSize( 2 ), 
  m_rng( std::time( 0 ) ),  // possible issue after jan 18, 2038?
  m_urd( 0.0, 1.0 ),  // probability in [0.0, 1.0)
  m_cntAboveAverage( 0 )
{
}

Population::~Population(void) {
  for ( vGenerations_t::iterator iter = m_vGenerations.begin(); m_vGenerations.end() != iter; ++iter ) {
    delete *iter;
  }
}

void Population::BuildIndividuals( vGeneration_t& vGeneration ) {

  using boost::phoenix::arg_names::arg1;

  // can only populate starting at beginning
  // create individuals with RootNode signals
  // keep rootnodes in a list so they can be populated with random expressions
  typedef Individual::pRootNode_t pRootNode_t;
  typedef std::vector<pRootNode_t*> vSignals_t;
  vSignals_t vSignals;
  unsigned int n( vGeneration.size() * Individual::Signals_t::cntSignals );
  vSignals.reserve( n );
  for ( vSignals_t::size_type ix = 0; ix < vGeneration.size(); ++ix ) {
    vGeneration[ ix ].m_Signals.EachSignal( boost::phoenix::push_back( boost::phoenix::ref(vSignals), arg1 ) );
  }

  // implement ramped half and half, koza, 1992, page 93
  pRootNode_t* ppRootNode;
  unsigned int nNodesPerStep = n / ( m_nMaxDepthOnCreation - 1 );
  unsigned int nNodesPerHalf = nNodesPerStep / 2;

  vSignals_t::const_iterator iter;
  vSignals_t::size_type ix;
  
  assert( n >= ( ( m_nMaxDepthOnCreation - 1 ) * nNodesPerHalf * 2 ) );
  for ( unsigned int step1 = 2; step1 <= m_nMaxDepthOnCreation; ++step1 ) {
    for ( unsigned int step2 = 1; step2 <= nNodesPerHalf; ++step2 ) {

      boost::random::uniform_int_distribution<vSignals_t::size_type> dist1( 0, vSignals.size() - 1 );  // closed range
      ix = dist1( m_rng );
      iter = vSignals.begin() + ix;
      ppRootNode = *iter;
      vSignals.erase( iter );
      assert( 0 != ppRootNode );
      assert( 0 == *ppRootNode );
      *ppRootNode = new RootNode;
      m_tb.BuildTree( **ppRootNode, true, true, step1 );  // random tree

      boost::random::uniform_int_distribution<vSignals_t::size_type> dist2( 0, vSignals.size() - 1 );  // closed range
      ix = dist2( m_rng );
      iter = vSignals.begin() + ix;
      ppRootNode = *iter;
      vSignals.erase( iter );
      assert( 0 != ppRootNode );
      assert( 0 == *ppRootNode );
      *ppRootNode = new RootNode;
      m_tb.BuildTree( **ppRootNode, false, true, step1 );  // full tree
    }
  }
  // due to rounding above, finish up any unused rootnodes
  while ( 0 != vSignals.size() ) {
    iter = vSignals.begin();  // change this to use end of vector instead
    ppRootNode = *iter;
    vSignals.erase( iter );
    assert( 0 != ppRootNode );
    assert( 0 == *ppRootNode );
    *ppRootNode = new RootNode;
    m_tb.BuildTree( **ppRootNode, true, true, m_nMaxDepthOnCreation );  // full tree
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

  bool bMore( false );
  unsigned int cntMaxElites( (unsigned int) std::floor( m_ratioElitism * m_dblPopulationSize + 0.5 ) );

  if ( 0 == m_vGenerations.size() ) {
    bMore = true;
    m_pvCurGeneration = new vGeneration_t;
    m_pvCurGeneration->resize( m_nPopulationSize );
    BuildIndividuals( *m_pvCurGeneration );
    m_vGenerations.push_back( m_pvCurGeneration );
  }
  else if ( m_nMaxGenerations > m_vGenerations.size() + 1 ) { // is this a correct validation?

    bMore = true;
    m_pvNxtGeneration = new vGeneration_t;  // set no individuals

//    assert( 0 != m_cntAboveAverage );  // put back in sometime

    double dblPopulationSize( m_nPopulationSize );
    // decimate and repopulate minimum of ( 40% population, count below average )
    unsigned int decimation = std::min<unsigned int>( (unsigned int) std::floor( 0.40 * m_dblPopulationSize + 0.5 ), m_nPopulationSize - m_cntAboveAverage );
    // always introduce a certain minimum set of new individuals
    decimation = std::max<unsigned int>( decimation, (unsigned int) std::floor( 0.04 * m_dblPopulationSize + 0.5 ) );

    m_pvNxtGeneration->resize( decimation );
    BuildIndividuals( *m_pvNxtGeneration );  // build the beginning population
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
    while ( m_nPopulationSize > m_pvNxtGeneration->size() ) {
      double prob1( m_urd( m_rng ) );
      if ( ( m_probReproduction > prob1 ) || ( 1 == ( m_nPopulationSize - m_pvNxtGeneration->size() ) ) ) {  // cross over appends two individuals
        // reproduction
        const Individual& test( m_pvCurGeneration->at( TournamentSelection( m_cntAboveAverage ) ) );
        if ( !IsMatchInGeneration( test, *m_pvNxtGeneration, m_pvNxtGeneration->size() - 1 ) ) {
          m_pvNxtGeneration->push_back( test ); 
        }
      }
      else {
        // cross over
        bool bOk( true );

        Individual& indvlSrc1( m_pvCurGeneration->at( TournamentSelection( m_cntAboveAverage ) ) );
        Individual& indvlSrc2( m_pvCurGeneration->at( TournamentSelection( m_cntAboveAverage ) ) );

        pRootNode_t rnNewXOver1; // seed and grow crossovers here
        pRootNode_t rnNewXOver2;
        pRootNode_t rnOldXOver1; // obtain seed from here
        pRootNode_t rnOldXOver2;
        pRootNode_t rnNode( new RootNode() );  // holds node

        std::vector<pRootNode_t*> vSrcNodes;
        indvlSrc1.m_Signals.EachSignal( push_back( boost::phoenix::ref( vSrcNodes ), arg1 ) );
        indvlSrc2.m_Signals.EachSignal( push_back( boost::phoenix::ref( vSrcNodes ), arg1 ) );

        Individual indvlDst1;
        Individual indvlDst2;

        std::vector<pRootNode_t*> vDstNodes;

        indvlDst1.m_Signals.EachSignal( push_back( boost::phoenix::ref( vDstNodes ), arg1 ) );
        indvlDst2.m_Signals.EachSignal( push_back( boost::phoenix::ref( vDstNodes ), arg1 ) );

        // crossover all but last random pair
        typedef std::vector<pRootNode_t>::size_type prob2_t;
        prob2_t prob2;
        for ( unsigned int k = Individual::Signals_t::cntSignals - 1; k >= 1; --k ) {
          boost::random::uniform_int_distribution<prob2_t> dist1( 0, vSrcNodes.size() - 1 );  // closed range
          prob2 = dist1( m_rng );
          rnOldXOver1 = *vSrcNodes[ prob2 ];
          vSrcNodes.erase( vSrcNodes.begin() + prob2 );

          boost::random::uniform_int_distribution<prob2_t> dist2( 0, vSrcNodes.size() - 1 );  // closed range
          prob2 = dist2( m_rng );
          rnOldXOver2 = *vSrcNodes[ prob2 ];
          vSrcNodes.erase( vSrcNodes.begin() + prob2 );

          rnNewXOver1 = dynamic_cast<RootNode*>( rnOldXOver1->Replicate( bCopyValues ) );
          rnNewXOver2 = dynamic_cast<RootNode*>( rnOldXOver2->Replicate( bCopyValues ) );

          // rebuild candidate lists here, if we need them
          rnNewXOver1->PopulateCandidates( &m_rng );
          rnNewXOver2->PopulateCandidates( &m_rng );

          // do the crossover
          CrossOver( rnNewXOver1, rnNewXOver2 );

          *vDstNodes.back() = rnNewXOver1;
          vDstNodes.pop_back();
          *vDstNodes.back() = rnNewXOver2;
          vDstNodes.pop_back();
        }

        // cross over last pair
        rnOldXOver1 = *vSrcNodes.back();
        vSrcNodes.pop_back();
        rnOldXOver2 = *vSrcNodes.back();
        vSrcNodes.pop_back();
        assert( 0 == vSrcNodes.size() );

        rnNewXOver1 = dynamic_cast<RootNode*>( rnOldXOver1->Replicate( bCopyValues ) );
        rnNewXOver2 = dynamic_cast<RootNode*>( rnOldXOver2->Replicate( bCopyValues ) );

        rnNewXOver1->PopulateCandidates( &m_rng );
        rnNewXOver2->PopulateCandidates( &m_rng );

        CrossOver( rnNewXOver1, rnNewXOver2 );

        *vDstNodes.back() = rnNewXOver1;
        vDstNodes.pop_back();
        *vDstNodes.back() = rnNewXOver2;
        vDstNodes.pop_back();

        // add to generation
        m_pvNxtGeneration->push_back( indvlDst1 ); // does this properly copy Signals_t?
        m_pvNxtGeneration->push_back( indvlDst2 ); 
      }
      m_pvCurGeneration = m_pvNxtGeneration;
    }
  }

  return bMore;
}

bool Population::CrossOver( pRootNode_t& rn1, pRootNode_t& rn2 ) {

  bool bSuccessful = true;

  Node* node1( 0 );

  NodeType::E ntCrossOver;

  assert( rn1->HasBooleanCandidates() );
  assert( rn2->HasBooleanCandidates() );

  if ( !rn1->HasDoubleCandidates() || !rn2->HasDoubleCandidates() ) {
    // crossover on boolean candidates, there will always be boolean candidates if one or the other doesn't have double candidates
    node1 = rn1->RandomBooleanCandidate();
    assert( NodeType::Bool == node1->ReturnType() );
    ntCrossOver = NodeType::Bool;
  }
  else {
    // crossover on all candidates
    node1 = rn1->RandomAllCandidate();
    ntCrossOver = node1->ReturnType();
  }

  Node* node2( 0 );
  switch ( ntCrossOver ) {
  case NodeType::Bool:
    node2 = rn2->RandomBooleanCandidate();
    break;
  case NodeType::Double:
    node2 = rn2->RandomDoubleCandidate();
    break;
  }

  Node& parent1( node1->Parent() );
  Node& parent2( node2->Parent() );

  switch ( node1->ParentSide() ) {
  case ParentLink::Left:
    parent1.AddLeft( node2 );
    break;
  case ParentLink::Center:
    parent1.AddCenter( node2 );
    break;
  case ParentLink::Right:
    parent1.AddRight( node2 );
    break;
  }

  switch ( node2->ParentSide() ) {
  case ParentLink::Left:
    parent2.AddLeft( node1 );
    break;
  case ParentLink::Center:
    parent2.AddCenter( node1 );
    break;
  case ParentLink::Right:
    parent2.AddRight( node1 );
    break;
  }

  return bSuccessful;
}

void Population::CalcFitness( void ) {
  double dblMax( 0.0 );
  double dblMin( 0.0 );

  for ( vGeneration_t::iterator iter = (*m_pvCurGeneration).begin(); (*m_pvCurGeneration).end() != iter; ++iter ) {
    if ( (*m_pvCurGeneration).begin() == iter ) {
      dblMin = iter->m_dblRawFitness;
      dblMax = iter->m_dblRawFitness;
    }
    else {
      dblMin = std::min<double>( dblMin, iter->m_dblRawFitness );
      dblMax = std::max<double>( dblMax, iter->m_dblRawFitness );
    }
  }

  double dblSum( 0.0 );
  for ( vGeneration_t::iterator iter = (*m_pvCurGeneration).begin(); (*m_pvCurGeneration).end() != iter; ++iter ) {
    iter->m_dblRelativeFitness = dblMax - iter->m_dblRawFitness;
    dblSum += iter->m_dblAdjustedFitness = 1.0 / ( 1.0 + iter->m_dblRelativeFitness );
  }

  m_cntAboveAverage = 0;
  double dblMean = dblSum / m_pvCurGeneration->size();
  for ( vGeneration_t::iterator iter = (*m_pvCurGeneration).begin(); (*m_pvCurGeneration).end() != iter; ++iter ) {
    iter->m_dblNormalizedFitness = iter->m_dblAdjustedFitness / dblSum;
    m_cntAboveAverage += ( iter->m_dblAdjustedFitness > dblMean ) ? 1 : 0;
  }

  using boost::phoenix::arg_names::arg1;
  using boost::phoenix::arg_names::arg2;

  vGeneration_t& gen( *m_pvCurGeneration );

  std::sort( gen.begin(), gen.end(), arg1 > arg2 );
}

} // namespace gp
} // namespace ou

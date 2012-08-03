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

#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>

#include "Node.h"

namespace ou { // One Unified
namespace gp { // genetic programming

class RootNode: public NodeProxy<RootNode> {
public:

  typedef std::vector<Node*> vpNode_t;

  RootNode(void);
  RootNode( const RootNode& rhs );
  ~RootNode(void);

  const RootNode& operator=( const RootNode& rhs );

  void ToString( std::stringstream& ss ) const { ss << "root="; };
  bool EvaluateBoolean( void );

  bool HasBooleanCandidates( void ) { return ( 0 != m_vBooleanCandidates.size() ); };  // should always be true
  bool HasDoubleCandidates( void ) { return ( 0 != m_vDoubleCandidates.size() ); };

  Node* RandomAllCandidate( void );
  Node* RandomBooleanCandidate( void );
  Node* RandomDoubleCandidate( void );

  void PopulateCandidates( boost::random::mt19937* prng );

protected:
private:

  unsigned int m_nMaxDepth;  // count excludes root level

  boost::random::mt19937* m_prng;

  vpNode_t m_vAllCandidates;  // use this to determine node count
  vpNode_t m_vBooleanCandidates;
  vpNode_t m_vDoubleCandidates;

  void AddCandidateNode( unsigned int nDepth, Node* );

};


} // namespace gp
} // namespace ou

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

#include "RootNode.h"

namespace ou { // One Unified
namespace gp { // genetic programming

// ********* RootNode *********

RootNode::RootNode(void)
  : NodeProxy<RootNode>( NodeType::Bool, NodeType::Bool ), 
    m_nMaxDepth( 0 ), m_prng( 0 )
{
  m_cntNodes = 1;
}

RootNode::~RootNode(void) {
}

bool RootNode::EvaluateBoolean( void ) {
  return ChildCenter().EvaluateBoolean();
}

void RootNode::PopulateCandidates( boost::random::mt19937* prng ) {
  m_prng = prng;
  assert( 0 == m_vAllCandidates.size() );  // if not, then we need a ResetCandidates method?
  AddCandidateNode( 0, this );
}

void RootNode::AddCandidateNode( unsigned int nDepth, Node* pNode ) {
  if ( nDepth > m_nMaxDepth ) m_nMaxDepth = nDepth;
  ++nDepth;
  if ( 0 == dynamic_cast<RootNode*>( pNode ) ) { // assign vectors for all but RootNode type
    m_vAllCandidates.push_back( pNode );
    if ( NodeType::Bool == pNode->ReturnType() ) m_vBooleanCandidates.push_back( pNode );
    if ( NodeType::Double == pNode->ReturnType() ) m_vDoubleCandidates.push_back( pNode );
//    if ( 0 == pNode->IsTerminal() ) m_vTerminalCandidates.push_back( pNode );
    // need to handle function candidates yet
  }
  switch ( pNode->NodeCount() ) {
  case 0:
    // do nothing
    break;
  case 1:
    AddCandidateNode( nDepth, &pNode->ChildCenter() ); 
    break;
  case 2:
    AddCandidateNode( nDepth, &pNode->ChildLeft() );
    AddCandidateNode( nDepth, &pNode->ChildRight() );
    break;
  }
}

Node* RootNode::RandomAllCandidate( void ) {
  boost::random::uniform_int_distribution<vpNode_t::size_type> dist( 0, m_vAllCandidates.size() - 1 );
  return m_vAllCandidates[ dist( *m_prng ) ];
}

Node* RootNode::RandomBooleanCandidate( void ) {
  boost::random::uniform_int_distribution<vpNode_t::size_type> dist( 0, m_vBooleanCandidates.size() - 1 );
  return m_vBooleanCandidates[ dist( *m_prng ) ];
}

Node* RootNode::RandomDoubleCandidate( void ) {
  boost::random::uniform_int_distribution<vpNode_t::size_type> dist( 0, m_vDoubleCandidates.size() - 1 );
  return m_vDoubleCandidates[ dist( *m_prng ) ];
}

} // namespace gp
} // namespace ou

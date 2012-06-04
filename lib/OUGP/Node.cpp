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

#include "Node.h"

namespace ou { // One Unified
namespace gp { // genetic programming

std::stringstream& operator<<( std::stringstream& ss, const Node& node ) {
  node.ToString( ss );
  return ss;
}

Node::Node( NodeType::E ReturnType, NodeType::E ChildType ) 
  : m_cntNodes( 0 ), m_bIsTimeSeries( false ),
    m_pParent( 0 ), m_eParentSide( ParentLink::None ), m_ReturnType( ReturnType ), m_ChildType( ChildType ),
    m_pChildLeft( 0 ), m_pChildCenter( 0 ), m_pChildRight( 0 ),
    m_ixCreateNode( 0 )
{
}

Node::~Node(void) {
#ifdef _DEBUG
  if ( ( 0 == m_pChildLeft ) && ( 0 == m_pChildCenter ) && ( 0 == m_pChildRight ) ) {
  }
  else {
    if ( 0 == m_cntNodes ) {
      assert( 0 == m_pChildLeft );
      assert( 0 == m_pChildCenter );
      assert( 0 == m_pChildRight );
    }
    if ( 1 == m_cntNodes ) {
      assert( 0 == m_pChildLeft );
      assert( 0 != m_pChildCenter );
      assert( 0 == m_pChildRight );
    }
    if ( 2 == m_cntNodes ) {
      assert( 0 != m_pChildLeft );
      assert( 0 == m_pChildCenter );
      assert( 0 != m_pChildRight );
    }
  }
#endif
  if ( 0 != m_pChildLeft ) delete m_pChildLeft;
  if ( 0 != m_pChildCenter ) delete m_pChildCenter;
  if ( 0 != m_pChildRight ) delete m_pChildRight;
}

void Node::AddLeft( Node* node ) {
  m_pChildLeft = node;
  node->m_pParent = this;
  node->m_eParentSide = ParentLink::Left;
}

void Node::AddCenter( Node* node ) {
  m_pChildCenter = node;
  node->m_pParent = this;
  node->m_eParentSide = ParentLink::Center;
}

void Node::AddRight( Node* node ) {
  m_pChildRight = node;
  node->m_pParent = this;
  node->m_eParentSide = ParentLink::Right;
}

void Node::TreeToString( std::stringstream& ss ) const {
  switch ( m_cntNodes ) {
  case 0:
    ToString( ss );
    break;
  case 1:
    ToString( ss );
    m_pChildCenter->TreeToString( ss );
    break;
  case 2:
    ss << '(';
    m_pChildLeft->TreeToString( ss );
    ToString( ss );
    m_pChildRight->TreeToString( ss );
    ss << ')';
    break;
  }
}

Node* Node::Replicate( bool bCopyValues ) {
  Node* node = Clone( bCopyValues );
  if ( 0 != m_pChildLeft ) {
    node->AddLeft( m_pChildLeft->Replicate( bCopyValues ) );
  }
  if ( 0 != m_pChildCenter ) {
    node->AddCenter( m_pChildCenter->Replicate( bCopyValues ) );
  }
  if ( 0 != m_pChildRight ) {
    node->AddRight( m_pChildRight->Replicate( bCopyValues ) );
  }
  return node;
}

} // namespace gp
} // namespace ou

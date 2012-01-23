/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

Node::Node(void) 
  : m_bTerminal( false ), m_cntNodes( 2 ), m_eParentSide( None ),
  m_pParent( 0 ), m_pChildLeft( 0 ), m_pChildCenter( 0 ), m_pChildRight( 0 )
{
}

Node::~Node(void) {
}

void Node::TreeToString( std::stringstream& ss ) const {
  if ( m_bTerminal ) {
    ToString( ss );
  }
  else {
    switch ( m_cntNodes ) {
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
}

std::stringstream& operator<<( std::stringstream& ss, const Node& node ) {
  node.ToString( ss );
  return ss;
}

void Node::AddLeft( Node* node ) {
  m_pChildLeft = node;
  node->m_pParent = this;
  node->m_eParentSide = Left;
}

void Node::AddCenter( Node* node ) {
  m_pChildCenter = node;
  node->m_pParent = this;
  node->m_eParentSide = Center;
}

void Node::AddRight( Node* node ) {
  m_pChildRight = node;
  node->m_pParent = this;
  node->m_eParentSide = Right;
}

} // namespace gp
} // namespace ou

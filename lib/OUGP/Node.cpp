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
    m_eParentSide( ParentLink::None ), m_ReturnType( ReturnType ), m_ChildType( ChildType ),
    m_pParent( 0 ), m_pChildLeft( 0 ), m_pChildCenter( 0 ), m_pChildRight( 0 )
{
}

Node::Node( const Node& rhs ) 
  : 
  m_ReturnType( rhs.m_ReturnType ), m_ChildType( rhs.m_ChildType ),
  m_cntNodes( rhs.m_cntNodes ), m_bIsTimeSeries( rhs.m_bIsTimeSeries ),
  m_eParentSide( rhs.m_eParentSide ), 
  m_pParent( 0 ), // to be filled with replicate
  m_pChildLeft( 0 ), // to be filled with replicate
  m_pChildCenter( 0 ), // to be filled with replicate
  m_pChildRight( 0 ) // to be filled with replicate
{
  if ( 0 != rhs.m_pChildLeft ) {
    AddLeft( rhs.m_pChildLeft->Replicate() );
  }
  if ( 0 != rhs.m_pChildCenter ) {
    AddCenter( rhs.m_pChildCenter->Replicate() );
  }
  if ( 0 != rhs.m_pChildRight ) {
    AddRight( rhs.m_pChildRight->Replicate() );
  }
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
  if ( 0 != m_pChildLeft ) { 
    delete m_pChildLeft; 
    m_pChildLeft = 0; 
  }
  if ( 0 != m_pChildCenter ) { 
    delete m_pChildCenter; 
    m_pChildCenter = 0; 
  }
  if ( 0 != m_pChildRight ) {
    delete m_pChildRight; 
    m_pChildRight = 0; 
  }
}

const Node& Node::operator=( const Node& rhs ) {
  if ( &rhs != this ) {
    assert( false ); // not done yet
  }
  return *this;
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
//    ss << '(';
    m_pChildCenter->TreeToString( ss );
//    ss << ')';
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

Node* Node::Replicate( void ) {
  Node* node = CloneBasics();
  if ( 0 != m_pChildLeft ) {
    node->AddLeft( m_pChildLeft->Replicate() );
  }
  if ( 0 != m_pChildCenter ) {
    node->AddCenter( m_pChildCenter->Replicate() );
  }
  if ( 0 != m_pChildRight ) {
    node->AddRight( m_pChildRight->Replicate() );
  }
  return node;
}

} // namespace gp
} // namespace ou

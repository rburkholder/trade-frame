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

//#include <string>
#include <cassert>
#include <sstream>
#include <vector>
#include <stdexcept>

//#include <boost/fusion/sequence.hpp>
//#include <boost/fusion/include/sequence.hpp>

//#include <boost/fusion/container/vector.hpp>

namespace ou { // One Unified
namespace gp { // genetic programming

template<typename T> // used for CRTP 
class Node {
public:

  enum EParentLink {
    None = 0, Left, Center, Right
  };

  Node(void);
  virtual ~Node(void);

  bool IsTerminal( void ) const { return m_bTerminal; };

  virtual void TreeToString( std::stringstream& ) const;
  virtual void ToString( std::stringstream& ) const {};

  // maybe use union here or change names to suit
  void AddLeft( Node* node );  // used with two node
  void AddCenter( Node* node );  // used with terminal node
  void AddRight( Node* node );  // used with two node

  // these two need to be fast as they are evaluated for the whole time series
  bool EvaluateBoolean( void ) { return static_cast<T*>( this)->EvaluateBooleanImpl(); };
  double EvaluateDouble( void ) { return static_cast<T*>( this)->EvaluateDoubleImpl(); };

protected:

  EParentLink m_eParentSide;

  Node* m_pParent;

  Node* m_pChildLeft;
  Node* m_pChildCenter;
  Node* m_pChildRight;

  unsigned int m_cntNodes; // how many child nodes by default (0 for terminal nodes, 1 for single, 2 for two nodes)

private:
  //bool EvaluateBooleanImpl( void ) const { throw std::logic_error( "Node::EvaluateBoolean" ); return false; };
  //double EvaluateDoubleImpl( void ) const { throw std::logic_error( "Node::EvaluateDouble" ); return 0.0; };
};

template<typename T>
std::stringstream& operator<<( std::stringstream& ss, const Node<T>& node ) {
  node.ToString( ss );
  return ss;
}

template<typename T>
Node<T>::Node(void) 
  : m_cntNodes( 0 ), m_eParentSide( None ),
    m_pParent( 0 ), m_pChildLeft( 0 ), m_pChildCenter( 0 ), m_pChildRight( 0 )
{
  m_pParent = 0;
}

template<typename T>
Node<T>::~Node(void) {
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
  if ( 0 != m_pChildLeft ) delete m_pChildLeft;
  if ( 0 != m_pChildCenter ) delete m_pChildCenter;
  if ( 0 != m_pChildRight ) delete m_pChildRight;
}

template<typename T>
void Node<T>::TreeToString( std::stringstream& ss ) const {
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

template<typename T>
void Node<T>::AddLeft( Node* node ) {
  m_pChildLeft = node;
  node->m_pParent = this;
  node->m_eParentSide = Left;
}

template<typename T>
void Node<T>::AddCenter( Node* node ) {
  m_pChildCenter = node;
  node->m_pParent = this;
  node->m_eParentSide = Center;
}

template<typename T>
void Node<T>::AddRight( Node* node ) {
  m_pChildRight = node;
  node->m_pParent = this;
  node->m_eParentSide = Right;
}

} // namespace gp
} // namespace ou

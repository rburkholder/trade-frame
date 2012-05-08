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

#include <cassert>
#include <sstream>
#include <vector>
#include <stdexcept>

namespace ou { // One Unified
namespace gp { // genetic programming

namespace NodeType {
  enum E { Bool = 0, Double = 1, Count = 2 };  // used for indexing to correct lookup vector
}

class Node {
public:

  enum EParentLink {
    None = 0, Left, Center, Right
  };

  Node( NodeType::E ReturnType, NodeType::E ChildType );
  virtual ~Node(void);

  bool IsTerminal( void ) const { return 0 == m_cntNodes; };
  unsigned int NodeCount( void ) const { return m_cntNodes; };
  NodeType::E ReturnType( void ) const { return m_ReturnType; };
  NodeType::E ChildType( void ) const  { return m_ChildType; };

  virtual void TreeToString( std::stringstream& ) const;
  virtual void ToString( std::stringstream& ) const {};

  virtual bool EvaluateBoolean( void ) const { throw std::logic_error( "EvaluateBoolean no override" ); };
  virtual double EvaluateDouble( void ) const { throw std::logic_error( "EvaluateDouble no override" ); };

  // maybe use union here or change names to suit
  void AddLeft( Node* node );  // used with two node
  void AddCenter( Node* node );  // used with terminal node
  void AddRight( Node* node );  // used with two node

  Node& ChildLeft( void ) { assert( 0 != m_pChildLeft ); return *m_pChildLeft; };
  Node& ChildCenter( void ) { assert( 0 != m_pChildCenter ); return *m_pChildCenter; };
  Node& ChildRight( void ) { assert( 0 != m_pChildRight ); return *m_pChildRight; };

protected:

  NodeType::E m_ReturnType;
  NodeType::E m_ChildType;

  Node* m_pParent;

  Node* m_pChildLeft;
  Node* m_pChildCenter;
  Node* m_pChildRight;

  unsigned int m_cntNodes; // how many child nodes permitted by default (0 for terminal nodes, 1 for single, 2 for two nodes)

  EParentLink m_eParentSide;

private:
};

std::stringstream& operator<<( std::stringstream& ss, const Node& node );

} // namespace gp
} // namespace ou

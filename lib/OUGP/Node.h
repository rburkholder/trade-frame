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

class Node {
public:

  enum EParentLink {
    None = 0, Left, Center, Right
  };

  Node(void);
  virtual ~Node(void);

  bool IsTerminal( void ) const { return 0 == m_cntNodes; };
  unsigned int NodeCount( void ) const { return m_cntNodes; };

  virtual void TreeToString( std::stringstream& ) const;
  virtual void ToString( std::stringstream& ) const {};

  virtual bool EvaluateBoolean( void ) const { throw std::logic_error( "EvaluateBoolean no override" ); };
  virtual double EvaluateDouble( void ) const { throw std::logic_error( "EvaluateDouble no override" ); };

  // maybe use union here or change names to suit
  void AddLeft( Node* node );  // used with two node
  void AddCenter( Node* node );  // used with terminal node
  void AddRight( Node* node );  // used with two node

  const Node& ChildLeft( void ) const { assert( 0 != m_pChildLeft ); return *m_pChildLeft; };
  const Node& ChildCenter( void ) const{ assert( 0 != m_pChildCenter ); return *m_pChildCenter; };
  const Node& ChildRight( void ) const { assert( 0 != m_pChildRight ); return *m_pChildRight; };

protected:

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

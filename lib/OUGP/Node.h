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

#pragma once

#include <string>
#include <sstream>
#include <vector>

namespace ou { // One Unified
namespace gp { // genetic programming

class Node {
public:

  enum EParentLink {
    None = 0, Left, Center, Right
  };

  Node(void);
  ~Node(void);

  bool IsTerminal( void ) const { return m_bTerminal; };

  virtual void TreeToString( std::stringstream& ) const;
  virtual void ToString( std::stringstream& ) const = 0;

  void AddLeft( Node* node );
  void AddCenter( Node* node );
  void AddRight( Node* node );

  virtual void AddRandomChildren( bool bUseTerminal, bool bUseNode, unsigned int nDepth, unsigned int nMaxDepth ) = 0;
  void AddRandomChildren( bool bUseTerminal, bool bUseNode, unsigned int nDepth, unsigned int nMaxDepth, std::vector<int>& ); // fix vector

protected:

  EParentLink m_eParentSide;

  bool m_bTerminal;

  Node* m_pParent;

  Node* m_pChildLeft;
  Node* m_pChildCenter;
  Node* m_pChildRight;

  unsigned int  m_cntNodes; // how many child nodes by default (0 for terminal nodes)

private:
};

std::stringstream& operator<<( std::stringstream& ss, const Node& );

} // namespace gp
} // namespace ou

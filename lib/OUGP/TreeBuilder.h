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

#include <boost/random.hpp>

#include "Node.h"
#include "NodeBoolean.h"
#include "NodeCompare.h"
#include "NodeDouble.h"

namespace ou { // One Unified
namespace gp { // genetic programming

class TreeBuilder {
public:

  TreeBuilder(void);
  ~TreeBuilder(void);

  void BuildTree( Node& node, bool bUseTerminal, bool bUseNode, unsigned int nMaxDepth ) {
    AddRandomChildren( node, bUseTerminal, bUseNode, 1, nMaxDepth );
  }

protected:
private:

  typedef std::vector<Node* (*)()> vNodeFactory_t; 

  boost::random::mt19937 m_rng;

  vNodeFactory_t m_vNodeFactoryBooleanAll; 
  vNodeFactory_t m_vNodeFactoryBooleanTerminals;
  vNodeFactory_t m_vNodeFactoryBooleanNodes; // non terminal 

  vNodeFactory_t m_vNodeFactoryDoubleAll;
  vNodeFactory_t m_vNodeFactoryDoubleTerminals;
  vNodeFactory_t m_vNodeFactoryDoubleNodes;  // non terminal

  vNodeFactory_t* m_rNodeFactories[ NodeType::Count ][ FactoryType::Count ];

  Node* CreateChild( NodeType::E nt, bool bUseTerminal, bool bUseNode, unsigned int nDepth, unsigned int nMaxDepth );
  void AddRandomChildren( Node& node, bool bUseTerminal, bool bUseNode, unsigned int nDepth, unsigned int nMaxDepth );
};

} // namespace gp
} // namespace ou

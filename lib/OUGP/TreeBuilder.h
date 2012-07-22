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

#include <boost/fusion/include/for_each.hpp>

#include "Node.h"
#include "NodeBoolean.h"
#include "NodeCompare.h"
#include "NodeDouble.h"

namespace ou { // One Unified
namespace gp { // genetic programming

// CreateNode from http://stackoverflow.com/questions/363453/looking-for-a-better-c-class-factory

template<class T>
Node* CreateNode( void ) {
  // check whether we create or clone.  If we clone, from where do we obtain it?
  return new T();
}

struct NodeFactoryInit {  
  // todo:  if IsTimeSeries, have array available to hold caller assigned NodeTSxxxx nodes for cloning
  //  need to change CreateNode?
  template<typename T>
  void operator()( T& t ) const {
    vAll.push_back( &CreateNode<T> );
    if ( t.IsTerminal() ) 
      vTerm.push_back( &CreateNode<T> );
    else 
      vNode.push_back( &CreateNode<T> );
  }
  typedef std::vector<Node* (*)()> vCreateNode_t;
  NodeFactoryInit( vCreateNode_t& vAll_, vCreateNode_t& vTerm_, vCreateNode_t& vNode_ )
    : vAll( vAll_ ), vTerm( vTerm_ ), vNode( vNode_ ) {}
  vCreateNode_t& vAll;
  vCreateNode_t& vTerm;
  vCreateNode_t& vNode;
};

class TreeBuilder {
public:

  TreeBuilder(void);
  ~TreeBuilder(void);

  void BuildTree( Node& node, bool bUseTerminal, bool bUseNode, unsigned int nMaxDepth ) {
    AddRandomChildren( node, bUseTerminal, bUseNode, 1, nMaxDepth );
  }

  template<typename L> // list of node types
  void RegisterBoolean( void ) {
    L list;
    boost::fusion::for_each( list, NodeFactoryInit( m_vNodeFactoryBooleanAll, m_vNodeFactoryBooleanTerminals, m_vNodeFactoryBooleanNodes ) );
  }

  template<typename L> // list of node types
  void RegisterDouble( void ) {
    L list;
    boost::fusion::for_each( list, NodeFactoryInit( m_vNodeFactoryDoubleAll, m_vNodeFactoryDoubleTerminals, m_vNodeFactoryDoubleNodes ) );
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

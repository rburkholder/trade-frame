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

#include <boost/fusion/include/for_each.hpp>

#include "TreeBuilder.h"

namespace ou { // One Unified
namespace gp { // genetic programming

// CreateNode from http://stackoverflow.com/questions/363453/looking-for-a-better-c-class-factory

template<class T>
Node* CreateNode( void ) {
  return new T();
}

struct NodeFactoryInit {
  template<typename T>
  void operator()( T& t ) const {
    v.push_back( &CreateNode<T> );
  }
  typedef std::vector<Node* (*)()> vCreateNode_t;
  NodeFactoryInit( vCreateNode_t& v_ ): v( v_ ) {}
  vCreateNode_t& v;
};

TreeBuilder::TreeBuilder(void) 
  : m_rng( std::time( 0 ) )  // possible issue after jan 18, 2038?
{
  m_vNodeFactories[ NodeType::Bool ] = &m_vNodeFactoryBoolean;
  m_vNodeFactories[ NodeType::Double ] = &m_vNodeFactoryDouble;

  NodeBoolean_t b1;
  boost::fusion::for_each( b1, NodeFactoryInit(m_vNodeFactoryBoolean) );
  NodeCompare_t b2;
  boost::fusion::for_each( b2, NodeFactoryInit(m_vNodeFactoryBoolean) );
  NodeDouble_t d1;
  boost::fusion::for_each( d1, NodeFactoryInit(m_vNodeFactoryDouble) );

}

TreeBuilder::~TreeBuilder(void) {
}

void TreeBuilder::AddRandomChildren( 
  Node& node, bool bUseTerminal, bool bUseNode, unsigned int nDepth, unsigned int nMaxDepth ) {
  assert( !bUseTerminal && !bUseNode ); // use one or both
  assert( nDepth <= nMaxDepth ); 
  vNodeFactory_t& vFactory( *m_vNodeFactories[ node.ChildType() ] );
  switch ( node.NodeCount() ) {
  case 0: // nothing, this is terminal node
    assert( node.IsTerminal() );
    break;
  case 1:
    node.AddCenter( CreateChild( bUseTerminal, bUseNode, nDepth, nMaxDepth, vFactory ) );
    if ( !node.ChildCenter().IsTerminal() ) {
      AddRandomChildren( node.ChildCenter(), bUseTerminal, bUseNode, nDepth + 1, nMaxDepth );
    }
    break;
  case 2: 
    node.AddLeft( CreateChild( bUseTerminal, bUseNode, nDepth, nMaxDepth, vFactory ) );
    if ( !node.ChildLeft().IsTerminal() ) {
      AddRandomChildren( node.ChildLeft(), bUseTerminal, bUseNode, nDepth + 1, nMaxDepth );
    }
    node.AddRight( CreateChild( bUseTerminal, bUseNode, nDepth, nMaxDepth, vFactory ) );
    if ( !node.ChildRight().IsTerminal() ) {
      AddRandomChildren( node.ChildRight(), bUseTerminal, bUseNode, nDepth + 1, nMaxDepth );
    }
    break;
  }
}

} // namespace gp
} // namespace ou

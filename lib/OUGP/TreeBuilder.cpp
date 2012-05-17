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

  m_rNodeFactories[ NodeType::Bool ] = &m_vNodeFactoryBoolean;
  m_rNodeFactories[ NodeType::Double ] = &m_vNodeFactoryDouble;

  NodeBoolean_t b1;
  boost::fusion::for_each( b1, NodeFactoryInit(m_vNodeFactoryBoolean) );
  boost::fusion::for_each( b1, NodeFactoryInit(m_vNodeFactoryAllNodes) );

  NodeCompare_t b2;
  boost::fusion::for_each( b2, NodeFactoryInit(m_vNodeFactoryBoolean) );
  boost::fusion::for_each( b2, NodeFactoryInit(m_vNodeFactoryAllNodes) );

  NodeDouble_t d1;
  boost::fusion::for_each( d1, NodeFactoryInit(m_vNodeFactoryDouble) );
  boost::fusion::for_each( d1, NodeFactoryInit(m_vNodeFactoryAllNodes) );

}

TreeBuilder::~TreeBuilder(void) {
}

Node* TreeBuilder::CreateChild( bool bUseTerminal, bool bUseNode, unsigned int nDepth, unsigned int nMaxDepth, const vNodeFactory_t& v ) {

    /*               | !depth==max | depth==max |
      *               |-------------|------------|
      * !term * !node | illegal     | illegal    |
      *  term * !node | term        | term       |
      * !term *  node | node        | term       |
      *  term *  node | node, term  | term       |
      */

  Node* node( 0 );

  bool bEnd( false );
  bool bTerminal = ( ( 1 == nDepth ) && ( nDepth < nMaxDepth ) ) ? false : bUseTerminal; // don't use a terminal on first node
  boost::random::uniform_int_distribution<unsigned int> dist( 0, v.size() - 1 );
  while ( !bEnd ) {
    vNodeFactory_t::size_type ix( dist( m_rng ) );
    node = v[ ix ]();
    if ( bUseNode && ( nDepth < nMaxDepth ) ) {
      if ( !bTerminal && node->IsTerminal() ) {
        // reject and try another;
      }
      else {
        bEnd = true;
      }
    }
    else {
      if ( node->IsTerminal() ) {
        bEnd = true;
      }
      else {
        // reject and try another;
      }
    }
    if ( !bEnd ) {
      delete node;
      node = 0;
    }
  }
  assert( 0 != node );

  return node;
}

void TreeBuilder::AddRandomChildren( 
  Node& node, bool bUseTerminal, bool bUseNode, unsigned int nDepth, unsigned int nMaxDepth ) {
  assert( !bUseTerminal && !bUseNode ); // use one or both
  assert( nDepth <= nMaxDepth ); 
  vNodeFactory_t& vFactory( *m_rNodeFactories[ node.ChildType() ] );
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

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

#include <algorithm>

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

//struct SortNodeFactoryOnBoolean

TreeBuilder::TreeBuilder(void) 
  : m_rng( std::time( 0 ) )  // possible issue after jan 18, 2038?
{

  NodeBoolean_t b1;
  boost::fusion::for_each( b1, NodeFactoryInit( m_vNodeFactoryBooleanAll, m_vNodeFactoryBooleanTerminals, m_vNodeFactoryBooleanNodes ) );

  NodeCompare_t b2;
  boost::fusion::for_each( b2, NodeFactoryInit( m_vNodeFactoryBooleanAll, m_vNodeFactoryBooleanTerminals, m_vNodeFactoryBooleanNodes ) );

  NodeDouble_t d1;
  boost::fusion::for_each( d1, NodeFactoryInit( m_vNodeFactoryDoubleAll, m_vNodeFactoryDoubleTerminals, m_vNodeFactoryDoubleNodes ) );

  m_rNodeFactories[ NodeType::Bool ][ FactoryType::All ] = &m_vNodeFactoryBooleanAll;
  m_rNodeFactories[ NodeType::Bool ][ FactoryType::Terminals ] = &m_vNodeFactoryBooleanTerminals;
  m_rNodeFactories[ NodeType::Bool ][ FactoryType::Nodes ] = &m_vNodeFactoryBooleanNodes;

  m_rNodeFactories[ NodeType::Double ][ FactoryType::All ] = &m_vNodeFactoryDoubleAll;
  m_rNodeFactories[ NodeType::Double ][ FactoryType::Terminals ] = &m_vNodeFactoryDoubleTerminals;
  m_rNodeFactories[ NodeType::Double ][ FactoryType::Nodes ] = &m_vNodeFactoryDoubleNodes;

}

TreeBuilder::~TreeBuilder(void) {
}

Node* TreeBuilder::CreateChild( NodeType::E nt, bool bUseTerminal, bool bUseNode, unsigned int nDepth, unsigned int nMaxDepth ) {

    /*               | !depth==max | depth==max |
      *               |-------------|------------|
      * !term * !node | illegal     | illegal    |
      *  term * !node | term        | term       |
      * !term *  node | node        | term       |
      *  term *  node | node, term  | term       |
      */

  Node* node( 0 );

  FactoryType::E ft( FactoryType::All ); // default to mixture of nodes and terminals

  if ( nDepth == nMaxDepth ) {
    ft = FactoryType::Terminals; // force terminal on last level
  }
  else {
    if ( ( 1 == nDepth ) && ( nDepth < nMaxDepth ) ) {
      ft = FactoryType::Nodes; // don't use terminal on first node, if more levels available
    }
    else {
      if ( !bUseTerminal ) {
        ft = FactoryType::Nodes;  // make sure most levels are nodes
      }
    }
  }

  vNodeFactory_t& vFactory( *m_rNodeFactories[ nt ][ ft ] );

  boost::random::uniform_int_distribution<unsigned int> dist( 0, vFactory.size() - 1 );
  vNodeFactory_t::size_type ix( dist( m_rng ) );
  node = vFactory[ ix ]();

  return node;
}

void TreeBuilder::AddRandomChildren( 
  Node& node, bool bUseTerminal, bool bUseNode, unsigned int nDepth, unsigned int nMaxDepth ) {
  assert( bUseTerminal || bUseNode ); // use one or both
  assert( nDepth <= nMaxDepth ); 
  NodeType::E nt( node.ChildType() );
  switch ( node.NodeCount() ) {
  case 0: // nothing, this is terminal node
    assert( node.IsTerminal() );
    break;
  case 1:
    node.AddCenter( CreateChild( nt, bUseTerminal, bUseNode, nDepth, nMaxDepth ) );
    if ( !node.ChildCenter().IsTerminal() ) {
      AddRandomChildren( node.ChildCenter(), bUseTerminal, bUseNode, nDepth + 1, nMaxDepth );
    }
    break;
  case 2: 
    node.AddLeft( CreateChild( nt, bUseTerminal, bUseNode, nDepth, nMaxDepth ) );
    if ( !node.ChildLeft().IsTerminal() ) {
      AddRandomChildren( node.ChildLeft(), bUseTerminal, bUseNode, nDepth + 1, nMaxDepth );
    }
    node.AddRight( CreateChild( nt, bUseTerminal, bUseNode, nDepth, nMaxDepth ) );
    if ( !node.ChildRight().IsTerminal() ) {
      AddRandomChildren( node.ChildRight(), bUseTerminal, bUseNode, nDepth + 1, nMaxDepth );
    }
    break;
  }
}

} // namespace gp
} // namespace ou

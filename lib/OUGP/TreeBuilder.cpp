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

#include "TreeBuilder.h"

namespace ou { // One Unified
namespace gp { // genetic programming

//struct SortNodeFactoryOnBoolean

TreeBuilder::TreeBuilder(void) 
  : m_rng( std::time( 0 ) )  // possible issue after jan 18, 2038?
{

  RegisterBoolean<NodeTypesBoolean_t>();
  RegisterBoolean<NodeTypesCompare_t>();
  RegisterDouble<NodeTypesDouble_t>();

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

  // determine what type of Node to Construct

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

  // choose factory based upon factors computed above
  vNodeFactory_t& vFactory( *m_rNodeFactories[ nt ][ ft ] );

  // construct the node
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

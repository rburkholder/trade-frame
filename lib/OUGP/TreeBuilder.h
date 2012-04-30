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

#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/value_at.hpp>

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

  template<typename S> // S=fusion sequence
  Node* CreateChild( bool bUseTerminal, bool bUseNode, unsigned int nDepth, unsigned int nMaxDepth ) {
    Node* node( 0 );
/*
    typedef boost::fusion::result_of::size<S> size_of_s_type;
//    size_of_s_type::type sizeofS( size_of_s_type::value );
    unsigned int sizeofS( size_of_s_type::value );
    bool bEnd( false );
    bool bTerminal = ( ( 1 == nDepth ) && ( nDepth < nMaxDepth ) ) ? false : bUseTerminal; // don't use a terminal on first node
    while ( !bEnd ) {
      boost::random::uniform_int_distribution<unsigned int> dist( 0, size_of_s_type::value - 1 );
      unsigned int ix( dist( m_rng ) );
      node = new boost::fusion::result_of::value_at_c<S,dist( m_rng )>;
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
*/
    return node;
  }

  template<typename S> // S is fusion view of node types
  void AddRandomChildren( 
    Node& node, bool bUseTerminal, bool bUseNode, unsigned int Depth, unsigned int MaxDepth ) {
    assert( !bUseTerminal && !bUseNode ); // use one or both
    assert( nDepth <= nMaxDepth ); 
    switch ( node.NodeCount() ) {
    case 0: // nothing
      break;
    case 1:
      node.AddCenter( CreateChild<T>( bUseTerminal, bUseNode, nDepth, nMaxDepth ) );
      if ( !node.ChildCenter().IsTerminal() ) {
      }
      break;
    case 2: 
      node.AddLeft( CreateChild<T>( bUseTerminal, bUseNode, nDepth, nMaxDepth ) );
      node.AddRight( CreateChild<T>( bUseTerminal, bUseNode, nDepth, nMaxDepth ) );
      break;
    }
  }

protected:
private:
  boost::random::mt19937 m_rng;
  typedef std::vector<Node* (*)()> vNodeFactory_t;
  vNodeFactory_t m_vNodeFactoryBoolean;
  vNodeFactory_t m_vNodeFactoryDouble;

};

} // namespace gp
} // namespace ou

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
private:
  typedef std::vector<Node* (*)()> vNodeFactory_t; 
public:

  TreeBuilder(void);
  ~TreeBuilder(void);

  template<typename V> // V vector of CreateNode calls
  Node* CreateChild( bool bUseTerminal, bool bUseNode, unsigned int nDepth, unsigned int nMaxDepth, const V& v ) {

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
    while ( !bEnd ) {
      boost::random::uniform_int_distribution<unsigned int> dist( 0, v.size() - 1 );
      V::size_type ix( dist( m_rng ) );
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

  void AddRandomChildren( Node& node, bool bUseTerminal, bool bUseNode, unsigned int nDepth, unsigned int nMaxDepth );

protected:
private:

  boost::random::mt19937 m_rng;
  vNodeFactory_t m_vNodeFactoryAllNodes;
  vNodeFactory_t m_vNodeFactoryBoolean; 
  vNodeFactory_t m_vNodeFactoryDouble;

  vNodeFactory_t* m_vNodeFactories[ NodeType::Count ];

};

} // namespace gp
} // namespace ou

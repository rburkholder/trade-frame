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

  template<class T>
  Node* CreateNode( void ) {
    return new T();
  }

struct NodeFactoryInit {
  template<typename T>
  void operator()( T& t ) const {
    v.push_back( &CreateNode<T> );
  }
  NodeFactoryInit( std::vector<Node* (*)()>& v_ ): v( v_ ) {}
  std::vector<Node* (*)()>& v;
};

TreeBuilder::TreeBuilder(void) 
  : m_rng( std::time( 0 ) )  // possible issue after jan 18, 2038?
{
  NodeBoolean_t b1;
  boost::fusion::for_each( b1, NodeFactoryInit(m_vNodeFactoryBoolean) );
  NodeCompare_t b2;
  boost::fusion::for_each( b2, NodeFactoryInit(m_vNodeFactoryBoolean) );
  NodeDouble_t d1;
  boost::fusion::for_each( d1, NodeFactoryInit(m_vNodeFactoryDouble) );
}

TreeBuilder::~TreeBuilder(void) {
}

} // namespace gp
} // namespace ou

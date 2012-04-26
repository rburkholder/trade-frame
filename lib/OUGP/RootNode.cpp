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

#include "RootNode.h"

namespace ou { // One Unified
namespace gp { // genetic programming

// ********* RootNode *********

RootNode::RootNode(void): Node<RootNode>() {
  m_cntNodes = 1;
}

RootNode::~RootNode(void) {
}

bool RootNode::EvaluateBooleanImpl( void ) const {
  assert( 0 != m_pChildCenter );
  return m_pChildCenter->EvaluateBoolean();
}

} // namespace gp
} // namespace ou

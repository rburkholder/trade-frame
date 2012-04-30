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

RootNode::RootNode(void): Node() {
  m_cntNodes = 1;
}

RootNode::~RootNode(void) {
}

bool RootNode::EvaluateBoolean( void ) const {
  return ChildCenter().EvaluateBoolean();
}

} // namespace gp
} // namespace ou

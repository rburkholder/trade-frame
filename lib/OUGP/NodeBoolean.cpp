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

#include <cassert>

#include "NodeBoolean.h"

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

// ********* NodeBooleanFalse *********

NodeBooleanFalse::NodeBooleanFalse( void ) : Node<NodeBooleanFalse>() {
  m_cntNodes = 0;
}

NodeBooleanFalse::~NodeBooleanFalse( void ) {
}

// ********* NodeBooleanTrue *********

NodeBooleanTrue::NodeBooleanTrue( void ) : Node<NodeBooleanTrue>() {
  m_cntNodes = 0;
}

NodeBooleanTrue::~NodeBooleanTrue( void ) {
}

// ********* NodeBooleanNot *********

NodeBooleanNot::NodeBooleanNot( void ) : Node<NodeBooleanNot>() {
  m_cntNodes = 1;
}

NodeBooleanNot::~NodeBooleanNot( void ) {
}

bool NodeBooleanNot::EvaluateBooleanImpl( void ) const {
  assert( 0 != m_pChildCenter );
  return !m_pChildCenter->EvaluateBoolean();
}

// ********* NodeBooleanAnd *********

NodeBooleanAnd::NodeBooleanAnd( void ) : Node<NodeBooleanAnd>() {
  m_cntNodes = 2;
}

NodeBooleanAnd::~NodeBooleanAnd( void ) {
}

bool NodeBooleanAnd::EvaluateBooleanImpl( void ) const {
  assert( 0 != m_pChildLeft );
  assert( 0 != m_pChildRight );
  bool b1 = m_pChildLeft->EvaluateBoolean();
  bool b2 = m_pChildRight->EvaluateBoolean();
  return b1 && b2;
}

// ********* NodeBooleanOr *********

NodeBooleanOr::NodeBooleanOr( void ) : Node<NodeBooleanOr>() {
  m_cntNodes = 2;
}

NodeBooleanOr::~NodeBooleanOr( void ) {
}

bool NodeBooleanOr::EvaluateBooleanImpl( void ) const {
  assert( 0 != m_pChildLeft );
  assert( 0 != m_pChildRight );
  bool b1 = m_pChildLeft->EvaluateBoolean();
  bool b2 = m_pChildRight->EvaluateBoolean();
  return b1 || b2;
}

} // namespace gp
} // namespace ou

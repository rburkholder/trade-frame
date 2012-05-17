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

// ********* NodeBooleanFalse *********

NodeBooleanFalse::NodeBooleanFalse( void ) : NodeBoolean<NodeBooleanFalse>() {
  m_cntNodes = 0;
}

NodeBooleanFalse::~NodeBooleanFalse( void ) {
}

// ********* NodeBooleanTrue *********

NodeBooleanTrue::NodeBooleanTrue( void ) : NodeBoolean<NodeBooleanTrue>() {
  m_cntNodes = 0;
}

NodeBooleanTrue::~NodeBooleanTrue( void ) {
}

// ********* NodeBooleanNot *********

NodeBooleanNot::NodeBooleanNot( void ) : NodeBoolean<NodeBooleanNot>() {
  m_cntNodes = 1;
}

NodeBooleanNot::~NodeBooleanNot( void ) {
}

//bool NodeBooleanNot::EvaluateBooleanImpl( void ) const {
bool NodeBooleanNot::EvaluateBoolean( void ) {
  return !ChildCenter().EvaluateBoolean();
}

// ********* NodeBooleanAnd *********

NodeBooleanAnd::NodeBooleanAnd( void ) : NodeBoolean<NodeBooleanAnd>() {
  m_cntNodes = 2;
}

NodeBooleanAnd::~NodeBooleanAnd( void ) {
}

//bool NodeBooleanAnd::EvaluateBooleanImpl( void ) const {
bool NodeBooleanAnd::EvaluateBoolean( void ) {
  bool b1 = ChildLeft().EvaluateBoolean();
  bool b2 = ChildRight().EvaluateBoolean();
  return b1 && b2;
}

// ********* NodeBooleanOr *********

NodeBooleanOr::NodeBooleanOr( void ) : NodeBoolean<NodeBooleanOr>() {
  m_cntNodes = 2;
}

NodeBooleanOr::~NodeBooleanOr( void ) {
}

//bool NodeBooleanOr::EvaluateBooleanImpl( void ) const {
bool NodeBooleanOr::EvaluateBoolean( void ) {
  bool b1 = ChildLeft().EvaluateBoolean();
  bool b2 = ChildRight().EvaluateBoolean();
  return b1 || b2;
}

} // namespace gp
} // namespace ou

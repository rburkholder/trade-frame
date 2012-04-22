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

#include "NodeCompare.h"

namespace ou { // One Unified
namespace gp { // genetic programming

// ********* NodeCompareGT *********

NodeCompareGT::NodeCompareGT( void ) : Node<NodeCompareGT>() {
  m_cntNodes = 2;
}

NodeCompareGT::~NodeCompareGT( void ) {
}

bool NodeCompareGT::EvaluateBooleanImpl( void ) const {
  assert( 0 != m_pChildLeft );
  assert( 0 != m_pChildRight );
  double d1 = m_pChildLeft->EvaluateBoolean();
  double d2 = m_pChildRight->EvaluateBoolean();
  return d1 > d2;
}

// ********* NodeCompareGE *********

NodeCompareGE::NodeCompareGE( void ) : Node<NodeCompareGE>() {
  m_cntNodes = 2;
}

NodeCompareGE::~NodeCompareGE( void ) {
}

bool NodeCompareGE::EvaluateBooleanImpl( void ) const {
  assert( 0 != m_pChildLeft );
  assert( 0 != m_pChildRight );
  double d1 = m_pChildLeft->EvaluateBoolean();
  double d2 = m_pChildRight->EvaluateBoolean();
  return d1 >= d2;
}

// ********* NodeCompareLT *********

NodeCompareLT::NodeCompareLT( void ) : Node<NodeCompareLT>() {
  m_cntNodes = 2;
}

NodeCompareLT::~NodeCompareLT( void ) {
}

bool NodeCompareLT::EvaluateBooleanImpl( void ) const {
  assert( 0 != m_pChildLeft );
  assert( 0 != m_pChildRight );
  double d1 = m_pChildLeft->EvaluateBoolean();
  double d2 = m_pChildRight->EvaluateBoolean();
  return d1 < d2;
}

// ********* NodeCompareLE *********

NodeCompareLE::NodeCompareLE( void ) : Node<NodeCompareLE>() {
  m_cntNodes = 2;
}

NodeCompareLE::~NodeCompareLE( void ) {
}

bool NodeCompareLE::EvaluateBooleanImpl( void ) const {
  assert( 0 != m_pChildLeft );
  assert( 0 != m_pChildRight );
  double d1 = m_pChildLeft->EvaluateBoolean();
  double d2 = m_pChildRight->EvaluateBoolean();
  return d1 <= d2;
}

} // namespace gp
} // namespace ou


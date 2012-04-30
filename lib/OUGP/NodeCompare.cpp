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

NodeCompareGT::NodeCompareGT( void ) : Node() {
  m_cntNodes = 2;
}

NodeCompareGT::~NodeCompareGT( void ) {
}

bool NodeCompareGT::EvaluateBoolean( void ) const {
  double d1 = ChildLeft().EvaluateDouble();
  double d2 = ChildRight().EvaluateDouble();
  return d1 > d2;
}

// ********* NodeCompareGE *********

NodeCompareGE::NodeCompareGE( void ) : Node() {
  m_cntNodes = 2;
}

NodeCompareGE::~NodeCompareGE( void ) {
}

bool NodeCompareGE::EvaluateBoolean( void ) const {
  double d1 = ChildLeft().EvaluateDouble();
  double d2 = ChildRight().EvaluateDouble();
  return d1 >= d2;
}

// ********* NodeCompareLT *********

NodeCompareLT::NodeCompareLT( void ) : Node() {
  m_cntNodes = 2;
}

NodeCompareLT::~NodeCompareLT( void ) {
}

bool NodeCompareLT::EvaluateBoolean( void ) const {
  double d1 = ChildLeft().EvaluateDouble();
  double d2 = ChildRight().EvaluateDouble();
  return d1 < d2;
}

// ********* NodeCompareLE *********

NodeCompareLE::NodeCompareLE( void ) : Node() {
  m_cntNodes = 2;
}

NodeCompareLE::~NodeCompareLE( void ) {
}

bool NodeCompareLE::EvaluateBoolean( void ) const {
  double d1 = ChildLeft().EvaluateDouble();
  double d2 = ChildRight().EvaluateDouble();
  return d1 <= d2;
}

} // namespace gp
} // namespace ou


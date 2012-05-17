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
#include <math.h>

#include "NodeDouble.h"

namespace ou { // One Unified
namespace gp { // genetic programming

//NodeDouble::NodeDouble(void) {
//}

//NodeDouble::~NodeDouble(void) {
//}

// ********* NodeDoubleAdd *********

NodeDoubleAdd::NodeDoubleAdd( void ) : NodeDouble<NodeDoubleAdd>() {
  m_cntNodes = 2;
}

NodeDoubleAdd::~NodeDoubleAdd( void ) {
}

double NodeDoubleAdd::EvaluateDouble( void ) {
  double d1 = ChildLeft().EvaluateDouble();
  double d2 = ChildRight().EvaluateDouble();
  return d1 + d2;
}

// ********* NodeDoubleSub *********

NodeDoubleSub::NodeDoubleSub( void ) : NodeDouble<NodeDoubleSub>() {
  m_cntNodes = 2;
}

NodeDoubleSub::~NodeDoubleSub( void ) {
}

double NodeDoubleSub::EvaluateDouble( void ) {
  double d1 = ChildLeft().EvaluateDouble();
  double d2 = ChildRight().EvaluateDouble();
  return d1 - d2;
}

// ********* NodeDoubleMlt *********

NodeDoubleMlt::NodeDoubleMlt( void ) : NodeDouble<NodeDoubleMlt>() {
  m_cntNodes = 2;
}

NodeDoubleMlt::~NodeDoubleMlt( void ) {
}

double NodeDoubleMlt::EvaluateDouble( void ) {
  double d1 = ChildLeft().EvaluateDouble();
  double d2 = ChildRight().EvaluateDouble();
  return d1 * d2;
}

// ********* NodeDoubleDvd *********

NodeDoubleDvd::NodeDoubleDvd( void ) : NodeDouble<NodeDoubleDvd>() {
  m_cntNodes = 2;
}

NodeDoubleDvd::~NodeDoubleDvd( void ) {
}

double NodeDoubleDvd::EvaluateDouble( void ) {
  double d1 = ChildLeft().EvaluateDouble();
  double d2 = ChildRight().EvaluateDouble();
  return ( 0.0 == d2 ) ? HUGE : d1 / d2;
}

} // namespace gp
} // namespace ou

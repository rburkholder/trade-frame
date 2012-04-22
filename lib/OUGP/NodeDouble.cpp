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

NodeDoubleAdd::NodeDoubleAdd( void ) : Node<NodeDoubleAdd>() {
  m_cntNodes = 2;
}

NodeDoubleAdd::~NodeDoubleAdd( void ) {
}

double NodeDoubleAdd::EvaluateDoubleImpl( void ) const {
  assert( 0 != m_pChildLeft );
  assert( 0 != m_pChildRight );
  double d1 = m_pChildLeft->EvaluateDouble();
  double d2 = m_pChildRight->EvaluateDouble();
  return d1 + d2;
}

// ********* NodeDoubleSub *********

NodeDoubleSub::NodeDoubleSub( void ) : Node<NodeDoubleSub>() {
  m_cntNodes = 2;
}

NodeDoubleSub::~NodeDoubleSub( void ) {
}

double NodeDoubleSub::EvaluateDoubleImpl( void ) const {
  assert( 0 != m_pChildLeft );
  assert( 0 != m_pChildRight );
  double d1 = m_pChildLeft->EvaluateDouble();
  double d2 = m_pChildRight->EvaluateDouble();
  return d1 - d2;
}

// ********* NodeDoubleMlt *********

NodeDoubleMlt::NodeDoubleMlt( void ) : Node<NodeDoubleMlt>() {
  m_cntNodes = 2;
}

NodeDoubleMlt::~NodeDoubleMlt( void ) {
}

double NodeDoubleMlt::EvaluateDoubleImpl( void ) const {
  assert( 0 != m_pChildLeft );
  assert( 0 != m_pChildRight );
  double d1 = m_pChildLeft->EvaluateDouble();
  double d2 = m_pChildRight->EvaluateDouble();
  return d1 * d2;
}

// ********* NodeDoubleDvd *********

NodeDoubleDvd::NodeDoubleDvd( void ) : Node<NodeDoubleDvd>() {
  m_cntNodes = 2;
}

NodeDoubleDvd::~NodeDoubleDvd( void ) {
}

double NodeDoubleDvd::EvaluateDoubleImpl( void ) const {
  assert( 0 != m_pChildLeft );
  assert( 0 != m_pChildRight );
  double d1 = m_pChildLeft->EvaluateDouble();
  double d2 = m_pChildRight->EvaluateDouble();
  return ( 0.0 == d2 ) ? HUGE : d1 / d2;
}

} // namespace gp
} // namespace ou

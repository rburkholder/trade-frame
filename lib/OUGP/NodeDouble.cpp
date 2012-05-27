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

#include <boost/random.hpp>
#include <boost/random/uniform_real_distribution.hpp>

#include "NodeDouble.h"

namespace ou { // One Unified
namespace gp { // genetic programming
namespace rng { // random number generator
boost::random::mt19937 rng( std::time( 0 ) );
boost::random::uniform_real_distribution<double> m_urd( -1.0, 1.0 );
double calc( void ) {
  return m_urd( rng );
}
} // namespace rng

//NodeDouble::NodeDouble(void) {
//}

//NodeDouble::~NodeDouble(void) {
//}

// ********* NodeDoubleZero *********

NodeDoubleZero::NodeDoubleZero( void ) : NodeDouble<NodeDoubleZero>() {
  m_cntNodes = 0;
}

NodeDoubleZero::~NodeDoubleZero( void ) {
}

double NodeDoubleZero::EvaluateDouble( void ) {
  return 0.0;
}

// ********* NodeDoubleRandom *********

NodeDoubleRandom::NodeDoubleRandom( void ) : NodeDouble<NodeDoubleRandom>(), m_val( rng::calc() ) {
  m_cntNodes = 0;
}

NodeDoubleRandom::NodeDoubleRandom( const NodeDoubleRandom& rhs ): NodeDouble<NodeDoubleRandom>(), m_val( rhs.m_val ) {
  m_cntNodes = 0;
}

NodeDoubleRandom& NodeDoubleRandom::operator=( const NodeDoubleRandom& rhs ) {
  if ( this != &rhs ) {
    m_cntNodes = 0;
    m_val = rhs.m_val;
  }
  return *this;
}

NodeDoubleRandom::~NodeDoubleRandom( void ) {
}

double NodeDoubleRandom::EvaluateDouble( void ) {
  return m_val;
}

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

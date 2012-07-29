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

#pragma once

#include <boost/fusion/container/vector.hpp>

#include "node.h"

namespace ou { // One Unified
namespace gp { // genetic programming
namespace rng {
  extern boost::random::mt19937 common;
}

class NodeDoubleZero: public NodeDouble<NodeDoubleZero> {
public:
  NodeDoubleZero( void );
  ~NodeDoubleZero( void );
  void ToString( std::stringstream& ss ) const { ss << "0.0"; };
  double EvaluateDouble( void );
protected:
private:
};

class NodeDoubleRandom: public NodeDouble<NodeDoubleRandom> {
public:
  NodeDoubleRandom( void );
  NodeDoubleRandom( const NodeDoubleRandom& rhs );
  NodeDoubleRandom& operator=( const NodeDoubleRandom& rhs );
  ~NodeDoubleRandom( void );
  void ToString( std::stringstream& ss ) const { ss << m_val; };
  double EvaluateDouble( void );
protected:
private:
  double m_val;
};

class NodeDoubleAbs: public NodeDouble<NodeDoubleAbs> {
public:
  NodeDoubleAbs( void );
  NodeDoubleAbs( const NodeDoubleAbs& rhs );
  NodeDoubleAbs& operator=( const NodeDoubleAbs& rhs );
  ~NodeDoubleAbs( void );
  void ToString( std::stringstream& ss ) const { ss << "abs"; };
  double EvaluateDouble( void );
protected:
private:
};

class NodeDoubleAdd: public NodeDouble<NodeDoubleAdd> {
public:
  NodeDoubleAdd( void );
  ~NodeDoubleAdd( void );
  void ToString( std::stringstream& ss ) const { ss << "+"; };
  double EvaluateDouble( void );
protected:
private:
};

class NodeDoubleSub: public NodeDouble<NodeDoubleSub> {
public:
  NodeDoubleSub( void );
  ~NodeDoubleSub( void );
  void ToString( std::stringstream& ss ) const { ss << "-"; };
  double EvaluateDouble( void );
protected:
private:
};

class NodeDoubleMlt: public NodeDouble<NodeDoubleMlt> {
public:
  NodeDoubleMlt( void );
  ~NodeDoubleMlt( void );
  void ToString( std::stringstream& ss ) const { ss << "*"; };
  double EvaluateDouble( void );
protected:
private:
};

class NodeDoubleDvd: public NodeDouble<NodeDoubleDvd> {
public:
  NodeDoubleDvd( void );
  ~NodeDoubleDvd( void );
  void ToString( std::stringstream& ss ) const { ss << "/"; };
  double EvaluateDouble( void );
protected:
private:
};

//typedef boost::fusion::vector<NodeDoubleZero, NodeDoubleRandom, NodeDoubleAbs, NodeDoubleAdd, NodeDoubleSub, NodeDoubleMlt, NodeDoubleDvd> NodeTypesDouble_t;
// remove NodeDoubleAbs for now
typedef boost::fusion::vector<NodeDoubleZero, NodeDoubleRandom, NodeDoubleAdd, NodeDoubleSub, NodeDoubleMlt, NodeDoubleDvd> NodeTypesDouble_t;


} // namespace gp
} // namespace ou
